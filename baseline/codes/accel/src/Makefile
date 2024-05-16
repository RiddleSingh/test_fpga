# -*- Makefile -*-
CC = g++
CFLAGS = -Wall -O3
LDFLAGS = -lm -lgsl -lgslcblas

COMMON_H = \
	app.h					\
	definitions.h			\
	util.h					\
	dev_options.h			\
	\

KS_TARGET = app
AUX_TARGET = aux
TESTS = tests

TARGETS = $(KS_TARGET) #$(TESTS)

KS_MAIN = app
KS_DEPS =					\
	util					\
	init					\
	perf					\
	hw_li					\
	hw						\
	hw_util					\
	hw_init					\
	simulation				\
	\

AUX_MAIN = aux_main
TESTS_MAIN = tests
TESTS_DEPS = 				\
	test_hw_util			\
	\

# HW section
HW_SOURCES = ${KS_DEPS:=.cc} ${KS_MAIN:=.cc}
HW_EXEC = ksaccel.elf
CROSS_COMPILE = arm-xilinx-linux-gnueabi-
AR = ${CROSS_COMPILE}ar
LD = ${CROSS_COMPILE}ld
PLATFORM = ese532Linux
SDS_FLAGS = 
HW_CC = sds++ ${SDSFLAGS}


all: $(TARGETS)

# -I here if needed
%.o: %.cc $(COMMON_H)
	$(CC) $< $(CFLAGS) -c

# -L here if needed
$(KS_TARGET): ${KS_DEPS:=.o} ${KS_MAIN:=.o}
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $(KS_TARGET)

$(AUX_TARGET): ${KS_DEPS:=.o} ${AUX_MAIN:=.o}
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $(AUX_TARGET)

$(TESTS): ${KS_DEPS:=.o} ${TESTS_DEPS:=.o} ${TESTS_MAIN:=.o}
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $(TESTS)

# Make run
.PHONY: r
r::
	$(KS_TARGET) 

# Make run valgrind
.PHONY: v
v::
	make && valgrind --leak-check=full --show-leak-kinds=all $(KS_TARGET)

#Make run lldb
.PHONY: l
l::
	make && lldb $(KS_TARGET)

# Make run gdb
.PHONY: g
g:
	make && gdb $(KS_TARGET)

# clean binaries and objs
.PHONY: clean
clean::
	rm -r *.o $(KS_TARGET)

# ## HW
# ${HW_EXEC}: ${HW_OBJS}
# 	${HW_CC} ${HW_OBJS} -o $@ ${HW_LDFLAGS}