# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra
LDFLAGS =
LDLIBS =
EXEC= test

SRC = ${wildcard ../libs/src/*.c} ${wildcard src/*.c} main.c
OBJ = ${SRC:.c=.o}


.c.o:
	@$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: test clean

build: $(OBJ)
	@$(CC) -o $(EXEC) $^ $(LDFLAGS) $(LDLIBS)

test: build
	@./$(EXEC)

clean:
	${RM} ${OBJ} ${EXEC}
