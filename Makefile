# Makefile

CC := gcc
CPPFLAGS :=
CFLAGS := -Wall -Wextra -Werror -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-parameter \
		  -std=c99 -O3 -fsanitize=address `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS := -lm
LDLIBS := -fsanitize=address `pkg-config --libs sdl2 SDL2_image`

EXEC := main
EXEC_TEST := test
EXEC_SOLVER := solver

BUILD_DIR := build
DATA_DIR := out
BOX_DIR := out/box
TEST_DATA_DIR := ./tests/out
TEST_BOX_DIR := ./tests/out/box
TEST_BOX_DIR2 := ./tests/out/box2

SRC :=	${wildcard ./sudoc/src/*.c} ${wildcard ./tests/src/*.c} ./sudoc/main.c

SOLVER_SRC := ${wildcard ./sudoc/src/*.c} ./sudoc/solver.c

TEST_SRC :=	${wildcard ./sudoc/src/*.c} \
			${wildcard ./tests/src/*.c} \
			./tests/test.c

OBJ := ${SRC:.c=.o}
TEST_OBJ := ${TEST_SRC:.c=.o}
SOLVER_OBJ := ${SOLVER_SRC:.c=.o}

.PHONY: build all

all: build build-solver build-test clean-main clean-test clean-solver

# BUILD
build: ${OBJ}
	@mkdir -p ${BUILD_DIR}
	@mkdir -p ${DATA_DIR}
	@mkdir -p ${BOX_DIR}
	@mkdir -p ${TEST_DATA_DIR}
	@mkdir -p ${TEST_BOX_DIR}
	@mkdir -p ${TEST_BOX_DIR2}
	@${CC} -o ${BUILD_DIR}/${EXEC} $^ ${LDFLAGS} ${LDLIBS}

build-test: ${TEST_OBJ}
	@mkdir -p ${BUILD_DIR}
	@mkdir -p ${TEST_DATA_DIR}
	@mkdir -p ${TEST_BOX_DIR}
	@mkdir -p ${TEST_BOX_DIR2}
	@${CC} -o ${BUILD_DIR}/${EXEC_TEST} $^ ${LDFLAGS} ${LDLIBS}

build-solver: ${SOLVER_OBJ}
	@mkdir -p ${BUILD_DIR}
	@${CC} -o ${BUILD_DIR}/${EXEC_SOLVER} $^ ${LDFLAGS} ${LDLIBS}

# RUN
run: build clean-main
	@./${BUILD_DIR}/${EXEC}

test: build-test
	@./${BUILD_DIR}/${EXEC_TEST}

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
	${RM} -rf ${BOX_DIR}
	${RM} -rf ${DATA_DIR}

clean: clean-main clean-test clean-solver clean-data clean-test-data
	${RM} -r ${BUILD_DIR}

clear: clean
