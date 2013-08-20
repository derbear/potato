CFLAGS=-Wall -std=c99 -g -Werror
CORE=eval.o io.o util.o globals.o data.o main.o env.o
OBJECTS=${CORE} extensions.o
EXECUTABLE=potato

all: main

run: main
	./${EXECUTABLE}

test: main
	./${EXECUTABLE} test.pot

dist-source:
	tar -czf sources.tar.gz builtins.h data.h eval.h globals.h io.h util.h data.c eval.c globals.c io.c ioplus.c iobasic.c main.c util.c test.pot startup.pot Makefile README extensions.h ext_bind_stdio.h ext_bind_stdio.c

main: ${OBJECTS}
	cc ${CFLAGS} -o potato ${OBJECTS}

clean:
	rm -rf ${OBJECTS} *.gz ${EXECUTABLE}

main.o: globals.h eval.h builtins.h env.h main.c ext_bind_stdio.c
eval.o: data.h globals.h util.h data.h io.h env.h eval.h eval.c
io.o: util.h data.h io.h io.c
util.o: util.h util.c data.h eval.h
globals.o: globals.h globals.c env.h data.h builtins.h eval.h
data.o: data.h data.c
env.o: env.h env.c util.h globals.h

extensions.o: ext_bind_stdio.c ext_vector.c ${CORE}