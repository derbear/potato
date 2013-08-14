// ********** Derek Leung
// ********** POTATO evaluation

#include <stdlib.h>
#include <stdio.h>

#include "eval.h"

#include "env.h"
#include "globals.h"
#include "util.h"
#include "data.h"
#include "io.h"


//////////

int list_len(struct obj* obj) { // neg return if malformed
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

struct obj* list(struct obj* operand, struct env* env) {
  // this doubles as a shortcut for evaluating all operands
  if (operand->type == NIL) {
    return operand;
  } else if (operand->type != CELL) {
    return make_error("malformed arguments passed to <LIST>");
  }
  struct obj* first = evaluate(operand->cell->first, env);
  struct obj* rest = list(operand->cell->rest, env);
  if (first->type == ERROR) return first;
  else if (rest->type == ERROR) return rest;
  return make_object(CELL, make_cell(first, rest));
}

struct obj* apply(struct obj* operator, struct obj* operand, struct env* env) {
  if (operator->type == FUNCTION) {
    struct function* func = operator->data;
    struct obj* procedure = func->body;
    struct obj* params = func->params;
    struct env* parent = func->parent;
    if (list_len(params) != list_len(operand)) {
      return make_error("function called with incorrect number "
			 "of parameters");
    }

    operand = list(operand, env);
    if (operand->type == ERROR) return operand;

    struct env* call_env;
    call_env = make_env(parent, 10);
    while (params->type != NIL) {
      struct obj* symbol = params->cell->first;
      bind(call_env, symbol->string, operand->cell->first);
      operand = operand->cell->rest;
      params = params->cell->rest;
    }

    struct obj* ret = make_object(NIL, 0);
    while (procedure->type != NIL) {
      ret = evaluate(procedure->cell->first, call_env);
      procedure = procedure->cell->rest;
    }
    return ret;
  }

  if (operator->type == PRIMITIVE) {
    struct primitive* wrapper = operator->data;
    struct obj* (*raw_func)(struct obj*, struct env*) = wrapper->c_func;
    return (*raw_func) (operand, env);
  }
  if (DEBUG) {
    printf("Tried to call object: ");
    print_obj(operator);
  }
  return make_error("cannot call object");
}

struct obj* real_evaluate(struct obj* obj, struct env* env) {
  struct obj* operator;

  switch(obj->type) {
  case NUMBER:
  case LITERAL:
    return obj;
  case SYMBOL:
    return lookup(env, obj->string);
  case CELL:
    operator = evaluate(obj->cell->first, env);
    if (operator->type == ERROR) {
      return operator;
    }
    return apply(operator, obj->cell->rest, env);
  case STREAM:
    return obj;
  case NIL:
    return obj;
  case ERROR:
    return obj;
  case DONE:
    return obj;
  default:
    printf("ERROR: BAD OBJECT (EVAL)\n");
    return make_error(0);
  }
}

#define HARD_NESTING_LIMIT 1000

struct obj* evaluate(struct obj* obj, struct env* env) {
  static int nesting = 0;
  if (nesting > HARD_NESTING_LIMIT) {
    nesting = 0;
    return make_error("exceeded call stack limit");
  }
  nesting++;
  if (DEBUG) {
    printf("Entered recursive depth: %d\n", nesting);
  }
  obj = real_evaluate(obj, env);
  if (DEBUG) {
    printf("Exited recursive depth: %d\n", nesting);
  }
  nesting--;
  return obj;
}

void print_list(struct obj* object) {
  if (object->type == NIL) {
    printf(")");
  } else if (object->cell->rest->type != CELL &&
	     object->cell->rest->type != NIL) {
    print_obj(object->cell->first);
    printf(" . ");
    print_obj(object->cell->rest);
    printf(")");
  } else {
    print_obj(object->cell->first);
    object = object->cell->rest;
    if (object->type != NIL)
      printf(" ");
    print_list(object);
  }
}

void print_obj(struct obj* obj) {
  switch(obj->type) {
  case NUMBER:
    printf("%d", obj->number);
    break;
  case SYMBOL:
    printf("%s", (char*) obj->data);
    break;
  case LITERAL:
    printf("\"%s\"", (char*) obj->data);
    break;
  case NIL:
    printf("nil");
    break;
  case PRIMITIVE:
    printf("<PRIMITIVE-OPERATOR>");
    break;
  case FUNCTION:
    printf("<USER-DEFINED-FUNCTION>");
    break;
  case STREAM:
    printf("<DATA-STREAM>");
    break;
  case CELL:
    printf("(");
    print_list(obj);
    break;
  case ERROR:
    if (obj->data) {
      printf("ERROR: %s", (char*) obj->data);
      break;
    }
    // fall through
  default:
    printf("ERROR: BAD OBJECT (PRINT)\n");
  }
}
