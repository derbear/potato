// ********** Derek Leung
// ********** General utilities

/*
 * Miscellaneous utility functions.
 */

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

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

#endif
