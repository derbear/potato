#include "formatting.h"

#include <stdlib.h>
#include <stdio.h>

#include "data.h"
#include "builtins.h"
#include "util.h"

struct obj* c_snprintf_binding(struct obj* op_list, struct env* env) {
  struct obj* arg_proc = list(op_list, env);
  struct obj* fmt_str_obj = arg_proc->cell->first;
  struct obj* args = arg_proc->cell->rest;
  struct obj* fmt_size_obj = args->cell->first;
  args = args->cell->rest;

  void* print_args[16];
  int i;
  for (i = 0; i < 16; i++) {
    if (args->type == NIL) {
      print_args[i] = 0;
    } else {
      print_args[i] = args->cell->first->data;
      args = args->cell->rest;
    }
  }

  const char* fmt_str = fmt_str_obj->string;
  int fmt_size = fmt_size_obj->number;
  char* target = malloc(sizeof(char) * (fmt_size + 1));
  void** p = print_args;
  snprintf(target, fmt_size, fmt_str,
	   p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
	   p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
  target[fmt_size] = 0; // always null-term
  return make_object(LITERAL, target);
}
