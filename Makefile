## ======================================================================
## partial Makefile provided to students
##

GTK_INCLUDE := `pkg-config --cflags gtk+-3.0`
GTK_LIBS := `pkg-config --libs gtk+-3.0`

.PHONY: clean new style feedback submit1 submit2 submit

CFLAGS += -std=c11 -Wall -pedantic -g

# a bit more checks if you'd like to (uncomment
# CFLAGS += -Wextra -Wfloat-equal -Wshadow                         \
# -Wpointer-arith -Wbad-function-cast -Wcast-align -Wwrite-strings \
# -Wconversion -Wunreachable-code

# uncomment if you want to add DEBUG flag
# CPPFLAGS += -DDEBUG

# ----------------------------------------------------------------------
# feel free to update/modifiy this part as you wish

# all those libs are required on Debian, feel free to adapt it to your box
LDLIBS += -lcheck -lm -lrt -pthread -lsubunit

all:: test-cpu-week08

test-cpu-week08 	: test-cpu-week08.o bit.o cpu.o alu.o bus.o memory.o component.o cpu-storage.o opcode.o cpu-registers.o cpu-alu.o error.o
unit-test-bit 		: unit-test-bit.o bit.o
unit-test-alu 		: unit-test-alu.o alu.o bit.o
unit-test-bus 		:	unit-test-bus.o bus.o component.o bit.o memory.o
unit-test-memory 	: unit-test-memory.o memory.o bus.o component.o bit.o
unit-test-component : unit-test-component.o component.o bus.o memory.o bit.o
unit-test-cpu		: unit-test-cpu.o component.o bus.o memory.o bit.o cpu.o cpu-registers.o cpu-storage.o
unit-test-cpu-dispatch-week08 : unit-test-cpu-dispatch-week08.o alu.o \
 bit.o bus.o memory.o component.o opcode.o cpu-alu.o cpu-storage.o \
 cpu-registers.o gameboy.o timer.o cartridge.o


alu.o: alu.c bit.h alu.h error.h
bit.o: bit.c bit.h error.h
bootrom.o: bootrom.c bootrom.h bus.h memory.h component.h gameboy.h cpu.h \
 alu.h bit.h timer.h cartridge.h error.h
bus.o: bus.c bus.h memory.h component.h error.h bit.h
cartridge.o: cartridge.c component.h memory.h bus.h error.h cartridge.h
component.o: component.c component.h memory.h error.h
cpu-alu.o: cpu-alu.c error.h bit.h alu.h cpu-alu.h opcode.h cpu.h bus.h \
 memory.h component.h cpu-storage.h cpu-registers.h
cpu.o: cpu.c error.h opcode.h bit.h cpu.h alu.h bus.h memory.h \
 component.h cpu-alu.h cpu-registers.h cpu-storage.h util.h
cpu-registers.o: cpu-registers.c cpu-registers.h cpu.h alu.h bit.h bus.h \
 memory.h component.h error.h
cpu-storage.o: cpu-storage.c error.h cpu-storage.h memory.h opcode.h \
 bit.h cpu.h alu.h bus.h component.h cpu-registers.h gameboy.h timer.h \
 cartridge.h util.h
error.o: error.c
gameboy.o: gameboy.c gameboy.h bus.h memory.h component.h cpu.h alu.h \
 bit.h timer.h cartridge.h error.h bootrom.h
libsid_demo.o: libsid_demo.c sidlib.h
memory.o: memory.c memory.h error.h
opcode.o: opcode.c opcode.h bit.h
sidlib.o: sidlib.c sidlib.h
test-cpu-week08.o: test-cpu-week08.c opcode.h bit.h cpu.h alu.h bus.h \
 memory.h component.h cpu-storage.h util.h error.h
test-gameboy.o: test-gameboy.c gameboy.h bus.h memory.h component.h cpu.h \
 alu.h bit.h timer.h cartridge.h util.h error.h
timer.o: timer.c timer.h component.h memory.h bit.h cpu.h alu.h bus.h \
 error.h
util.o: util.c


unit-test-alu.o: unit-test-alu.c tests.h error.h alu.h bit.h
unit-test-alu_ext.o: unit-test-alu_ext.c tests.h error.h alu.h bit.h \
 alu_ext.h
unit-test-bit.o: unit-test-bit.c tests.h error.h bit.h
unit-test-bus.o: unit-test-bus.c tests.h error.h bus.h memory.h \
 component.h util.h
unit-test-cartridge.o: unit-test-cartridge.c tests.h error.h cartridge.h \
 component.h memory.h bus.h cpu.h alu.h bit.h
unit-test-component.o: unit-test-component.c tests.h error.h bus.h \
 memory.h component.h
unit-test-cpu.o: unit-test-cpu.c tests.h error.h alu.h bit.h opcode.h \
 util.h cpu.h bus.h memory.h component.h cpu-registers.h cpu-storage.h \
 cpu-alu.h
unit-test-cpu-dispatch.o: unit-test-cpu-dispatch.c tests.h error.h alu.h \
 bit.h cpu.h bus.h memory.h component.h opcode.h util.h \
 unit-test-cpu-dispatch.h cpu.c cpu-alu.h cpu-registers.h cpu-storage.h
unit-test-cpu-dispatch-week08.o: unit-test-cpu-dispatch-week08.c tests.h \
 error.h alu.h bit.h cpu.h bus.h memory.h component.h opcode.h gameboy.h \
 timer.h cartridge.h util.h unit-test-cpu-dispatch.h cpu.c cpu-alu.h \
 cpu-registers.h cpu-storage.h
unit-test-cpu-dispatch-week09.o: unit-test-cpu-dispatch-week09.c tests.h \
 error.h alu.h bit.h cpu.h bus.h memory.h component.h opcode.h util.h \
 unit-test-cpu-dispatch.h cpu.c cpu-alu.h cpu-registers.h cpu-storage.h
unit-test-memory.o: unit-test-memory.c tests.h error.h bus.h memory.h \
 component.h
unit-test-timer.o: unit-test-timer.c util.h tests.h error.h timer.h \
 component.h memory.h bit.h cpu.h alu.h bus.h

TARGETS := test-cpu-week08
CHECK_TARGETS := 
OBJS = 
OBJS_NO_STATIC_TESTS =
OBJS_STATIC_TESTS = 
OBJS = $(OBJS_STATIC_TESTS) $(OBJS_NO_STATIC_TESTS)

# ----------------------------------------------------------------------
# This part is to make your life easier. See handouts how to make use of it.


clean::
	-@/bin/rm -f *.o *~ $(CHECK_TARGETS)

new: clean all

static-check:
	scan-build -analyze-headers --status-bugs -maxloop 64 make CC=clang new

style:
	astyle -n -o -A8 -xt0 *.[ch]

# all those libs are required on Debian, adapt to your box
$(CHECK_TARGETS): LDLIBS += -lcheck -lm -lrt -pthread -lsubunit

check:: $(CHECK_TARGETS)
	$(foreach target,$(CHECK_TARGETS),./$(target) &&) true

# target to run tests
check:: all
	@if ls tests/*.*.sh 1> /dev/null 2>&1; then \
	  for file in tests/*.*.sh; do [ -x $$file ] || echo "Launching $$file"; ./$$file || exit 1; done; \
	fi

IMAGE=chappeli/feedback:latest
feedback:
	@docker pull $(IMAGE)
	@docker run -it --rm -v ${PWD}:/home/tester/done $(IMAGE)

SUBMIT_SCRIPT=../provided/submit.sh
submit1: $(SUBMIT_SCRIPT)
	@$(SUBMIT_SCRIPT) 1

submit2: $(SUBMIT_SCRIPT)
	@$(SUBMIT_SCRIPT) 2

submit:
	@printf 'what "make submit"??\nIt'\''s either "make submit1" or "make submit2"...\n'

libsid.so: sidlib.c
	$(CC) -fPIC -shared $(CPPFLAGS) $(CFLAGS) $(GTK_INCLUDE) $^ -o $@

libsid_demo.o: CFLAGS += $(GTK_INCLUDE)

libsid_demo: LDFLAGS += -L.
libsid_demo: LDLIBS += $(GTK_LIBS) -lsid
libsid_demo: libsid_demo.o libsid.so
