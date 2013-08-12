// ********** Derek Leung
// ********** POTATO evaluation

#include <stdlib.h>
#include <stdio.h>

#include "eval.h"

#include "globals.h"
#include "util.h"
#include "data.h"
#include "io.h"

typedef struct {
  char* name; // @heap
  struct obj* object;
} s_entry;

struct table {
  struct table* parent;
  s_entry* list; // @heap
  int next_free;
  int size;
}; // @heap

s_entry make_entry(char* name, struct obj* object) {
  s_entry entry;
  entry.name = name;
  entry.object = object;
  return entry;
}

struct table* make_table(struct table* parent, int size) {
  struct table* table = malloc(sizeof(struct table));
  table->parent = parent;
  table->list = malloc(sizeof(s_entry) * size);
  table->next_free = 0;
  table->size = size;
  return table;
}

void bind(struct table* table, char* name, struct obj* obj) {
  if (table->next_free == table->size) {
    printf("ERROR: TABLE FULL\n");
    exit(1);
  }
  table->list[(table->next_free)++] = make_entry(name, obj);
}

struct obj* lookup(struct table* table, char* name) {
  int i = 0;
  while (i < table->next_free) {
    s_entry entry = table->list[i];
    if (str_eq(name, entry.name)) {
      return entry.object;
    }
    i++;
  }
  if (table->parent) {
    return lookup(table->parent, name);
  }
  if (DEBUG) {
    printf("%s", name);
  }
  return make_error("cannot find symbol");
}

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

struct obj* list(struct obj* operand) { // this doubles as a shortcut for evaluating all operands
  if (operand->type == NIL) {
    return operand;
  } else if (operand->type != CELL) {
    return make_error("malformed arguments passed to <LIST>");
  }
  struct obj* first = evaluate(operand->cell->first);
  struct obj* rest = list(operand->cell->rest);
  if (first->type == ERROR) return first;
  else if (rest->type == ERROR) return rest;
  return make_object(CELL, make_cell(first, rest));
}

struct obj* apply(struct obj* operator, struct obj* operand) {
  if (operator->type == FUNCTION) {
    struct obj* params = operator->cell->first;
    struct obj* procedure = operator->cell->rest;
    if (list_len(params) != list_len(operand)) {
      return make_error("function called with incorrect number "
			 "of parameters");
    }

    operand = list(operand);
    if (operand->type == ERROR) return operand;

    global_table = make_table(global_table, 10); // TODO use real local frames
    while (params->type != NIL) {
      struct obj* symbol = params->cell->first;
      bind(global_table, symbol->string, operand->cell->first);
      operand = operand->cell->rest;
      params = params->cell->rest;
    }

    struct obj* ret = make_object(NIL, 0);
    while (procedure->type != NIL) {
      ret = evaluate(procedure->cell->first);
      procedure = procedure->cell->rest;
    }
    global_table = global_table->parent;
    return ret;
  }

  if (operator->type == PRIMITIVE) {
    struct primitive* wrapper = operator->data;
    struct obj* (*raw_func)(struct obj*) = wrapper->c_func;
    return (*raw_func) (operand);
  }
  return make_error("cannot call object");
}

struct obj* real_evaluate(struct obj* obj) {
  struct obj* operator;

  switch(obj->type) {
  case NUMBER:
  case LITERAL:
    return obj;
  case SYMBOL:
    return lookup(global_table, obj->string);
  case CELL:
    operator = evaluate(obj->cell->first);
    if (operator->type == ERROR) {
      return operator;
    }
    return apply(operator, obj->cell->rest);
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

struct obj* evaluate(struct obj* obj) {
  static int nesting = 0;
  if (nesting > HARD_NESTING_LIMIT) {
    nesting = 0;
    return make_error("exceeded call stack limit");
  }

  nesting++;
  obj = real_evaluate(obj);
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
