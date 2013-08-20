// ********** Derek Leung
// ********** POTATO evaluation

#ifndef EVAL_H_INCLUDED
#define EVAL_H_INCLUDED

#include "data.h"

/**
 * Applies _operator_ to _operand_, where _operator_ is either a builtin
 * operator or a user-defined function and _operand_ is a list of arguments.
 */
struct obj* apply(struct obj* operator, struct obj* operand, struct env*);

/**
 * Evaluates the given object (following normal LISP rules of evaluation).
 */
struct obj* evaluate(struct obj* obj, struct env*);

/**
 * Prints an object to standard output.
 */
void print_obj(struct obj* obj);


#endif
