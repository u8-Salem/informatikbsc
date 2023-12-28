DST=

copy_tests: lilo/tests hello/tests
	if [ "" = "${DST}" ]; then exit 1; fi
	cp -rf lilo/tests lilo/Makefile ${DST}/lilo/
	cp -rf hello/tests hello/Makefile ${DST}/hello
	cp -rf Makefile ${DST}/

test:
	@rm -f failed
	@make -C hello test || echo $$? hello_failed > failed
	@make -C lilo test || echo $$? lilo_failed >>failed
	@if [ -s failed ]; then exit 1; fi

.PHONY: clean test copy_tests
