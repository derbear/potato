// Bindings to stdio.h functions
// TODO this is incomplete

#ifndef EXT_BIND_STDIO_H
#define EXT_BIND_STDIO_H

#include "data.h"

/* STDLIB_FILE in this file refers to what's returned by fopen */

/**
 * Arguments: STRING filename, STRING mode
 * Return: STDLIB_FILE file stream | NIL (failure)
 */
struct obj* ext_bind_stdio_fopen(struct obj* operand);

/**
 * Arguments: STDLIB_FILE file stream
 * Return: SYMBOL (success) | NIL (failure)
 */
struct obj* ext_bind_stdio_fclose(struct obj* operand);

/**
 * Arguments: STDLIB_FILE file stream
 * Return: NUMBER char (success) | NIL (failure)
 */
struct obj* ext_bind_stdio_fgetc(struct obj* operand);

/**
 * Arguments: STDLIB_FILE file stream, NUMBER char
 * Return: NUMBER char (success) | NIL (failure)
 */
struct obj* ext_bind_stdio_fputc(struct obj* operand);

#endif
