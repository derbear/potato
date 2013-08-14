// Bindings to stdio.h functions
// TODO this is incomplete

#include "ext_bind_stdio.h"

#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "eval.h"
#include "env.h"
#include "data.h"
#include "util.h"

void ext_bind_stdio() {
  register_primitive("c-fopen", &ext_bind_stdio_fopen);
  register_primitive("c-fclose", &ext_bind_stdio_fclose);
  register_primitive("c-fgetc", &ext_bind_stdio_fgetc);
  register_primitive("c-fputc", &ext_bind_stdio_fputc);
  register_primitive("c-getc", &ext_bind_stdio_fgetc);
  register_primitive("c-putc", &ext_bind_stdio_fputc);

  bind(global_env, "sys-stdout", make_object(EXTENSION, stdout));
  bind(global_env, "sys-stdin", make_object(EXTENSION, stdin));
  bind(global_env, "sys-stderr", make_object(EXTENSION, stderr));
}

struct obj* ext_bind_stdio_fopen(struct obj* operand, struct env* env) {
  obj_type types[] = {LITERAL, LITERAL};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }
  FILE* fptr = fopen(processed[0]->string, processed[1]->string);

  if (!fptr) {
    return make_object(NIL, 0);
  }
  return make_object(EXTENSION, fptr);
}

struct obj* ext_bind_stdio_fclose(struct obj* operand, struct env* env) {
  obj_type types[] = {EXTENSION};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 1, types);
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

struct obj* ext_bind_stdio_fgetc(struct obj* operand, struct env* env) {
  obj_type types[] = {EXTENSION};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 1, types);
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

struct obj* ext_bind_stdio_fputc(struct obj* operand, struct env* env) {
  obj_type types[] = {EXTENSION, NUMBER};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 2, types);
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
