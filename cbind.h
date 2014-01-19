// Bindings to stdio.h functions
// TODO this is incomplete

#ifndef CBIND_H
#define CBIND_H

#include "data.h"

/* STDLIB_FILE in this file refers to what's returned by fopen */

/**
 * Arguments: none
 * Return: a wrapper around stdin
 */
struct obj* c_stdin(struct obj* operand, struct env* env);

/**
 * Arguments: none
 * Return: a wrapper around stdout
 */
struct obj* c_stdout(struct obj* operand, struct env* env);

/**
 * Arguments: none
 * Return: a wrapper around stderr
 */
struct obj* c_stderr(struct obj* operand, struct env* env);

/**
 * Arguments: STRING filename, STRING mode (e.g. "r", "w")
 * Return: STDLIB_FILE file stream | NIL (failure)
 */
struct obj* c_fopen(struct obj* operand, struct env* env);

/**
 * Arguments: STDLIB_FILE file stream
 * Return: SYMBOL (success) | NIL (failure)
 */
struct obj* c_fclose(struct obj* operand, struct env* env);

/**
 * Arguments: STDLIB_FILE file stream
 * Return: NUMBER char (success) | NIL (failure)
 */
struct obj* c_fgetc(struct obj* operand, struct env* env);

/**
 * Arguments: STDLIB_FILE file stream, NUMBER char
 * Return: NUMBER char (success) | NIL (failure)
 */
struct obj* c_fputc(struct obj* operand, struct env* env);

#endif
