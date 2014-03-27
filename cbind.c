// Bindings to C standard library objects
// TODO this is incomplete

#include "cbind.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "data.h"
#include "util.h"

struct obj* c_stdin(struct obj* operand, struct env* env) {
  static int init = 0;
  static struct obj wrap_c_stdin;
  if (!init) {
    struct obj dummy = { UNSPECIFIED, {stdin} };
    wrap_c_stdin = dummy;
    init = 1;
  }
  return &wrap_c_stdin;
}

struct obj* c_stdout(struct obj* operand, struct env* env) {
  static int init = 0;
  static struct obj wrap_c_stdout;
  if (!init) {
    struct obj dummy = { UNSPECIFIED, {stdout} };
    wrap_c_stdout = dummy;
    init = 1;
  }
  return &wrap_c_stdout;
}

struct obj* c_stderr(struct obj* operand, struct env* env) {
  static int init = 0;
  static struct obj wrap_c_stderr;
  if (!init) {
    struct obj dummy = { UNSPECIFIED, {stderr} };
    wrap_c_stderr = dummy;
    init = 1;
  }
  return &wrap_c_stderr;
}

struct obj* c_fopen(struct obj* operand, struct env* env) {
  FILE* fptr = fopen(LIST_FIRST(operand)->string, LIST_SECOND(operand)->string);
  if (!fptr) {
    return make_object(NIL, 0);
  }
  return make_object(UNSPECIFIED, fptr);
}

struct obj* c_fclose(struct obj* operand, struct env* env) {
  // TODO we need some kind of extension object "signing" so we don't do
  // operations on invalid file objects

  int result = fclose((FILE*) LIST_FIRST(operand)->data);
  if (result == 0) {
    char* success_symbol = malloc(sizeof(char) * 2);
    success_symbol[0] = 't'; success_symbol[1] = 0;
    return make_object(SYMBOL, success_symbol);
  } else {
    return make_object(NIL, 0);
  }
}

struct obj* c_fgetc(struct obj* operand, struct env* env) {
  FILE* fptr = LIST_FIRST(operand)->data;
  intptr_t result = getc(fptr);
  if (ferror(fptr)) {
    return make_object(NIL, 0);
  } else {
    return make_object(NUMBER, result);
  }
}

struct obj* c_fputc(struct obj* operand, struct env* env) {
  FILE* fptr = (FILE*) (LIST_FIRST(operand)->data);
  int num = LIST_SECOND(operand)->number;
  int result = putc(num, fptr);
  if (result == -1) {
    return make_object(NIL, 0);
  } else {
    return LIST_SECOND(operand);
  }
}

struct obj* c_fputs(struct obj* operand, struct env* env) {
  FILE* fptr = (FILE*) (LIST_FIRST(operand)->data);
  char* string = LIST_SECOND(operand)->string;
  int result = fputs(string, fptr);
  if (result == -1) {
    return make_object(NIL, 0);
  } else {
    return LIST_SECOND(operand);
  }
}

struct obj* c_strcmp(struct obj* operand, struct env* env) {
  char* str1 = LIST_FIRST(operand)->string;
  char* str2 = LIST_SECOND(operand)->string;
  intptr_t result = strcmp(str1, str2);
  return make_object(NUMBER, result);
}

struct obj* c_exit(struct obj* operand, struct env* env) {
  int code = LIST_FIRST(operand)->number;
  exit(code);
  return make_object(NIL, 0); // not reached
}
