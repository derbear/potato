// ********** Derek Leung
// ********** Built-in primitive operators

#include <stdlib.h>

#include "builtins.h"

#include "eval.h"
#include "data.h"
#include "globals.h"
#include "util.h"
#include "io.h"

struct obj* add(struct obj* operand) {
  int sum = 0;
  struct cell* curr;

  operand = list(operand);
  if (operand->type == ERROR) return operand;

  while (operand->type != NIL) {
    curr = operand->data;
    if (curr->first->type != NUMBER) {
      return make_error("cannot <ADD> non-numbers");
    }
    sum += *((int*) curr->first->data);
    operand = curr->rest;
  }

  int* value = malloc(sizeof(int));
  *value = sum;
  return make_object(NUMBER, value);
}

struct obj* sub(struct obj* operand) {
  struct cell* curr;

  operand = list(operand);
  if (operand->type == ERROR) return operand;

  if (list_len(operand) == 0) {
    return make_error("must <SUB> at least one number");
  }

  curr = operand->data;
  int diff = *((int*) curr->first->data);
  if (list_len(operand) == 1) {
    int* ret = malloc(sizeof(int));
    *ret = diff * -1;
    return make_object(NUMBER, ret);
  }

  operand = curr->rest;
  while (operand->type != NIL) {
    curr = operand->data;
    if (curr->first->type != NUMBER) {
      return make_error("cannot <SUB> non-numbers");
    }
    diff -= *((int*) curr->first->data);
    operand = curr->rest;
  }

  int* value = malloc(sizeof(int));
  *value = diff;
  return make_object(NUMBER, value);
}

struct obj* mul(struct obj* operand) {
  int product = 1;
  struct cell* curr;

  operand = list(operand);
  if (operand->type == ERROR) return operand;

  while (operand->type != NIL) {
    curr = operand->data;
    if (curr->first->type != NUMBER) {
      return make_error("cannot <MUL> non-numbers");
    }
    product *= *((int*) curr->first->data);
    operand = curr->rest;
  }

  int* value = malloc(sizeof(int));
  *value = product;
  return make_object(NUMBER, value);
}

struct obj* floor_div(struct obj* operand) {
  struct cell* curr;

  operand = list(operand);
  if (operand->type == ERROR) return operand;

  if (list_len(operand) == 0) {
    return make_error("must <FLOORDIV> at least one number");
  }

  curr = operand->data;
  int quot = *((int*) curr->first->data);
  if (list_len(operand) == 1) {
    int* ret = malloc(sizeof(int));
    if (!quot) {
      return make_error("attempted to divide by zero");
    }
    *ret = 1 / quot;
    return make_object(NUMBER, ret);
  }

  operand = curr->rest;
  while (operand->type != NIL) {
    curr = operand->data;
    if (curr->first->type != NUMBER) {
      return make_error("cannot <FLOORDIV> non-numbers");
    }
    int div = *((int*) curr->first->data);
    if (!div) {
      return make_error("attempted to divide by zero");
    }
    quot /= div;
    operand = curr->rest;
  }

  int* value = malloc(sizeof(int));
  *value = quot;
  return make_object(NUMBER, value);
}

struct obj* equals(struct obj* operand) {
  if (list_len(operand) < 2) {
    return make_error("must compare at least two objects");
  }
  operand = list(operand);
  struct cell* args = operand->data;
  struct obj* first = args->first;
  operand = args->rest;
  while (operand->type != NIL) {
    args = operand->data;
    struct obj* cmp = args->first;
    if (first->type == NUMBER && cmp->type == NUMBER) {
      if (*((int*) (first->data)) != *((int*) (cmp->data))) {
	return make_object(NIL, 0);
      }
    } else {
      return make_error("can only compare numbers");
    }
    operand = args->rest;
  }
  return first;
}

struct obj* lessthan(struct obj* operand) {
  if (list_len(operand) != 2) {
    return make_error("must compare exactly two objects");
  }
  operand = list(operand);
  struct cell* args = operand->data;
  struct obj* first = args->first;
  args = args->rest->data;
  struct obj* second = args->first;
  if (first->type != NUMBER && second->type != NUMBER) {
    return make_error("can only compare numbers");
  }
  if (*((int*) (first->data)) < *((int*) (second->data))) {
    return first;
  } else {
    return make_object(NIL, 0);
  }
}

struct obj* first(struct obj* operand) {
  if (list_len(operand) != 1) {
    return make_error("<FIRST> accepts exactly one argument");
  }
  operand = list(operand);
  if (operand->type == ERROR) return operand;

  struct cell* cell = operand->data;
  if (cell->first->type != CELL) {
    return make_error("can only call <FIRST> on a cell");
  }
  cell = cell->first->data;

  return cell->first;
}

struct obj* rest(struct obj* operand) {
  if (list_len(operand) != 1) {
    return make_error("<REST> accepts exactly one argument");
  }
  operand = list(operand);
  if (operand->type == ERROR) return operand;

  struct cell* cell = operand->data;
  if (cell->first->type != CELL) {
    return make_error("can only call <REST> on a cell");
  }
  cell = cell->first->data;

  return cell->rest;
}

struct obj* quote(struct obj* operand) {
  if (operand->type != CELL) {
    return make_error("malformed arguments passed to <QUOTE>");
  }
  if (list_len(operand) != 1) {
    return make_error("<QUOTE> expects exactly one argument");
  }
  struct cell* cell = operand->data;
  return cell->first;
}

struct obj* construct(struct obj* operand) {
  if (list_len(operand) != 2) {
    return make_error("<CONSTRUCT> accepts exactly two arguments");
  }
  operand = list(operand);
  if (operand->type == ERROR) return operand;

  struct cell* one = operand->data;
  struct cell* two = one->rest->data;

  return make_object(CELL, make_cell(one->first, two->first));
}

struct obj* define(struct obj* operand) {
  if (list_len(operand) != 2) {
    return make_error("<DEFINE> accepts exactly two arguments");
  }

  struct cell* one = operand->data;
  if (one->first->type != SYMBOL) {
    return make_error("the first argument passed to "
		       "<DEFINE> must be a symbol");
  }
  struct cell* two = one->rest->data;
  struct obj* value = evaluate(two->first);
  bind(global_table, (char*) one->first->data, value);
  return one->first;
}

struct obj* function(struct obj* operand) {
  if (list_len(operand) <= 0) {
    return make_error("<FUNCTION> requires at least one "
		       "valid argument");
  }
  struct cell* funcdef = operand->data;
  struct obj* params = funcdef->first;
  while (params->type != NIL) {
    struct cell* curr = params->data;
    if (curr->first->type != SYMBOL) {
	return make_error("cannot define functions with non-symbolic parameters");
    }
    params = curr->rest;
  }
  return make_object(FUNCTION, operand->data);
}

struct obj* ifelse(struct obj* operand) {
  if (list_len(operand) != 3) {
    return make_error("<IF> requires three arguments");
  }

  struct cell* pred = operand->data;
  struct cell* cons = pred->rest->data;
  struct cell* alt = cons->rest->data;

  struct obj* ret = evaluate(pred->first);
  if (ret->type == NIL) {
    return evaluate(alt->first);
  } else {
    return evaluate(cons->first);
  }
}

struct obj* builtin_eval(struct obj* operand) {
  if (list_len(operand) != 1) {
    return make_error("<EVAL> requires exactly one argument");
  }
  operand = list(operand);
  if (operand->type == ERROR) return operand;
  struct cell* single = operand->data;
  return evaluate(single->first);
}

struct obj* builtin_apply(struct obj* operand) {
  if (list_len(operand) != 2) {
    return make_error("<APPLY> requires two args");
  }
  operand = list(operand);
  struct cell* one = operand->data;
  struct cell* two = one->rest->data;
  return apply(one->first, two->first);
}

struct obj* builtin_read(struct obj* operand) {
  if (list_len(operand) != 0 &&
      list_len(operand) != 1) {
    return make_error("<READ> takes either zero or one args");
  }

  operand = list(operand);
  if (list_len(operand) == 0) {
    return next_object(stdin_reader);
  } else {
    struct cell* cell = operand->data;
    return next_object(cell->first->data);
  }
}

struct obj* builtin_print(struct obj* operand) {
  if (list_len(operand) != 1 &&
      list_len(operand) != 2) {
    return make_error("<PRINT> takes either one or two args");
  }

  operand = list(operand);
  struct cell* extract = operand->data;
  if (list_len(operand) == 1) {
    print_obj(extract->first);
    printf("\n");
    return extract->first;
  }

  return make_error("stream print not yet supported");
}

struct obj* load(struct obj* operand) {
  if (list_len(operand) != 1) {
    return make_error("<LOAD> takes one arg");
  }
  operand = list(operand);

  struct cell* extract = operand->data;
  if (extract->first->type != LITERAL) {
    return make_error("<LOAD> takes a string arg");
  }
  char* filename = extract->first->data;
  FILE* f = fopen(filename, "r");
  struct reader* r = make_reader(f);
  struct obj* next;
  while ((next = next_object(r))) {
    if (next->type == DONE) {
      fclose(f); // TODO implement reader destroy
      return make_object(NIL, 0);
    }
    next = evaluate(next);
    if (next->type == ERROR) {
      return next;
    }
  }

  printf("ERROR IN <LOAD>\n"); // should never reach here
  exit(1);
}
