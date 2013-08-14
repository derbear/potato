// ********** Derek Leung
// ********** The POTATO interpreter (until I think of a better name)

/*
 * Bootstrap version:
 * Use what we have to bootstrap a superior interpreter.
 */

#include <stdlib.h>
#include <stdio.h>

#include "globals.h"
#include "env.h"
#include "builtins.h"
#include "eval.h"
#include "io.h"
#include "util.h"

#include "extensions.h"

#define STARTUP_FILE "startup.pot"

void import(void (*init_func)()) {
  init_func();
}

int bootstrap(int argc, char* argv[]) {
  import(ext_bind_stdio);
  import(ext_vector);
  
  struct obj** translated = malloc(sizeof(struct obj*) * argc);
  for (int i = 0; i < argc; i++) {
    translated[i] = make_object(LITERAL, argv[i]); // TODO is this safe?
  }
  struct obj* arg_values = unflatten(translated, argc);
  bind(global_env, "program-arguments", arg_values);
  struct obj* str = make_object(LITERAL, STARTUP_FILE);
  struct obj* arg = make_object(CELL, make_cell(str, make_object(NIL, 0)));
  struct obj* result = load(arg);
  
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
