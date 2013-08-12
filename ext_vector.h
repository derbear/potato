// Resizeable vector operations

#ifndef EXT_VECTOR_H
#define EXT_VECTOR_H

void ext_vector();

/**
 * Arguments: none | NUMBER initial size
 * Return: VECTOR
 */
struct obj* ext_vector_new(struct obj* operand, struct env* env);

/**
 * Arguments: STRING
 * Return: VECTOR
 */
struct obj* ext_vector_from_string(struct obj* operand, struct env* env);

/**
 * Arguments: VECTOR
 * Return: NUMBER
 */
struct obj* ext_vector_length(struct obj* operand, struct env* env);

/**
 * Arguments: VECTOR v, NUMBER index
 * Return: OBJECT at index of v
 */
struct obj* ext_vector_index(struct obj* operand, struct env* env);

/**
 * Arguments: VECTOR v, NUMBER index, OBJECT o
 * Return: OBJECT formerly at index of v
 */
struct obj* ext_vector_set(struct obj* operand, struct env* env);

/**
 * Arguments: VECTOR v, NUMBER index, OBJECT o
 * Return: OBJECT o | NIL (out of bounds)
 */
struct obj* ext_vector_insert(struct obj* operand, struct env* env);

/**
 * Arguments: VECTOR v, NUMBER index
 * Return: OBJECT formerly at index of v
 */
struct obj* ext_vector_remove(struct obj* operand, struct env* env);

#endif
