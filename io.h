// ********** Derek Leung
// ********** File I/O and parsing

/*
 * This defines a basic interface for file reading and writing.
 * Any implementation can be used in place of another provided that these
 * specified functions are supported.
 */

#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

#include <stdio.h>

#include "data.h"

/**
 * Until arbitrary-length symbols are supported, this macro indicates the
 * maximum character-length of a symbol.
 */
#define MAX_SYMBOL_SIZE 100

/**
 * A stream/stream-reader which wraps around the abstract FILE objects provided
 * by the C standard library.
 */
struct reader;

/**
 * Create a stream (reader) given a file pointer.
 */
struct reader* make_reader(FILE*);

/**
 * Parse the next object from the given stream (reader).
 */
struct obj* next_object(struct reader*);

/**
 * Close the reader's associated file object, returning 0 only on failure.
 */
int reader_fclose(struct reader*);

#endif
