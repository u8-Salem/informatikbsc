#!/usr/bin/env python3

import subprocess
import tempfile
import os
import sys
import shutil
import logging
import re
import yaml
import logging
import random
import signal
import threading
logging.addLevelName( logging.WARNING, "\033[1;33m%s\033[1;0m" % logging.getLevelName(logging.WARNING))
logging.addLevelName( logging.ERROR, "\033[1;41m%s\033[1;0m" % logging.getLevelName(logging.ERROR))
logging.addLevelName( logging.INFO, "\033[1;34m%s\033[1;0m" % logging.getLevelName(logging.INFO))

SCRIPT_ROOT = os.path.dirname(__file__)
GDB_SCRIPT_TRACE = os.path.join(SCRIPT_ROOT, "gdb_trace.py")
GDB = os.environ.get("USE_GDB")

class MissingRequirementError(RuntimeError):pass
class LocalError(RuntimeError):pass
class FatalError(RuntimeError):pass

class Testcase:
    def __init__(self, testcase):
        self.testcase = testcase
        self.variables = {'Compilation': Compilation,
                          'os':os,
                          'logging':logging,
                          'LocalError': LocalError,
                          'FatalError': FatalError,
                          'MissingRequirementerror': MissingRequirementError,
        }
        Compilation.globals = self.variables
        self.failed = False
        self.skipped_tests = 0
        self.skip_tests = False
        self.earned_points = 0

        with open(testcase) as fd:
            content = fd.read()
            self.load(content, execute=True)

    def load(self, content, execute=False):
        block_types = {
            '!yaml': self.block_yaml,
            '!source': self.block_source,
            '!python': self.block_nop,
            '!inherit': self.block_inherit,
        }
        if execute:
            block_types['!python'] = self.block_python

        blocks = re.split("\n---", "\n"+content)
        blocks = [x.lstrip() for x in blocks if x]
        # For every block, we call the block_ function with the given arguments
        for block in blocks:
            lines = (block + "\n").split("\n",1)
            block_args = lines[0].split()
            if block_args[0] not in block_types:
                sys.exit("Block type %s is unknown" % block_args[0])

            ret = block_types[block_args[0]](lines[1],*block_args[1:], skip=self.skip_tests)
            if ret:
                success, skip = ret
                if success is False:
                    self.failed = True
                if skip is True:
                    self.skip_tests = True
                if success == 'skipped':
                    self.skipped_tests += 1


    def block_nop(self, *args, **kwargs):
        pass

    def block_yaml(self, block, *args, **kwargs):
        new_vars = yaml.safe_load(block)
        if not new_vars:
            return True, False
        self.variables.update(new_vars)
        if 'requirements' in new_vars:
            try:
                self.skip_tests = False
                Compilation.check_requirements(new_vars['requirements'])
            except MissingRequirementError as e:
                self.skip_tests = True
                logging.warning(e)
                return False, True
        return True, False

    def block_source(self, block, variable, *args, **kwargs):
        self.variables[variable.strip()] = block
        return True, False

    def block_inherit(self, block, other_testcase, *args, **kwargs):
        basedir = os.path.dirname(self.testcase)
        with open(os.path.join(basedir, other_testcase)) as fd:
            content = fd.read()
            self.load(content, execute=False)
        return True, False


    def block_python(self, block, *args, **kwargs):
        if self.skip_tests:
            logging.warning('... Subtest: "%s" skipped', " ".join(args))
            return 'skipped', False
        if args:
            logging.info('... Subtest: "%s"', " ".join(args))
        try:
            exec(block, self.variables)
        except TypeError as e:
            raise e
        except MissingRequirementError as e:
            logging.warning(e)
            return 'skipped', False
        except RuntimeError as e:
            m = self.variables.get('malus',0) or self.variables.get('global_malus', 0)
            self.earned_points -= m
            pts = " (-%s points)" % m if m else ""
            logging.error("testcase failed%s: %s, %s", pts, self.testcase, e)
            if "action_desc" in self.variables:
                logging.info("Testcase content: %s", self.variables["action_desc"])
            if "expect_desc" in self.variables:
                logging.info("Testcase expectations: %s", self.variables["expect_desc"])
            return False, False
        except RuntimeWarning as e:
            logging.warning(e)
            return 'skipped', False
        except subprocess.TimeoutExpired as e:
            logging.error("testcase failed by timeout: %s, %s", self.testcase, e)
            out = e.stdout or ""
            err = e.stderr or ""
            logging.error("STDOUT:\n%s\nSTDERR:\n%s", out, red(err))
            return False, False
        else:
            b = self.variables.get('bonus',0) or self.variables.get('global_bonus', 0)
            self.earned_points += b
            logging.info(f"...    OK ({b} points)" if b else "...    OK")
        finally:
            self.variables['bonus'] = 0
            self.variables['malus'] = 0
        return True, False



gdb_wait_snippet ="""
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
void handler(int sig){}
void __attribute__ ((constructor)) wait_for_gdb() {
  signal(SIGPROF, &handler);
  if (getenv("WAIT_FOR_GDB")) {
    // We are stoping this process untill gdb sends "continue"
    pause();
  }
}
"""


def red(msg):
    return "\x1b[31;4m%s\x1b[0m" % msg

class Compilation:
    """Compilation is executed in the context of a testcase"""

    globals = None

    instances = []

    no_strace = False

    def __init__(self, after_main = None, before_main=None, source_files=None):
        if source_files is None:
            if 'sources' in Compilation.globals:
                source_files = Compilation.globals['sources']
            else:
                sys.exit("Invalid Testcase: No sources given")
        assert type(source_files) is dict, "source_files must be a dict"
        self.source_files = source_files
        for fn in source_files:
            assert os.path.exists(fn) or 'content' in source_files[fn], "File %s does not exist" % fn

        self.after_main = after_main
        self.before_main = before_main
        self.tmpdir = tempfile.mkdtemp(prefix="gbs_test_")
        # We use a random number as a flag to replace {{{FINISHED}}}
        # in the output.
        self.flag = str(random.randint(0, 10000000))
        Compilation.instances.append(self)

    @classmethod
    def check_requirements(cls, requirements):
        if os.environ.get('IGNORE_REQUIREMENTS', False):
            return
        for req in requirements:
            if not cls.is_implemented(req):
                msg = f'Requirement "{req}" is missing. Remove marker to activate testcase.'
                raise MissingRequirementError(msg)

    @classmethod
    def is_implemented(cls, marker):
        sources = cls.globals.get('sources', None)
        if not sources:
            return False
        for fn in sources:
            if 'content' in sources[fn]:
                content = sources[fn]['content']
            else:
                with open(fn) as fd:
                    content = fd.read()
            if f"{marker}_NOT_IMPLEMENTED_MARKER" in content:
                return False
        return True


    def fail_marker(self, marker, msg):
        if not self.after_main:
            raise RuntimeError(msg)
        lines = self.after_main.split("\n")
        for idx, line in enumerate(lines):
            if "Marker " + str(marker) in line:
                lines[idx] = "// --> " + red("Error is around here")
            elif "Marker" in line:
                lines[idx] = None
        logging.error(red(msg) + "\n" + "\n".join([x for x in lines if x is not None]))

        raise RuntimeError(msg)

    def compile(self, flags=[], remap=None):
        try:
            # There are default flags
            flags += Compilation.globals.get('cflags', [])
            return self.__compile(flags, remap)
        except subprocess.CalledProcessError as e:
            logging.error("Compilation failed: %s", " ".join(e.args[1]))
            raise RuntimeError("Compilation Failed")

    def __compile(self, flags=[], remap=None):
        if remap is None:
            remap = {}

        if self.after_main:
            remap['main'] = 'studentMain'

        line = '#line 1 '
        if GDB:
            self.source_files["wait_for_gdb.c"] = {'content':gdb_wait_snippet}
            line = '//# line 1 '

        found_main = False
        for fn in self.source_files:
            src, dst = fn, os.path.join(self.tmpdir, os.path.basename(fn))

            if 'content' in self.source_files[fn]:
                content = self.source_files[fn]['content']
            else:
                with open(src) as fd:
                    content = fd.read()

            if self.source_files[src].get('main'):
                found_main = True
                before = ""
                after = ""
                for oldname, newname in remap.items():
                    before += "#define %s %s\n" % (oldname, newname)
                    after  += "#undef %s\n" %(oldname)

                if self.before_main:
                    before = self.before_main + before

                if self.after_main:
                    after += self.after_main

                content = \
                     line + '"<<before_main>>"\n'  \
                    + before \
                    + line + '"%s"\n' % src \
                    + content \
                    + '\n' + line + '"<<after_main>>"\n'  \
                    + after.replace("{{{FINISHED}}}", self.flag)

            with open(dst, "w+") as fd:
                fd.write(content)

        assert found_main, "One source file must be attributed with 'main: true'"

        object_files = []
        logging.debug("Compile sources: %s", self.source_files)
        for fn in self.source_files:
            if not fn.endswith(".c"):
                continue
            obj = os.path.join(self.tmpdir, os.path.basename(fn) + ".o")
            subprocess.check_output(["gcc"] + flags + ["-c", "-o", obj,
                                                       os.path.join(self.tmpdir, fn)])
            object_files += [obj]
        main = os.path.join(self.tmpdir, 'main')

        logging.debug("Link objects: %s", object_files)

        subprocess.check_output(["gcc"] + flags + object_files + ["-o", main])
        self.main = main

        return self

    def run(self, args = [], cmd_prefix = [],
            must_fail=False,
            retcode_expected = lambda retcode: retcode != 0,
            input=None, **kwargs):
        input = input.encode() if type(input) == str else input
        input_ = input.replace(b"{{{FINISHED}}}", self.flag.encode()) if type(input) == bytes else None
        (retcode, cmd, stdout, stderr) = self.__run(args, cmd_prefix, input=input_, **kwargs)
        stdout = stdout.decode(errors='replace')
        stderr = stderr.decode(errors='replace')
        if retcode < 0:
            stderr += "<<Killed by Signal '%s'>>" % (signal.Signals(retcode * -1))

        if self.after_main:
            m = re.search("<<after_main>>:([0-9]*)", stderr)
            after_main_txt = self.after_main
            if m:
                line = int(m.group(1))
                lines = self.after_main.split("\n")
                if line < len(lines):
                    lines[line-2] += "  // " + red("<- ERROR")
                    after_main_txt = "\n".join(lines)

        logging.debug("Run: %s, [retcode=%d]",
                      cmd_prefix+args, retcode)
        if retcode != 0 and not must_fail:
            msg = "Program Run failed unexpected (exitcode={}): {} ".format(retcode, " ".join(cmd))
            logging.error("%s\nSTDOUT:\n%s\nSTDERR:\n%s", red(msg),
                          stdout, red(stderr))
            if input_:
                logging.error("STDIN:\n%s\n", input_.decode())
            if self.after_main:
                logging.info("\n%s", after_main_txt)
            if GDB:
                self.__run_with_gdb(args, cmd_prefix, input_data=input_, gdb=GDB, **kwargs)
            raise RuntimeError(msg)
        elif must_fail and not retcode_expected(retcode):
            msg = "Program Run did not fail, while it should: " + str((retcode, cmd))
            logging.error("%s\nSTDOUT:\n%s\nSTDERR:\n%s", red(msg),
                          stdout, red(stderr))
            if input_:
                logging.error("STDIN:\n%s\n", input_.decode())
            if self.after_main:
                logging.info("\n%s", after_main_txt)
            raise RuntimeError(msg)
        else:
            # If the testcase was successful, we look for our flag in
            # the stdout output. If this was not present, the user
            # probably cheated on us.
            if (self.after_main and "{{{FINISHED}}}" in self.after_main) or\
               (input and b"{{{FINISHED}}}" in input):
                if self.flag not in stdout:
                    msg="Testcase was not run until the end. Flag was not found."
                    logging.error("%s\nSTDOUT:\n%s\nSTDERR:\n%s", red(msg),
                          stdout, red(stderr))
                    if input_:
                        logging.error("STDIN:\n%s\n", input_.decode())
                    raise RuntimeError("Testcase was not run until the end")

        return stdout, stderr

    def __run(self, args = [], cmd_prefix = [], input=None, **kwargs):
        assert self.main, "Compilation failed"
        cmd = cmd_prefix + [self.main] + args
        stdin = subprocess.PIPE if input else subprocess.DEVNULL
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             stdin=stdin, **kwargs)
        stdout, stderr = p.communicate(input=input, timeout=10)
        return (p.returncode, cmd, stdout, stderr)

    def __run_with_gdb(self, args = [], cmd_prefix = [], input_data=None, gdb=None, **kwargs):
        assert self.main, "Compilation failed"
        assert gdb, "run with gdb without gdb is nonsense"
        while True:
            ans = input("Testcase failed. Restart testcase with {}? [Yes/no]".format(gdb))
            if ans.lower() in ['y', 'j', 'yes', 'ja', '']:
                break
            if ans.lower() in ['no', 'nein', 'n']:
                return
        cmd = cmd_prefix + [self.main] + args
        stdin = subprocess.PIPE if input_data else subprocess.DEVNULL
        env = os.environ.copy()
        env['WAIT_FOR_GDB'] = 'true'
        p = subprocess.Popen(cmd, env=env, stdin=stdin, **kwargs)
        gdb_p = subprocess.Popen([gdb, '-ex', 'break main', '-ex', 'signal SIGPROF', self.main, str(p.pid)])
        threading.Thread(target=p.communicate,
                         args=(input_data, ),
                         daemon=True).start()
        gdb_p.communicate()
        while True:
            ans = input("Continue with next testcase? [Yes/no]")
            if ans.lower() in ['y', 'j', 'yes', 'ja', '']:
                break
            if ans.lower() in ['no', 'nein', 'n']:
                exit()

    def spawn(self, args = [], cmd_prefix = [], input=None, **kwargs):
        """Spawn a child process and return handle immediately"""
        assert self.main, "Compilation failed"
        cmd = cmd_prefix + [self.main] + args
        stdin = subprocess.PIPE if input else subprocess.DEVNULL
        return subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                stdin=stdin, **kwargs)

    def trace(self, functions, **kwargs):
        log_file = tempfile.NamedTemporaryFile()
        logging.debug(log_file.name)
        os.environ['TRACE_FILE'] = log_file.name
        os.environ['TRACE_FUNCTIONS'] = ','.join(functions)
        if not os.path.exists(GDB_SCRIPT_TRACE):
            m = "GDB_SCRIPT_TRACE not found: " + GDB_SCRIPT_TRACE
            raise RuntimeError(m)
        cmd_prefix = ["gdb", '-nx', "-x", GDB_SCRIPT_TRACE, "--args"]
        stdout, stderr = self.run(cmd_prefix=cmd_prefix, **kwargs)
        ret = []
        for line in log_file.readlines():
            ret.append(eval(line))

        return Trace(ret)

    def signal_trace(self, args=[], functions=[], **kwargs):
        log_file = tempfile.NamedTemporaryFile()
        logging.debug(log_file.name)
        os.environ['TRACE_FILE'] = log_file.name
        os.environ['TRACE_FUNCTIONS'] = ','.join(functions)
        os.environ['TRACE_SIGNALS'] = 'True'
        if not os.path.exists(GDB_SCRIPT_TRACE):
            m = "GDB_SCRIPT_TRACE not found: " + GDB_SCRIPT_TRACE
            raise RuntimeError(m)
        cmd_prefix = ["gdb", '-nx', "-x", GDB_SCRIPT_TRACE, "--args"]
        return (log_file.name, self.spawn(args=args, cmd_prefix=cmd_prefix, **kwargs))

    def strace(self, syscalls=None, **kwargs):
        if Compilation.no_strace:
            raise MissingRequirementError("strace is not installed. Test skipped.")
        log_file = tempfile.NamedTemporaryFile()
        try:
            cmd_prefix = ["strace", '-qqf', '-o', log_file.name]
            if syscalls:
                cmd_prefix += ['-e','trace='+syscalls]
            (stdout, stderr) = self.run(cmd_prefix=cmd_prefix, **kwargs)
            lines = log_file.readlines()
            lines = [l.decode() for l in lines]
            return stdout, stderr, lines
        except FileNotFoundError as e:
            if e.filename == cmd_prefix[0]:
                Compilation.no_strace = True
                raise MissingRequirementError("strace is not installed. "\
                                   "To run this test install strace."\
                                   "\n  apt-get install strace\n")
            raise RuntimeError(dir(e))

    def cleanup(self):
        if self.tmpdir is None:
            return
        shutil.rmtree(self.tmpdir)
        self.tmpdir = None

class Trace:
    def __init__(self, records):
        invocations = {}
        for x in records:
            if x[0] == "call":
                _, parent, child, name, asm_name, args, thread_id = x
                invocations[child] = {
                    'id': child,
                    'name': name,
                    'asm_name': asm_name,
                    'args': args,
                    'parent': None,
                    'return': None,
                    'children': [],
                    'thread_id': thread_id
                }
                if parent is not None:
                    invocations[child]['parent'] = invocations[parent]
                    invocations[parent]['children'].append(invocations[x[1]])
            elif x[0] == "return":
                _, id, _, ret = x
                invocations[id]['return'] = ret

        self.records = invocations


    def function_called(self, name):
        for record in sorted(self.records.values(), key=lambda x: x['id']):
            if type(name) is str and record['name'] == name:
                yield record
            if type(name) in [list, tuple] and record['name'] in name:
                yield record


if __name__ == "__main__":
    import argparse
    import glob
    parser = argparse.ArgumentParser(description='GBS Testcase Tool.')
    parser.add_argument('-t', '--testcases', dest='testcases',
                        default = "tests",
                        help='Testcase(s) - directory or file')
    parser.add_argument('-v', '--verbose', dest='verbose',
                        action="store_true",
                        default = False,
                        help='verbose output')

    args = parser.parse_args()
    if args.verbose:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)

    if os.path.isdir(args.testcases):
        testcases = glob.glob(os.path.join(args.testcases, "*.test"))
    else:
        testcases = [args.testcases]

    results = []
    for test in sorted(testcases):
        # Execute testcase
        logging.info("Testcase: %s", test)
        t = Testcase(test)
        results.append(t)

    for c in Compilation.instances:
        c.cleanup()
        pass

    earned_points = sum([x.earned_points for x in results])
    if earned_points:
        print(f"You earned circa {earned_points} points for this task.")
    if any([x.failed for x in results ]):
        print()
        sys.exit("Failed Testcases: \n - " + "\n - ".join([x.testcase for x in results if x.failed]))
    if any([x.skipped_tests for x in results]):
        sys.exit("{} test(s) were skipped."\
                 .format(sum([x.skipped_tests for x in results])))
