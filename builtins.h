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
 * Binds its first argument, a symbol, to the value of its second argument.
 */
struct obj* define(struct obj* operand, struct env* environment);

/**
 * Return the type of its first argument.
 */
struct obj* typeof(struct obj* operand, struct env* environment);

/**
 * Defines an anonymous function.
 *
 * The first argument should be a list of symbols, while the rest of the
 * arguments should be expressions constituting the function body.
 *
 * During evaluation, each successive argument is bound to the corresponding
 * symbol in the argument list. Each expression in the function body is then
 * evaluated sequentially, and finally the value of the last expression is
 * returned.
 *
 * Variable-length arguments are not yet supported.
 */
struct obj* function(struct obj* operand, struct env* environment);

/**
 * Copy the function object but mark it as a macro.
 */
struct obj* mark_macro(struct obj* operand, struct env* environment);

/**
 * Evaluates the first argument. If it is NIL, returns the value of the third
 * argument; otherwise, the second.
 */
struct obj* ifelse(struct obj* operand, struct env* environment);

/**
 * Evaluates the first argument.
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

#endif
