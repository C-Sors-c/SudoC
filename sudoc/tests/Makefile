# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra
LDFLAGS =
LDLIBS =
EXEC= test

SRC = ${wildcard ../libs/src/*.c} ${wildcard *.c}
OBJ = ${SRC:.c=.o}


.c.o:
	@$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: test clean

test: ${OBJ}
	${CC} -o $@ $^ ${CFLAGS}
	./test

clean:
	${RM} ${OBJ} ${EXEC}
