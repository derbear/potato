CFLAGS=-Wall -std=c99 -g -Werror
CORE=eval.o io.o util.o globals.o data.o main.o env.o bin.o
OBJECTS=${CORE} extensions.o
EXECUTABLE=potato

all: main

run: main
	./${EXECUTABLE}

test: main
	./${EXECUTABLE} test.pot

main: ${OBJECTS}
	cc ${CFLAGS} -o potato ${OBJECTS} -ldl # -ldl for dynamic loading

clean:
	rm -rf ${OBJECTS} *.gz ${EXECUTABLE}

main.o: globals.h eval.h builtins.h env.h main.c ext_bind_stdio.c
eval.o: data.h globals.h util.h data.h io.h env.h eval.h eval.c
io.o: util.h data.h io.h io.c
util.o: util.h util.c data.h eval.h
globals.o: globals.h globals.c env.h data.h builtins.h eval.h bin.h
data.o: data.h data.c globals.h eval.h util.h
env.o: env.h env.c util.h globals.h
bin.o: bin.c bin.h data.h util.h globals.h
	cc ${CFLAGS} -c -o bin.o bin.c

extensions.o: ext_bind_stdio.c ext_vector.c ${CORE}