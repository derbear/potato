// ********** Derek Leung
// ********** General utilities

/*
 * Miscellaneous utility functions.
 */

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

// string functions

/**
 * Returns 1 if _str_ contains the character _c_, 0 otherwise.
 *
 * _str_ must be NULL-terminated.
 */
int str_contains(char* str, char c);

/**
 * Returns a true value if the contents of _str1_ match the contents
 * of _str2_.
 *
 * Both arguments must be NULL-terminated.
 */
int str_eq(char* str1, char* str2);

/**
 * Returns the length of _str_, excluding the NULL-terminating character.
 */
int str_len(char* str);

/**
 * Copies the NULL-terminated _src_ string into whatever is pointed to by _dst_.
 */
void str_cpy(char* src, char* dest);

// functils

#include "data.h"

/**
 * Flattens a cell-linked list into an array; returns null ptr if NIL.
 *
 * The address of the second arg, if set not NULL, will be the size fo the list.
 *
 * Exits on malformed lists.
 */
struct obj** flatten(struct obj* list, int* size); // @heap

/**
 * Inverse function of flatten() - turns an array into a cell-linked list.
 */
struct obj* unflatten(struct obj** array, int size);

/**
 * Returns the length of its given argument.
 *
 * If its given argument is not a well-formed list, then this returns -1.
 */
int list_len(struct obj* obj);

/**
 * Preprocesses the operands during a function call.
 *
 * If is_fixed_length is nonzero, this checks the arguments against num_args.
 * If is_strict_typed is nonzero, this checks the arguments against arg_types.
 * If is_applicative is nonzero, this evaluates each argument (before doing
 * type checks). If propagates_exceptions is also nonzero, any exception thrown
 * during evaluation will stop execution and begin unwinding the stack.
 *
 * Note that if is_applicative is not set but is_strict_typed is, type-checking
 * will be necessarily be done on _unevaluated_ arguments.
 *
 * FUNCTION accepts PRIMIIVE, CELL accepts NIL, and NIL accepts any type.
 *
 * If a check fails, operand is set to the error emitted, and NULL is returned.
 * Otherwise, the operands are flattened and evaluated if necessary.
 */
struct obj** prologue(struct obj** operand, struct env* env,
		      int is_fixed_length, int is_strict_typed,
		      int is_applicative, int propagates_exceptions,
		      int num_args, obj_type* arg_types); // TODO variable length args

#endif
