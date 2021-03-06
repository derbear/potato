// ********** Derek Leung
// ********** General utilities

#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "eval.h"
#include "globals.h"

struct obj** flatten(struct obj* list, int* size) {
  int length = list_len(list);
  if (size) {
    *size = length;
  }

  if (length == 0) {
    return 0;
  } else if (length == -1) {
    printf("Tried to flatten a malformed list\n");
    exit(1);
  }

  struct obj** array = malloc(sizeof(struct obj*) * length);
  struct cell* current;
  int index = 0;
  while (list->type != NIL) {
    current = list->data;
    array[index++] = current->first;
    list = current->rest;
  }
  return array;
}

struct obj* unflatten(struct obj** array, int size) {
  struct obj* list = make_object(NIL, 0);
  while (size > 0) {
    list = make_object(CELL, make_cell(array[--size], list));
  }
  return list;
}

int list_len(struct obj* obj) {
  if (obj->type == NIL) {
    return 0;
  }
  if (obj->type != CELL) {
    return -1;
  }
  int ret = list_len(obj->cell->rest);
  if (ret < 0) return ret;
  else return ret+1;
}

#define ERROR_BUFSIZE 1000
struct obj** prologue(struct obj** operand,
		      struct env* env,
		      int is_fixed_length,
		      int is_strict_typed,
		      int is_applicative,
		      int propagates_exceptions,
		      int num_args,
		      obj_type* arg_types) {
  static char ERROR_BUF[ERROR_BUFSIZE];
  int size;
  char* fmt = "function requires %d arg(s)";
  struct obj** flattened = flatten(*operand, &size);

  if (is_fixed_length) {
    if (size != num_args) {
      int errorlen = snprintf(ERROR_BUF, ERROR_BUFSIZE, fmt, num_args);
      char* errorstrptr = malloc(errorlen+1);
      strncpy(errorstrptr, ERROR_BUF, errorlen);
      errorstrptr[errorlen] = 0;
      if (DEBUG) {
        print_obj(*operand);
      }
      *operand = make_object(ERROR, errorstrptr);
      return 0;
    }
  }

  if (is_applicative) {
    for (int i = 0; i < size; i++) {
      flattened[i] = evaluate(flattened[i], env);
      if (propagates_exceptions && flattened[i]->type == ERROR) {
	*operand = flattened[i];
	return 0;
      }
    }
  }

  if (is_strict_typed) {
    for (int i = 0; i < size; i++) {
      if (flattened[i]->type != arg_types[i]) {
	// nil is accepted whenever cells are accepted, and
	// primitives are accepted whenever functions are.
	// declaring a requirement to have type nil indicates any type
	if (!(flattened[i]->type == NIL && arg_types[i] == CELL) &&
	    !(flattened[i]->type == PRIMITIVE && arg_types[i] == FUNCTION) &&
	    arg_types[i] != NIL) {
	  *operand = make_object(ERROR, "incorrect argument type(s)");
	  return 0;
	}
      }
    }
  }

  return flattened;
}
