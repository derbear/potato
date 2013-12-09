// ********** Derek Leung
// ********** Global data objects

#include "globals.h"

#include <stdlib.h>

#include "io.h"
#include "bin.h"
#include "data.h"
#include "builtins.h"
#include "eval.h"
#include "env.h"

struct reader* stdin_reader;
struct env* global_env;

void register_primitive(char* name, struct obj* (*func)(struct obj*,
							struct env*)) {
  bind(global_env, name, make_object(PRIMITIVE, func));
}

/* TODO this is a stopgap to allow testing - remove later */
struct obj* global(struct obj* operand, struct env* env) {
  return define(operand, global_env);
}

#define DEFAULT_GLOBAL_ENVIRONMENT_INITIAL_SIZE 50

void initialize() {
  global_env = make_env(0, DEFAULT_GLOBAL_ENVIRONMENT_INITIAL_SIZE);
  stdin_reader = make_reader(stdin);

  register_primitive("list", &list);
  register_primitive("+", &add);
  register_primitive("-", &sub);
  register_primitive("*", &mul);
  register_primitive("//", &floor_div);
  register_primitive("=", &equals);
  register_primitive("<", &lessthan);
  register_primitive("quote", &quote);
  register_primitive("first", &first);
  register_primitive("rest", &rest);
  register_primitive("cons", &construct);
  register_primitive("define", &define);
  register_primitive("func", &function);
  register_primitive("type", &typeof);
  register_primitive("macro", &mark_macro);
  register_primitive("if", &ifelse);
  register_primitive("open", &open);
  register_primitive("error", &builtin_error);
  register_primitive("eval", &builtin_eval);
  register_primitive("apply", &builtin_apply);
  register_primitive("read", &builtin_read);
  register_primitive("print", &builtin_print);
  register_primitive("load", &load);
  register_primitive("set-first", &set_first);
  register_primitive("set-rest", &set_rest);

  register_primitive("bin-load", &bin_load);
  register_primitive("bin-get", &bin_get);
  register_primitive("bin-close", &bin_close);

  register_primitive("global", &global);
}
