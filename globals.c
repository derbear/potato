// ********** Derek Leung
// ********** Global data objects

#include "globals.h"

#include "io.h"
#include "builtins.h"
#include "eval.h"

struct reader* stdin_reader;
struct table* global_table;

void initialize() {
  global_table = make_table(0, 50);
  stdin_reader = make_reader(stdin);

  bind(global_table, "list", make_object(PRIMITIVE, &list));
  bind(global_table, "+", make_object(PRIMITIVE, &add));
  bind(global_table, "-", make_object(PRIMITIVE, &sub));
  bind(global_table, "*", make_object(PRIMITIVE, &mul));
  bind(global_table, "//", make_object(PRIMITIVE, &floor_div));
  bind(global_table, "=", make_object(PRIMITIVE, &equals));
  bind(global_table, "<", make_object(PRIMITIVE, &lessthan));
  bind(global_table, "quote", make_object(PRIMITIVE, &quote));
  bind(global_table, "first", make_object(PRIMITIVE, &first));
  bind(global_table, "rest", make_object(PRIMITIVE, &rest));
  bind(global_table, "cons", make_object(PRIMITIVE, &construct));
  bind(global_table, "define", make_object(PRIMITIVE, &define));
  bind(global_table, "func", make_object(PRIMITIVE, &function));
  bind(global_table, "if", make_object(PRIMITIVE, &ifelse));
  bind(global_table, "eval", make_object(PRIMITIVE, &builtin_eval));
  bind(global_table, "apply", make_object(PRIMITIVE, &builtin_apply));
  bind(global_table, "read", make_object(PRIMITIVE, &builtin_read));
  bind(global_table, "print", make_object(PRIMITIVE, &builtin_print));
  bind(global_table, "open", make_object(PRIMITIVE, &file_open));
  bind(global_table, "exec", make_object(PRIMITIVE, &execute));
}
