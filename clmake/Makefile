# -*- Remote HW Makefile -*-
SRC=../src/
LIBROOT=/home1/l/liyc/workspace/armlib

# Choose between Linux and Baremetal targets
TRIPLETS=aarch64-none-elf
#TRIPLETS=aarch64-linu-gnu
TARGET_CPU=cortex-a53
TARGET_OS=standalone

GSL_LIB_DIR=gsl-${TRIPLETS}

PLATFORM=zcu102_es2
SYSCONFIG=a53_standalone 
SDSPROC=a53_standalone 
#SYSCONFIG=a53_linux
#SDSPROC=a53_linux

SOURCES = ${SRC}/app			\
	${SRC}/util					\
	${SRC}/init					\
	${SRC}/perf					\
	${SRC}/hw_sim				\
	${SRC}/hw					\
	${SRC}/hw_util				\
	${SRC}/hw_init				\
	${SRC}/simulation			\
	\

EXEC = ksaccel.elf

OBJS = ${SOURCES:=.o}

CROSS_COMPILE = ${TRIPLETS}-
AR=${CROSS_COMPILE}ar
LD=${CROSS_COMPILE}ld
CC=${CROSS_COMPILE}g++
SDSFLAGS = -sds-pf ${PLATFORM} \
	-sds-hw hw_top ${SRC}/hw.cc -files ${SRC}/hw_util.cc -clkid 2 -sds-end \
	-sds-sys-config ${SYSCONFIG} \
	-target-cpu ${TARGET_CPU} \
	-target-os ${TARGET_OS} \
	\

SCC = sds++ ${SDSFLAGS}

LDDIRS = -L${LIBROOT}/${GSL_LIB_DIR}/lib 
LDLIBS = -lm -lgsl -lgslcblas
LDFLAGS = -g ${LDDIRS} ${LDLIBS}
INCDIRS = -I${LIBROOT}/${GSL_LIB_DIR}/include
CFLAGS = -W -c ${INCDIRS}


#DEBUG
#$(error VAR is $(CCC))

.PHONY: all

all: ${EXEC}

${EXEC}: ${OBJS}
	@echo 'Building target: $@'
	@echo 'Invoking: SDS++ Linker'
	${SCC} ${OBJS} -o $@ ${LDFLAGS}
	@echo 'Finished building target: $@'
	@echo ' '

${SRC}/%.o: ${SRC}/%.cc
	${SCC} ${CFLAGS} $<

clean:
	${RM} ${EXEC} ${OBJS}

ultraclean: clean
	${RM} ${EXEC}.bit 
	${RM} -rf _sds sd_card

# CURPWD = /home/liyc/git/ks/baseline/codes
# INC_DIRS = -I"../../lib/gsl-aarch64-linux-gnu/include"
# LD_DIRS = -L${CURPWD}/lib/gsl-aarch64-linux-gnu/lib
# LD_LIBS = -lgsl -lgslcblas -lm

# CFLAGS = -Wall -c ${INC_DIRS}
# LDFLAGS = ${LD_DIRS} ${LD_LIBS}

# COMMON_H = \
# 	app.h					\
# 	definitions.h			\
# 	util.h				\
# 	\

## HW
# ${HW_EXEC}: ${HW_OBJS}
# 	${CC} ${LDFLAGS} ${HW_OBJS} -o $@

# # -I here if needed
# %.o: %.cc $(COMMON_H)
# 	$(CC) $(CFLAGS) $< o $@

########################
# OLD
#######################
# APPSOURCES = mmult.cpp mmult_accel.cpp 
# EXECUTABLE = mmult.elf

# PLATFORM = zc702
# SDSFLAGS = -sds-pf ${PLATFORM} \
# 	-sds-hw mmult_accel mmult_accel.cpp -sds-end \
# 	-poll-mode 1

# CC = sds++ ${SDSFLAGS}

# CFLAGS = -Wall -O3 -c
# CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
# LFLAGS = -O3

# OBJECTS := $(APPSOURCES:.cpp=.o)
# DEPS := $(OBJECTS:.o=.d)

# .PHONY: all

# all: ${EXECUTABLE}

# ${EXECUTABLE}: ${OBJECTS}
# 	${CC} ${LFLAGS} ${OBJECTS} -o $@ 

# -include ${DEPS}

# %.o: %.cpp
# 	${CC} ${CFLAGS} $< -o $@

# clean:
# 	${RM} ${EXECUTABLE} ${OBJECTS} ${DEPS}

# ultraclean: clean
# 	${RM} ${EXECUTABLE}.bit 
# 	${RM} -rf _sds sd_card