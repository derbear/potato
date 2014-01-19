// Dynamic loading of binary library objects

#include "data.h"

/**
 * Arguments: STRING library name, NUMBER lazy (1) or eager (0)
 * Return: LIBRARY | NIL on failure
 */
struct obj* bin_load(struct obj* operand, struct env* env);

/**
 * Arguments: LIBRARY, STRING symbol name
 * Return: UNSPECIFIED-type object | NIL on failure
 */
struct obj* bin_get(struct obj* operand, struct env* env);

/**
 * Arguments: LIBRARY
 * Return: SYMBOL (success) | NIL (failure)
 */
struct obj* bin_close(struct obj* operand, struct env* env);
