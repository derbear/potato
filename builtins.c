// ********** Derek Leung
// ********** Built-in primitive operators

#include "builtins.h"

#include <stdlib.h>

#include "env.h"
#include "eval.h"
#include "data.h"
#include "globals.h"
#include "util.h"
#include "io.h"

struct obj* add(struct obj* operand, struct env* env) {
  obj_type types[] = {NUMBER, NUMBER};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }
  int result = processed[0]->number + processed[1]->number;
  return make_small_object(NUMBER, result);
}

struct obj* sub(struct obj* operand, struct env* env) {
  obj_type types[] = {NUMBER, NUMBER};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }
  int result = processed[0]->number - processed[1]->number;
  return make_small_object(NUMBER, result);
}

struct obj* mul(struct obj* operand, struct env* env) {
  obj_type types[] = {NUMBER, NUMBER};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }
  int result = processed[0]->number * processed[1]->number;
  return make_small_object(NUMBER, result);
}

struct obj* floor_div(struct obj* operand, struct env* env) {
  obj_type types[] = {NUMBER, NUMBER};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }
  if (processed[1]->number == 0) {
    return make_error("attempted to divide by zero");
  }
  int result = processed[0]->number / processed[1]->number;
  return make_small_object(NUMBER, result);
}

struct obj* equals(struct obj* operand, struct env* env) {
  obj_type types[] = {NUMBER, NUMBER};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }
  if (processed[0]->number == processed[1]->number) {
    return processed[0];
  } else {
    return make_object(NIL, 0);
  }
}

struct obj* lessthan(struct obj* operand, struct env* env) {
  obj_type types[] = {NUMBER, NUMBER};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }
  if (processed[0]->number < processed[1]->number) {
    return processed[0];
  } else {
    return make_object(NIL, 0);
  }
}

struct obj* first(struct obj* operand, struct env* env) {
  obj_type types[] = {CELL};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 1, types);
  if (!processed) {
    return operand;
  }
  if (processed[0]->type == NIL) {
    return processed[0];
  }
  return processed[0]->cell->first;
}

struct obj* rest(struct obj* operand, struct env* env) {
  obj_type types[] = {CELL};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 1, types);
  if (!processed) {
    return operand;
  }
  if (processed[0]->type == NIL) {
    return processed[0];
  }
  return processed[0]->cell->rest;
}

struct obj* quote(struct obj* operand, struct env* env) {
  struct obj** processed = prologue(&operand, env, 1, 0, 0, 1, 1, 0);
  if (!processed) {
    return operand;
  }
  return processed[0];
}

struct obj* construct(struct obj* operand, struct env* env) {
  struct obj** processed = prologue(&operand, env, 1, 0, 1, 1, 2, 0);
  if (!processed) {
    return operand;
  }
  return make_object(CELL, make_cell(processed[0], processed[1]));
}

struct obj* define(struct obj* operand, struct env* env) {
  struct obj** processed = prologue(&operand, env, 1, 0, 0, 1, 2, 0);
  if (!processed) {
    return operand;
  }
  if (processed[0]->type != SYMBOL) {
    return make_error("the first argument passed to "
		      "<DEFINE> must be a symbol");
  }
  struct obj* value = evaluate(processed[1], env);
  bind(env, processed[0]->string, value);
  return processed[0];
}

struct obj* function(struct obj* operand, struct env* env) {
  if (list_len(operand) <= 0) {
    return make_error("<FUNCTION> requires at least one "
		      "valid argument");
  }
  struct obj* params = operand->cell->first;
  while (params->type != NIL) {
    struct cell* curr = params->cell;
    if (curr->first->type != SYMBOL) {
	return make_error("cannot define functions with non-symbols");
    }
    params = curr->rest;
  }

  struct function* f = malloc(sizeof(struct function));
  f->body = operand->cell->rest;
  f->params = operand->cell->first;
  f->parent = env;
  return make_object(FUNCTION, f);
}

#define TYPEOFCASE(TYPE) {case TYPE: return make_object(SYMBOL, #TYPE);}

struct obj* typeof(struct obj* operand, struct env* env) {
  struct obj** processed = prologue(&operand, env, 1, 0, 1, 1, 1, 0);
  if (!processed) {
    return operand;
  }

  switch (processed[0]->type) { // TODO safe to return const strings?
    TYPEOFCASE(NUMBER);
    TYPEOFCASE(SYMBOL);
    TYPEOFCASE(LITERAL);
    TYPEOFCASE(NIL);
    TYPEOFCASE(PRIMITIVE);
    TYPEOFCASE(FUNCTION);
    TYPEOFCASE(STREAM);
    TYPEOFCASE(CELL);
  default:
    return make_object(SYMBOL, "NONSTANDARD");
  }
}

struct obj* ifelse(struct obj* operand, struct env* env) {
  struct obj** processed = prologue(&operand, env, 1, 0, 0, 1, 3, 0);
  if (!processed) {
    return operand;
  }

  struct obj* ret = evaluate(processed[0], env);
  struct thunk* deferred = malloc(sizeof(struct thunk));
  deferred->env = env;
  if (ret->type == NIL) {
    deferred->obj = processed[2];
  } else {
    deferred->obj = processed[1];
  }
  return make_object(THUNK, deferred);
}

struct obj* builtin_eval(struct obj* operand, struct env* env) {
  struct obj** processed = prologue(&operand, env, 1, 0, 1, 1, 1, 0);
  if (!processed)  {
    return operand;
  }
  return evaluate(processed[0], env);
}

struct obj* builtin_apply(struct obj* operand, struct env* env) {
  obj_type types[] = {FUNCTION, CELL};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }
  return apply(processed[0], processed[1], env);
}

struct obj* builtin_read(struct obj* operand, struct env* env) {
  if (list_len(operand) != 0 &&
      list_len(operand) != 1) {
    return make_error("<READ> takes either zero or one args");
  }

  operand = list(operand, env);
  if (list_len(operand) == 0) {
    return next_object(stdin_reader);
  } else {
    struct cell* cell = operand->data;
    return next_object(cell->first->data);
  }
}

struct obj* builtin_print(struct obj* operand, struct env* env) {
  struct obj** processed = prologue(&operand, env, 1, 0, 1, 1, 1, 0);
  if (!processed) {
    // TODO have something else catch exceptions
    print_obj(operand);
    printf("\n");
    return make_object(NIL, 0);
  }
  print_obj(processed[0]);
  printf("\n");
  return processed[0];
}

struct obj* load(struct obj* operand, struct env* env) {
  obj_type types[] = {LITERAL};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 1, types);
  if (!processed) {
    return operand;
  }

  char* filename = processed[0]->string;
  FILE* f = fopen(filename, "r");
  if (!f) {
    return make_error("file not found");
  }
  struct reader* r = make_reader(f);
  struct obj* next;
  while ((next = next_object(r))) {
    if (next->type == DONE) {
      fclose(f); // TODO implement reader destroy
      return make_object(NIL, 0);
    }
    next = evaluate(next, env);
    if (next->type == ERROR) {
      return next;
    }
  }

  printf("ERROR IN <LOAD>\n"); // should never reach here
  exit(1);
}
