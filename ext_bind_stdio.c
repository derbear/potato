// Bindings to stdio.h functions
// TODO this is incomplete

#include "ext_bind_stdio.h"

#include <stdio.h>
#include <stdlib.h>

#include "eval.h"
#include "data.h"
#include "util.h"

struct obj* ext_bind_stdio_fopen(struct obj* operand) {
  obj_type types[] = {LITERAL, LITERAL};
  struct obj** processed = prologue(&operand, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }
  FILE* fptr = fopen(processed[0]->string, processed[1]->string);

  if (!fptr) {
    return make_object(NIL, 0);
  }
  return make_object(EXTENSION, fptr);
}

struct obj* ext_bind_stdio_fclose(struct obj* operand) {
  obj_type types[] = {EXTENSION};
  struct obj** processed = prologue(&operand, 1, 1, 1, 1, 1, types);
  if (!processed) {
    return operand;
  }
  
  // TODO we need some kind of extension object "signing" so we don't do
  // operations on invalid file objects

  int result = fclose((FILE*) (processed[0]->data));
  if (result == 0) {
    char* success_symbol = malloc(sizeof(char) * 2);
    success_symbol[0] = 't'; success_symbol[1] = 0;
    return make_object(SYMBOL, success_symbol);
  } else {
    return make_object(NIL, 0);
  }
}

struct obj* ext_bind_stdio_fgetc(struct obj* operand) {
  obj_type types[] = {EXTENSION};
  struct obj** processed = prologue(&operand, 1, 1, 1, 1, 1, types);
  if (!processed) {
    return operand;
  }
  
  FILE* fptr = processed[0]->data;
  int result = getc(fptr);
  if (ferror(fptr)) {
    return make_object(NIL, 0);
  } else {
    return make_small_object(NUMBER, result);
  }
}

struct obj* ext_bind_stdio_fputc(struct obj* operand) {
  obj_type types[] = {EXTENSION, NUMBER};
  struct obj** processed = prologue(&operand, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }
  FILE* fptr = (FILE*) (processed[0]->data);
  int num = processed[1]->number;
  int result = putc(num, fptr);
  if (result == -1) {
    return make_object(NIL, 0);
  } else {
    return processed[1];
  }
}
