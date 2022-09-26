# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra
LDFLAGS =
LDLIBS =

SRC = src/add.c src/main.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

utils: ${OBJ}

-include ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} test

# END