# Makefile

CC := gcc
CPPFLAGS :=
CFLAGS := -Wall -Wextra -std=c99 -O3 `pkg-config --cflags sdl2 SDL2_image` -Wno-unknown-pragmas
LDFLAGS := -lm
LDLIBS := `pkg-config --libs sdl2 SDL2_image`

EXEC := main
EXEC_TEST := test
EXEC_SOLVER := solver

BUILD_DIR := build
DATA_DIR := out
TEST_DATA_DIR := ./tests/out
TEST_BOX_DIR := ./tests/out/box

SRC :=	${wildcard ./sudoc/src/*.c} ${wildcard ./tests/src/*.c} ./sudoc/main.c

SOLVER_SRC := ${wildcard ./sudoc/src/*.c} ./sudoc/solver.c

TEST_SRC :=	${wildcard ./sudoc/src/*.c} \
			${wildcard ./tests/src/*.c} \
			./tests/test.c

OBJ := ${SRC:.c=.o}
TEST_OBJ := ${TEST_SRC:.c=.o}
SOLVER_OBJ := ${SOLVER_SRC:.c=.o}

.PHONY: build all

all: build build-solver build-test clean-main clean-test

# BUILD
build: ${OBJ}
	@mkdir -p ${BUILD_DIR}
	@${CC} -o ${BUILD_DIR}/${EXEC} $^ ${LDFLAGS} ${LDLIBS}

build-test: ${TEST_OBJ}
	@mkdir -p ${BUILD_DIR}
	@${CC} -o ${BUILD_DIR}/${EXEC_TEST} $^ ${LDFLAGS} ${LDLIBS}

build-solver: ${SOLVER_OBJ}
	@mkdir -p ${BUILD_DIR}
	@${CC} -o ${BUILD_DIR}/${EXEC_SOLVER} $^ ${LDFLAGS} ${LDLIBS}

build-s1: build build-solver

# RUN
run: build clean-main
	@./${BUILD_DIR}/${EXEC}

test: build-test
	@mkdir -p ${TEST_DATA_DIR}
	@mkdir -p ${TEST_BOX_DIR}
	@./${BUILD_DIR}/${EXEC_TEST}

# test with valgrind
tv: build-test
	@mkdir -p ${TEST_DATA_DIR}
	@mkdir -p ${TEST_BOX_DIR}
	@valgrind --leak-check=full --show-leak-kinds=all ./${BUILD_DIR}/${EXEC_TEST}

# CLEAN
clean-main:
	${RM} ${OBJ}

clean-solver:
	${RM} ${SOLVER_OBJ}

clean-test:
	${RM} ${TEST_OBJ}

clean-test-data:
	${RM} -rf ${TEST_DATA_DIR}

clean-data:
	${RM} -rf ${DATA_DIR}

clean: clean-main clean-test clean-solver clean-data clean-test-data
	${RM} -r ${BUILD_DIR}

clear: clean
