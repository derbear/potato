// ********** Derek Leung
// ********** File I/O and parsing

#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

#include <stdio.h>

#include "data.h"

#define MAX_SYMBOL_SIZE 100

struct reader;

struct reader* make_reader(FILE*);
struct obj* next_object(struct reader*);

#endif
