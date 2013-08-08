// ********** Derek Leung
// ********** The POTATO interpreter (until I think of a better name)

/*
 * Bootstrap version:
 * Use what we have to bootstrap a superior interpreter.
 */

#include <stdlib.h>
#include <stdio.h>

#include "globals.h"
#include "builtins.h"
#include "eval.h"
#include "io.h"

#include "extensions.h"

#define STARTUP_FILE "startup.pot"

// flattens a cell-linked list into an array; returns null ptr if NIL
// throws up if error
// TODO test this
struct obj** flatten(struct obj* list) {
  int length = list_len(list);
  if (length == 0) {
    return 0;
  } else if (length == -1) {
    printf("Tried to flatten a malformed list\n");
    exit(1);
  }
  struct obj** array = malloc(sizeof(struct obj*) * length);
  struct cell* current = list->data;
  int index = 0;
  while (current->first->type != NIL) {
    array[index++] = current->first;
    list = current->rest;
    current = list->data;
  }
  return array;
}

// inverse of flatten
struct obj* unflatten(struct obj** array, int size) {
  struct obj* list = make_object(NIL, 0);
  while (size > 0) {
    list = make_object(CELL, make_cell(array[--size], list));
  }
  return list;
}

int bootstrap(int argc, char* argv[]) {
  register_primitive("c-fopen", &ext_bind_stdio_fopen);
  register_primitive("c-fclose", &ext_bind_stdio_fclose);
  register_primitive("c-fgetc", &ext_bind_stdio_fgetc);
  register_primitive("c-fputc", &ext_bind_stdio_fputc);
  
  struct obj** translated = malloc(sizeof(struct obj*) * argc);
  for (int i = 0; i < argc; i++) {
    translated[i] = make_object(LITERAL, argv[i]); // TODO is this safe?
  }
  struct obj* arg_values = unflatten(translated, argc);
  bind(global_table, "program-arguments", arg_values);
  FILE* f = fopen(STARTUP_FILE, "r");
  if (!f) {
    printf("ERROR: cannot open startup file \"%s\"", STARTUP_FILE);
    return 1;
  }
  struct obj* str = make_object(STREAM, make_reader(f));
  struct obj* arg = make_object(CELL, make_cell(str, make_object(NIL, 0)));
  struct obj* result = execute(arg);
  
  if (result->type == NIL) {
    return 0;
  }
  print_obj(result); // error
  printf("\n");
  return 1;
}

int main(int argc, char* argv[]) {
  initialize();
  int exit_val = bootstrap(argc, argv);
  return exit_val;
}
