// ********** Derek Leung
// ********** Global data objects

#include "globals.h"

#include <stdlib.h>

#include "io.h"
#include "builtins.h"
#include "eval.h"

struct reader* stdin_reader;
struct table* global_table;

void register_primitive(char* name, struct obj* (*func)(struct obj*)) {
  struct primitive* wrapped = malloc(sizeof(struct primitive));
  wrapped->c_func = func;
  wrapped->name = name;
  bind(global_table, name, make_object(PRIMITIVE, wrapped));
}

void initialize() {
  global_table = make_table(0, 50);
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
  register_primitive("if", &ifelse);
  register_primitive("eval", &builtin_eval);
  register_primitive("apply", &builtin_apply);
  register_primitive("read", &builtin_read);
  register_primitive("print", &builtin_print);
  register_primitive("load", &load);
}
