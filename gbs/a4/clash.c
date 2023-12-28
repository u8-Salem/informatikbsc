// TODO: includes

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#include "plist.h"

#define BUFFSIZE 4096
typedef enum {false, true} bool; 

char* readCommand(); 
int parseCommands(char* line, char ***command_list_ret, size_t *size); 
int listWalker(pid_t pid, const char* cmdLine); 
int executeCommand(list *pid_list, char **parsed_command, size_t size, int *pid_ret); 
void printExitStatus(int exit_status, char *raw_input, size_t size); 
void collectZombies(list *pid_list); 
bool hasExceededLineLimit(int len); 

int main () {
	list pid_list = {NULL}; 

	while (true) {
		collectZombies(&pid_list);  

		char* line = readCommand(); 
		int line_len = strlen(line);
		if (hasExceededLineLimit(line_len)) {
			printf("Input line too long, ignoring.\n");
			free(line);
			continue; 
		}
		char* raw_input = malloc(sizeof(char) * line_len + 1); 
		strcpy(raw_input, line); 

		char **parsed_command = NULL; 
		size_t size = 0; 
		int parsing_result = parseCommands(line, &parsed_command, &size); 

		if (parsing_result) {
			printExitStatus(parsing_result, raw_input, line_len); 
			goto free; 
		}

		int pid_ret = 0; 
		int exit_status = executeCommand(&pid_list, parsed_command, size, &pid_ret); 

		if (pid_ret > 0) {
			int res = insertElement(&pid_list, pid_ret, raw_input);

			if (res < 0) {
				printf("Error with the pid-list! Trying to continue.\n"); 
			}

			goto free; 
		}
		printExitStatus(exit_status, raw_input, line_len); 

	free: 
		free(raw_input); 
		free(line); 
		free(parsed_command); 
	}
}

char* readCommand() {
	char *current_directory = getcwd(NULL, 0); 
	fprintf(stderr, "%s: ", current_directory);
	free(current_directory);  

	char *line = NULL; 
	size_t size = 0; 
	int result = getline(&line, &size, stdin); 

	if (result == -1) {
		free(line); 
		printf("\n"); 
		exit(0); 
	}

	int str_len = strlen(line); 
	line[str_len-1] = ' '; 

	return line; 
}

int parseCommands(char* line, char ***command_list_ret, size_t *size) {
	char *command = strtok(line, " "); 
	if (!command) return 0; 

	int index = 0; 
	char **command_list = NULL; 
	command_list = malloc((index + 1) * sizeof(char**)); 
	command_list[index++] = command; 

	while ((command = strtok(NULL, " "))) {
		command_list = realloc(command_list, (index + 1) * sizeof(char**)); 
		command_list[index++] = command; 
	}

	command_list = realloc(command_list, (index + 1) * sizeof(char**)); 
	command_list[index++] = NULL;

	*size = index; 
	*command_list_ret = command_list; 

	return 0; 

}

int listWalker(pid_t pid, const char* cmdLine) {
	printf("%d\t%s\n", pid, cmdLine); 
	return 0; 
}

int executeCommand(list *pid_list, char **parsed_command, size_t size, int *pid_ret) {
	if (size <= 0) { 
		return 0; 
	}

	if (strcmp(parsed_command[0], "jobs") == 0) {
		walkList(pid_list, listWalker);
		return 0;  
	}
	if (size > 1 && strcmp(parsed_command[0], "cd") == 0) {
		int result = chdir(parsed_command[1]); 
		return result; 
	}

	bool is_background_process = false; 
	if (strcmp(parsed_command[size - 2], "&") == 0) {
		is_background_process = true; 
		parsed_command[size - 2] = NULL; 
	}

	int pid = fork(); 
	if (pid > 0) {
		if (is_background_process) {
			*pid_ret = pid; 
			return 0; 
		} 

		int status = 0; 
		waitpid(pid, &status, 0);  
				
		return WEXITSTATUS(status); 


	} else if (pid == 0) {
		int result = execvp(parsed_command[0], parsed_command); 

		return result; 

	} else {
		return -1; 
	}

}

void printExitStatus(int exit_status, char *raw_input, size_t size) {
	raw_input[size - 1] = '\0'; 
	fprintf(stderr, "Exitstatus [%s] = %d\n", raw_input, exit_status); 

}

void collectZombies(list *pid_list) {
	pid_t pid = 0;

	while ((pid = waitpid(-1, NULL, WNOHANG))) {

		if (pid == -1) {
			return; 
		}

		char buff[BUFFSIZE]; 
		int result = removeElement(pid_list, pid, buff, BUFFSIZE); 

		if (result < 0) {
			printf("Error with the pid-list! Trying to continue.\n"); 
		} else {
			printExitStatus(0, buff, result); 
		}
	} 

}

bool hasExceededLineLimit(int len) {
	size_t max_line_len = sysconf(_SC_LINE_MAX); 
	return len > max_line_len; 
}
