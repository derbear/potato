// ********** Derek Leung
// ********** General utilities

#include "util.h"

int str_contains(char* str, char c) { // str null-term
  char t;
  while ((t = *str) != 0) {
    if (c == t) return 1;
    str++;
  }
  return 0;
}

int str_eq(char* str1, char* str2) { // str null-term
  while (*str1 != 0 && *str2 != 0) {
    if (*str1 != *str2) {
      return 0;
    }
    str1++;
    str2++;
  }
  return *str1 == *str2;
}

int str_len(char* str) { // does not include null char
  char* c;
  for (c = str; *c != 0; c++);
  return c - str;
}

void str_cpy(char* src, char* dest) {
  for (; *src != 0; src++) {
    *dest = *src;
    dest++;
  }
  *dest = 0;
}
