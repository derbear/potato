// ********** Derek Leung
// ********** Built-in primitive operators

/*
 * Functions here are called directly by evaluate() as primitive operators, so
 * they should all have the signature (struct obj*) (*function) (struct obj*).
 */

#ifndef BUILTINS_H_INCLUDED
#define BUILTINS_H_INCLUDED

#include "data.h"
#include "env.h"

/**
 * Adds each operand to 0.
 */
struct obj* add(struct obj* operand, struct env* environment);

/**
 * With one argument, inverts it; with more, subtracts the rest from the first.
 */
struct obj* sub(struct obj* operand, struct env* environment);

/**
 * Multiplies each operand with 1.
 */
struct obj* mul(struct obj* operand, struct env* environment);

/**
 * Floor division.
 *
 * With one argument, finds the multiplicative inverse;
 * with more, divides the rest from the first.
 *
 * Since floating-point operators are not yet supported, if there is one
 * argument this will return 0 (unless that argument is 0 or 1).
 */
struct obj* floor_div(struct obj* operand, struct env* environment);

/**
 * Modulo operator.
 */
struct obj* mod(struct obj* operand, struct env* environment);

/**
 * Returns the first number if it is equal to the rest; otherwise returns NIL.
 */
struct obj* equals(struct obj* operand, struct env* environment);

/**
 * Returns the first number if it is equal to the second; otherwise NIL.
 */
struct obj* lessthan(struct obj* operand, struct env* environment);

/**
 * Returns the first object in a cell.
 */
struct obj* first(struct obj* operand, struct env* environment);

/**
 * Returns the second object in a cell.
 */
struct obj* rest(struct obj* operand, struct env* environment);

/**
 * Returns its first argument, unevaluated.
 */
struct obj* quote(struct obj* operand, struct env* environment);

/**
 * Constructs a cell with its first and second arguments.
 */
struct obj* construct(struct obj* operand, struct env* environment);

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
struct obj* list(struct obj* operand, struct env*);

/**
 * Binds its first argument, a symbol, to the value of its second argument.
 *
 * Raises an error if the argument has already been bound.
 */
struct obj* define(struct obj* operand, struct env* environment);

/**
 * Return the type of its first argument.
 */
struct obj* typeof(struct obj* operand, struct env* environment);

/**
 * Defines an anonymous function.
 *
 * The first argument should be a list of parameters or a symbol, while the
 * rest of the arguments should be expressions constituting the function body.
 *
 * During evaluation, each successive argument is bound to the corresponding
 * parameter in the argument list, or it is bound to a symbol directly.
 * Each expression in the function body is then evaluated sequentially, and
 * finally the value of the last expression is returned.
 */
struct obj* function(struct obj* operand, struct env* environment);

/**
 * Non-destructively reinterprets the first argument as the type indicated by
 * the second argument.
 *
 * The first argument can be any object, but the second argument should be a
 * symbol which corresponds to the target type.
 */
struct obj* cast(struct obj* operand, struct env* environment);

/**
 * Without any arguments, returns the current frame from which this has been
 * called.
 *
 * With a FRAME argument, returns the parent of that argument.
 */
struct obj* inspect_frame(struct obj* operand, struct env* environment);

/**
 * Evaluates the first argument. If it is NIL, returns the value of the third
 * argument; otherwise, the second.
 */
struct obj* ifelse(struct obj* operand, struct env* environment);

/**
 * Open up a file for reading, returning a STREAM object.
 */
struct obj* open(struct obj* operand, struct env* environment);

/**
 * Returns the evaluation of the first argument unless it returns an ERROR,
 * in which case this calls the function/primitive specified by the second
 * argument with the error string as the only argument (or NIL if no second
 * argument is supplied).
 */
struct obj* protect(struct obj* operand, struct env* environment);

/**
 * Evaluates the first argument under the current environment.
 *
 * If a FRAME object is provided, evaluates the first argument under that frame
 * instead.
 */
struct obj* builtin_eval(struct obj* operand, struct env* environment);

/**
 * Evaluates the first argument, which should be a operator, and applies the
 * second argument as a list of operands.
 */
struct obj* builtin_apply(struct obj* operand, struct env* environment);

/**
 * Reads an expression from the first operand, a stream.
 *
 * With no arguments, reads from standard input.
 */
struct obj* builtin_read(struct obj* operand, struct env* environment);

/**
 * Prints the first operand to standard output.
 */
struct obj* builtin_print(struct obj* operand, struct env* environment);

/**
 * Opens and executes the contents of a file.
 */
struct obj* load(struct obj* operand, struct env* environment);

/**
 * Destructively sets the first and second elements of a cell, respectively.
 */
struct obj* set_first(struct obj* operand, struct env* environment);
struct obj* set_rest(struct obj* operand, struct env* environment);

#endif
