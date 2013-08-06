CFLAGS=-Wall -std=c99 -g -Werror
OBJECTS=eval.o io.o util.o globals.o data.o builtins.o main.o
all: main

run: main
	./main

test: main
	./main test.pot

dist-source:
	tar -czf sources.tar.gz builtins.h data.h eval.h globals.h io.h util.h builtins.c data.c eval.c globals.c io.c ioplus.c iobasic.c main.c util.c test.pot Makefile README

main: ${OBJECTS}
	cc ${CFLAGS} -o main ${OBJECTS}

clean:
	rm -rf ${OBJECTS} *.gz main

main.o: globals.h eval.h builtins.h main.c
eval.o: data.h globals.h util.h data.h io.h eval.h eval.c
builtins.o: builtins.h data.h globals.h util.h io.h eval.h builtins.c
io.o: util.h data.h io.h io.c ioplus.c iobasic.c
util.o: util.h util.c
globals.o: globals.h globals.c
data.o: data.h data.c
