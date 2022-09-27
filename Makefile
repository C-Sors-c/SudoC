# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra
LDFLAGS =
LDLIBS =
EXEC = sudoc
EXEC_TEST = test
BUILD_DIR = build

SRC =	${wildcard ./sudoc/libs/src/*.c} \
		${wildcard ./sudoc/core/src/*.c} \
		./sudoc/main.c

TEST_SRC =	${wildcard ./sudoc/libs/src/*.c} \
			${wildcard ./sudoc/core/src/*.c} \
			${wildcard ./tests/src/*.c}      \
			./tests/test.c

OBJ = ${SRC:.c=.o}
TEST_OBJ = ${TEST_SRC:.c=.o}

.PHONY: all

all: build build-test clean-src clean-test

# BUILD
build: $(OBJ)
	@mkdir -p $(BUILD_DIR)
	@$(CC) -o $(BUILD_DIR)/$(EXEC) $^ $(LDFLAGS) $(LDLIBS)

build-test: $(TEST_OBJ)
	@mkdir -p $(BUILD_DIR)
	@$(CC) -o $(BUILD_DIR)/$(EXEC_TEST) $^ $(LDFLAGS) $(LDLIBS)
	
# RUN
run: build clean-src
	@./$(BUILD_DIR)/$(EXEC)

test: build-test clean-test
	@./$(BUILD_DIR)/$(EXEC_TEST)

# CLEAN
clean-src:
	${RM} ${OBJ}

clean-test:
	${RM} ${TEST_OBJ}

clean: clean-src clean-test
	${RM} -r $(BUILD_DIR)


