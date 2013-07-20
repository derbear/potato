// ********** Derek Leung
// ********** General utilities

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

int str_contains(char* str, char c); // str null-term
int str_eq(char* str1, char* str2); // str null-term
int str_len(char* str); // does not include null char
void str_cpy(char* src, char* dest);

#endif
