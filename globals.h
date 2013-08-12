// ********** Derek Leung
// ********** Global data objects

#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include "data.h"

/**
 * Set this to a non-zero value to print more information.
 */
#define DEBUG 1

/**
 * The global stream reader for standard input.
 */
extern struct reader* stdin_reader;

/**
 * The global lookup table for the global namespace.
 */
extern struct table* global_table;

/**
 * Initializes the global namespace and other global objects.
 */
void initialize(void);

/**
 * Register a primitive function to the global namespace.
 */
void register_primitive(char* name, struct obj* (*func)(struct obj*));

#endif
