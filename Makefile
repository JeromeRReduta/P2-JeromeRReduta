# Output binary name
bin=this_leaks_how_much_oh_sh

# Set the following to '0' to disable log messages:
LOGGER ?= 0

# Compiler/linker flags
CFLAGS += -g -Wall -fPIC -DLOGGER=$(LOGGER)
LDLIBS += -lm -lreadline
LDFLAGS +=

src=history.c shell.c ui.c util.c signal.c pipe.c job_list.c process_io.c
obj=$(src:.c=.o)

all: $(bin) libshell.so

$(bin): $(obj)
	$(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(obj) -o $@

libshell.so: $(obj)
	$(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(obj) -shared -o $@

shell.o: shell.c history.h logger.h ui.h signal.c signal.h pipe.c pipe.h job_list.c job_list.h 
history.o: history.c history.h logger.h 
ui.o: ui.h ui.c logger.h history.h 
util.o: util.c util.h logger.h
pipe.o: pipe.c pipe.h logger.h
job_list.o: job_list.c job_list.h logger.h
process_io.o: process_io.c process_io.h logger.h



clean:
	rm -f $(bin) $(obj) libshell.so vgcore.*


# Tests --

test: $(bin) libshell.so ./tests/run_tests
	@DEBUG="$(debug)" ./tests/run_tests $(run)

testupdate: testclean test

./tests/run_tests:
	rm -rf tests
	git clone https://github.com/USF-OS/Shell-Tests.git tests

testclean:
	rm -rf tests
