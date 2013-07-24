// ********** Derek Leung
// ********** POTATO evaluation

#ifndef EVAL_H_INCLUDED
#define EVAL_H_INCLUDED

#include "data.h"

/**
 * Lookup tables which map strings to objects and have a pointer to parent
 * lookup tables (which is NULL if it doesn't have one).
 */
struct table;

/**
 * Create a lookup table with a given parent and a given initial size.
 */
struct table* make_table(struct table* parent, int size);

/**
 * Creates a list from the rest of its arguments or returns NIL if no arguments
 * were provided.
 *
 * Becuase this function first evaluates each operand, this can be used as a
 * shortcut for "preprocessing" a list of arguments, which is what
 * most functions generally want to do.
 *
 * This function can be called directly as a primitive operator.
 */
struct obj* list(struct obj* operand);

/**
 * Applies _operator_ to _operand_, where _operator_ is either a builtin
 * operator or a user-defined function.
 */
struct obj* apply(struct obj* operator, struct obj* operand);

/**
 * Evaluates the given object (following normal LISP rules of evaluation).
 */
struct obj* evaluate(struct obj* obj);

/**
 * Returns the length of its given argument.
 *
 * If its given argument is not a well-formed list, then this returns -1.
 */
int list_len(struct obj* obj);

/**
 * Binds the given name to an object in the given lookup table.
 */
void bind(struct table*, char* name, struct obj* obj);

/**
 * Prints an object to standard output.
 */
void print_obj(struct obj* obj);


#endif
