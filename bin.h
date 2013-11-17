// Dynamic loading of binary library native objects (functions only)

#include "data.h"

/**
 * Arguments: LITERAL library name, NUMBER lazy (1) or eager (0)
 * Return: LIBRARY | NIL on failure
 */
struct obj* bin_load(struct obj* operand, struct env* env);

/**
 * Arguments: LIBRARY, LITERAL symbol name
 * Return: PRIMITIVE | NIL on failure
 */
struct obj* bin_get(struct obj* operand, struct env* env);

/**
 * Arguments: LIBRARY
 * Return: SYMBOL (success) | NIL (failure)
 */
struct obj* bin_close(struct obj* operand, struct env* env);
