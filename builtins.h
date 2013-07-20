// ********** Derek Leung
// ********** Built-in primitive operators

#ifndef BUILTINS_H_INCLUDED
#define BUILTINS_H_INCLUDED

#include "data.h"

/**
 * Functions here are called directly by evaluate() as primitive operators, so
 * they should all have the signature (struct obj*) (*function) (struct obj*).
 */

struct obj* add(struct obj* operand);
struct obj* sub(struct obj* operand);
struct obj* mul(struct obj* operand);
struct obj* floor_div(struct obj* operand);
struct obj* equals(struct obj* operand);
struct obj* lessthan(struct obj* operand);
struct obj* first(struct obj* operand);
struct obj* rest(struct obj* operand);
struct obj* quote(struct obj* operand);
struct obj* construct(struct obj* operand);
struct obj* define(struct obj* operand);
struct obj* function(struct obj* operand);
struct obj* ifelse(struct obj* operand);
struct obj* file_open(struct obj* operand);
struct obj* builtin_eval(struct obj* operand);
struct obj* builtin_apply(struct obj* operand);
struct obj* builtin_read(struct obj* operand);
struct obj* builtin_print(struct obj* operand);
struct obj* execute(struct obj* operand);

#endif
