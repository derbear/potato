#ifndef FORMATTING_H
#define FORMATTING_H

#include "data.h"

/**
 * Calls snprintf. First argument is format string, second is size,
 * rest of args (maximum 16) are values.
 *
 * Returns formatted string which was malloc()ed.
 *
 * Arguments: LITERAL format string, NUMBER size, ARGS arguments to snprintf
 * Return: LITERAL formatted string
 */
struct obj* c_snprintf_binding(struct obj* args, struct env* env);


#endif
