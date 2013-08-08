// Bindings to stdio.h functions
// TODO this is incomplete

#include "ext_bind_stdio.h"

#include <stdio.h>
#include <stdlib.h>

#include "eval.h"
#include "data.h"

struct obj* ext_bind_stdio_fopen(struct obj* operand) {
  int len = list_len(operand);
  if (len != 2) {
    return make_error("fopen requires two args");
  }
  operand = list(operand);
  
  struct cell* data = operand->data;
  struct obj* first = data->first;
  operand = data->rest;
  data = operand->data;
  struct obj* second = data->first;
  if (first->type != LITERAL || second->type != LITERAL) {
    return make_error("fopen requires string args");
  }
  FILE* fptr = fopen((char*) (first->data), (char*) (second->data));
  
  if (!fptr) {
    return make_object(NIL, 0);
  }
  return make_object(EXTENSION, fptr);
}

struct obj* ext_bind_stdio_fclose(struct obj* operand) {
  int len = list_len(operand);
  if (len != 1) {
    return make_error("fclose requires one arg");
  }
  operand = list(operand);
  
  struct cell* data = operand->data;
  struct obj* first = data->first;
  // TODO we need some kind of extension object "signing" so we don't do
  // operations on invalid file objects
  if (first->type != EXTENSION) {
    return make_error("fclose needs a file arg");
  }

  int result = fclose((FILE*) (first->data));
  if (result == 0) {
    char* success_symbol = malloc(sizeof(char) * 2);
    success_symbol[0] = 't'; success_symbol[1] = 0;
    return make_object(SYMBOL, success_symbol);
  } else {
    return make_object(NIL, 0);
  }
}

struct obj* ext_bind_stdio_fgetc(struct obj* operand) {
  int len = list_len(operand);
  if (len != 1) {
    return make_error("fgetc requires one arg");
  }
  operand = list(operand);
  
  struct cell* data = operand->data;
  struct obj* first = data->first;
  // TODO we need some kind of extension object "signing" so we don't do
  // operations on invalid file objects
  if (first->type != EXTENSION) {
    return make_error("fgetc needs a file arg");
  }

  FILE* fptr = first->data;
  int result = getc(fptr);
  if (ferror(fptr)) {
    return make_object(NIL, 0);
  } else {
    int* n = malloc(sizeof(int));
    *n = result;
    return make_object(NUMBER, n);
  }
}

struct obj* ext_bind_stdio_fputc(struct obj* operand) {
  int len = list_len(operand);
  if (len != 2) {
    return make_error("fputc requires two args");
  }
  operand = list(operand);
  
  struct cell* data = operand->data;
  struct obj* first = data->first;
  operand = data->rest;
  data = operand->data;
  struct obj* second = data->first;
  if (first->type != EXTENSION || second->type != NUMBER) {
    return make_error("fputc requires args (STDLIB_FILE, NUMBER)");
  }

  FILE* fptr = (FILE*) (first->data);
  int* num = second->data;
  int result = putc(*num, fptr);
  if (result == -1) {
    return make_object(NIL, 0);
  } else {
    return second;
  }
}


// TODO function decl template, maybe functils?
