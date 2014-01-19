// ********** Derek Leung
// ********** POTATO evaluation

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "eval.h"

#include "env.h"
#include "globals.h"
#include "util.h"
#include "data.h"
#include "io.h"

struct obj* proc_operands(struct obj* operand, struct env* env) {
  // this doubles as a shortcut for evaluating all operands
  if (operand->type == NIL) {
    return operand;
  } else if (operand->type != CELL) {
    return make_error("malformed arguments passed to <LIST>");
  }
  struct obj* first = evaluate(operand->cell->first, env);
  struct obj* rest = proc_operands(operand->cell->rest, env);
  if (first->type == ERROR) return first;
  else if (rest->type == ERROR) return rest;
  return make_object(CELL, make_cell(first, rest));
}

#define DEFAULT_FRAME_INITIAL_SIZE 1 // default env size for a function call

struct obj* apply(struct obj* operator, struct obj* operand, struct env* env) {
  if (operator->type == FUNCTION || operator->type == MACRO) {
    struct function* func = operator->data;
    struct obj* procedure = func->body;
    struct obj* params = func->params;
    struct env* parent = func->parent;

    if (list_len(params) != list_len(operand) && list_len(params) != -1) {
      if (DEBUG) {
	printf("\nDEBUG: function called with (%d) parameters but declared "
	       "with (%d) parameters: ", list_len(operand), list_len(params));
	print_obj(operator);
	printf("\n");
      }
      return make_error("function called with incorrect number "
			 "of parameters");
    }

    if (operator->type == FUNCTION) {
      operand = proc_operands(operand, env);
    }
    if (operand->type == ERROR) return operand;

    // set up called function environment
    struct env* call_env;
    call_env = make_env(parent, DEFAULT_FRAME_INITIAL_SIZE);
    if (params->type == SYMBOL) {
      bind(call_env, params->string, operand);
    } else {
      while (params->type != NIL) {
	struct obj* symbol = params->cell->first;
	bind(call_env, symbol->string, operand->cell->first);
	operand = operand->cell->rest;
	params = params->cell->rest;
      }
    }

    // evaluate function body
    if (procedure->type == NIL) {
      return procedure;
    }
    while (procedure->cell->rest->type != NIL) {
      struct obj* ret = evaluate(procedure->cell->first, call_env);
      if (ret->type == ERROR) {
	return ret;
      }
      procedure = procedure->cell->rest;
    }

    // determine what is to be passed back to eval
    if (operand->type == FUNCTION) { // return thunk for tail-recursion
      struct thunk* deferred = malloc(sizeof(struct thunk));
      deferred->env = call_env;
      deferred->obj = procedure->cell->first;
      return make_object(THUNK, deferred);
    } else { // it's a macro; returned value is evaluated anyways
      return evaluate(procedure->cell->first, call_env);
    }
  } else if (operator->type == PRIMITIVE || operator->type == SPCFORM) {
    if (operator->type == PRIMITIVE) {
      operand = proc_operands(operand, env);
    }
    if (operand->type == ERROR) return operand;

    struct obj* (*raw_func)(struct obj*, struct env*) = operator->data;
    return (*raw_func) (operand, env);
  }

  if (DEBUG) {
    printf("Tried to call object: ");
    print_obj(operator);
    printf("\n");
  }
  return make_error("cannot call object");
}

struct obj* real_evaluate(struct obj* obj, struct env* env) {
  struct obj* operator;
  struct obj* applied;

  while (1) {
    switch(obj->type) {
    case NUMBER:
    case STRING:
      return obj;
    case SYMBOL:
      return lookup(env, obj->string);
    case CELL:
      operator = evaluate(obj->cell->first, env);
      if (operator->type == ERROR) {
	if (DEBUG) {
	  printf("\nDEBUG: operator error: ");
	  print_obj(obj->cell->first);
	  printf("\n");
	}
	return operator;
      }
      applied = apply(operator, obj->cell->rest, env);

      if (applied->type == THUNK) {
	obj = ((struct thunk*) (applied->data))->obj;
	env = ((struct thunk*) (applied->data))->env;
	continue;
      } else if (operator->type == MACRO) {
	if (DEBUG == 2) {
	  printf("macro expansion: ");
	  print_obj(applied);
	  printf("\n");
	}
	obj = applied;
	continue;
      } else { // any builtin operator that directly returns
	if (DEBUG) {
	  if (applied->type == ERROR) {
	    printf("in function: ");
	    print_obj(obj->cell->first);
	    printf("\n");
	  }
	}
	return applied;
      }
    case LIBRARY:
      return obj;
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
      if (DEBUG) {
	printf("DEBUG: object type: %d\n", obj->type);
      }
      return make_error(0);
    }
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
    //printf("Entered recursive depth: %d\n", nesting);
  }
  obj = real_evaluate(obj, env);
  if (DEBUG) {
    //printf("Exited recursive depth: %d\n", nesting);
  }
  nesting--;
  return obj;
}

int snprint_list(struct obj* object, char* result, int limit) {
  if (object->type == NIL) {
    return snprintf(result, limit, ")");
  } else if (object->cell->rest->type != CELL &&
	     object->cell->rest->type != NIL) {
    int printed = snprint_obj(object->cell->first, result, limit);
    printed += snprintf(result + printed, limit - printed, " . ");
    printed += snprint_obj(object->cell->rest, result + printed, limit - printed);
    printed += snprintf(result + printed, limit - printed, ")");
    return printed;
  } else {
    int printed = snprint_obj(object->cell->first, result, limit);
    object = object->cell->rest;
    if (object->type != NIL) {
      printed += snprintf(result + printed, limit - printed, " ");
    }
    printed += snprint_list(object, result + printed, limit - printed);
    return printed;
  }
}

void print_obj(struct obj* obj) {
  #define OBJ_PRINT_BUFFER_LIMIT 1000
  static char OBJ_PRINT_BUFFER[OBJ_PRINT_BUFFER_LIMIT]; // TODO make elastic and efficient

  int length = snprint_obj(obj, OBJ_PRINT_BUFFER, OBJ_PRINT_BUFFER_LIMIT);
  if (length >= OBJ_PRINT_BUFFER_LIMIT) {
    printf("ERROR: OBJECT TO BE PRINTED TOO LARGE\n");
    exit(1);
  }
  printf("%s", OBJ_PRINT_BUFFER);
  // printf("%d: %s", length, OBJ_PRINT_BUFFER);
}

int snprint_obj(struct obj* obj, char* result, int limit) {
  if (limit <= 0) {
    printf("ERROR: OUT OF PRINT SPACE\n");
    exit(1);
    return -1;
  } else if (limit == 1) { // write null-term
    result[0] = 0;
    return 0;
  }

  switch(obj->type) {
  case NUMBER:
    return snprintf(result, limit, "%d", obj->number);
  case SYMBOL:
    return snprintf(result, limit, "%s", (char*) obj->data);
  case STRING:
    return snprintf(result, limit, "\"%s\"", (char*) obj->data);
  case NIL:
    return snprintf(result, limit, "nil");
  case PRIMITIVE:
    return snprintf(result, limit, "<PRIMITIVE-OPERATOR@%p>", obj->data);
  case FUNCTION:
    return snprintf(result, limit, "<USER-DEFINED-FUNCTION@%p>", obj->data);
  case MACRO:
    return snprintf(result, limit, "<USER-DEFINED-MACRO@%p>", obj->data);
  case STREAM:
    return snprintf(result, limit, "<DATA-STREAM@%p>", obj->data);
  case LIBRARY:
    return snprintf(result, limit, "<BINARY-LIBRARY@%p>", obj->data);
  case CELL:
    if (snprintf(result, limit, "(") != 1) { // '(' and '\0' should be printed
      printf("ERROR: OUT OF PRINT SPACE\n");
      exit(1);
      return -1;
    } else { // return '(' and printed list
      return 1 + snprint_list(obj, result + 1, limit - 1);
    }
  case ERROR:
    if (obj->data) {
      return snprintf(result, limit, "ERROR: %s", (char*) obj->data);
    }
    // else fall through
  default:
    if (DEBUG) {
      return snprintf(result, limit, "ERROR: BAD OBJECT (PRINT)\nDEBUG: object type: %d\n", obj->type);
    } else {
      return snprintf(result, limit, "ERROR: BAD OBJECT (PRINT)\n");
    }
  }
}

#if DEBUG
char* debug_obj_contents(struct obj* obj) {
  #define DEBUG_OBJ_PRINT_BUFFER_LIMIT 1000
  static char OBJ_PRINT_BUFFER[DEBUG_OBJ_PRINT_BUFFER_LIMIT]; // TODO make elastic and efficient

  int length = snprint_obj(obj, OBJ_PRINT_BUFFER, DEBUG_OBJ_PRINT_BUFFER_LIMIT);
  if (length >= DEBUG_OBJ_PRINT_BUFFER_LIMIT) {
    printf("ERROR: OBJECT TO BE PRINTED TOO LARGE\n");
    exit(1);
  }
  return OBJ_PRINT_BUFFER;
}
#endif

// ********** Derek Leung
// ********** Built-in primitive operators

struct obj* add(struct obj* operand, struct env* env) {
  int result = LIST_FIRST(operand)->number + LIST_SECOND(operand)->number;
  return make_small_object(NUMBER, result);
}

struct obj* sub(struct obj* operand, struct env* env) {
  int result = LIST_FIRST(operand)->number - LIST_SECOND(operand)->number;
  return make_small_object(NUMBER, result);
}

struct obj* mul(struct obj* operand, struct env* env) {
  int result = LIST_FIRST(operand)->number * LIST_SECOND(operand)->number;
  return make_small_object(NUMBER, result);
}

struct obj* floor_div(struct obj* operand, struct env* env) {
  if (LIST_SECOND(operand)->number == 0) {
    return make_error("attempted to divide by zero");
  }
  int result = LIST_FIRST(operand)->number / LIST_SECOND(operand)->number;
  return make_small_object(NUMBER, result);
}

struct obj* mod(struct obj* operand, struct env* env) {
  if (LIST_SECOND(operand)->number == 0) {
    return make_error("mod by zero undefined");
  }
  int result = LIST_FIRST(operand)->number % LIST_SECOND(operand)->number;
  return make_small_object(NUMBER, result);
}

struct obj* equals(struct obj* operand, struct env* env) { // TODO generalize
  if (LIST_FIRST(operand)->type != LIST_SECOND(operand)->type) {
    return make_object(NIL, 0);
  }

  if (LIST_FIRST(operand)->type == NUMBER) {
    if (LIST_FIRST(operand)->number == LIST_SECOND(operand)->number) {
      return LIST_FIRST(operand);
    }
  } else if (LIST_FIRST(operand)->type == NIL) {
    return make_object(SYMBOL, "t");
  } else if (LIST_FIRST(operand)->type == SYMBOL) {
    if (!strcmp(LIST_FIRST(operand)->string, LIST_SECOND(operand)->string)) {
      return LIST_FIRST(operand);
    }
  }
  return make_object(NIL, 0);
}

struct obj* lessthan(struct obj* operand, struct env* env) {
  if (LIST_FIRST(operand)->number < LIST_SECOND(operand)->number) {
    return LIST_FIRST(operand);
  } else {
    return make_object(NIL, 0);
  }
}

struct obj* first(struct obj* operand, struct env* env) {
  return operand->cell->first->cell->first;
}

struct obj* rest(struct obj* operand, struct env* env) {
  return operand->cell->first->cell->rest;
}

struct obj* quote(struct obj* operand, struct env* env) {
  struct obj** processed = prologue(&operand, env, 1, 0, 0, 1, 1, 0);
  if (!processed) {
    return operand;
  }
  return processed[0];
}

struct obj* construct(struct obj* operand, struct env* env) {
  return make_object(CELL, make_cell(LIST_FIRST(operand), LIST_SECOND(operand)));
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
  if (bind(env, processed[0]->string, value)) {
    return processed[0];
  } else {
    if (DEBUG) {
      printf("tried to define: %s", processed[0]->string);
    }
    return make_error("symbol is already defined");
  }
}

struct obj* function(struct obj* operand, struct env* env) {
  if (list_len(operand) <= 0) {
    return make_error("<FUNCTION> requires at least one "
		      "valid argument");
  }
  // parameter-checking
  struct obj* params = operand->cell->first;
  if (params->type == CELL || params->type == NIL) {
    while (params->type != NIL) {
      if (params->type != CELL) {
	return make_error("<FUNCTION> received malformed argument list");
      }
      struct cell* curr = params->cell;
      if (curr->first->type != SYMBOL) {
	return make_error("cannot define functions with non-symbols");
      }
      params = curr->rest;
    }
  } else if (params->type == SYMBOL) {
    ; // pass
  } else {
    return make_error("first argument to <FUNCTION> must either be a list of "
		      "arguments or a symbol");
  }

  struct function* f = malloc(sizeof(struct function));
  f->body = operand->cell->rest;
  f->params = operand->cell->first;
  f->parent = env;
  return make_object(FUNCTION, f);
}

#define TYPEOFCASE(TYPE) {case TYPE: return make_object(SYMBOL, #TYPE);}

struct obj* typeof(struct obj* operand, struct env* env) {
  switch (operand->cell->first->type) { // TODO safe to return const strings?
    TYPEOFCASE(NUMBER);
    TYPEOFCASE(SYMBOL);
    TYPEOFCASE(STRING);
    TYPEOFCASE(NIL);
    TYPEOFCASE(PRIMITIVE);
    TYPEOFCASE(FUNCTION);
    TYPEOFCASE(SPCFORM);
    TYPEOFCASE(MACRO);
    TYPEOFCASE(STREAM);
    TYPEOFCASE(CELL);
  default:
    return make_object(SYMBOL, "NONSTANDARD");
  }
}

struct obj* mark_macro(struct obj* operand, struct env* env) {
  struct obj* copy = make_object(MACRO, operand->cell->first->data);
  return copy;
}

struct obj* ifelse(struct obj* operand, struct env* env) {
  struct obj** processed = prologue(&operand, env, 1, 0, 0, 1, 3, 0);
  if (!processed) {
    return operand;
  }

  struct obj* ret = evaluate(processed[0], env);
  if (ret->type == ERROR) {
    return ret;
  }
  struct thunk* deferred = malloc(sizeof(struct thunk));
  deferred->env = env;
  if (ret->type == NIL) {
    deferred->obj = processed[2];
  } else {
    deferred->obj = processed[1];
  }
  return make_object(THUNK, deferred);
}

struct obj* open(struct obj* operand, struct env* env) {
  FILE* f = fopen(operand->cell->first->string, "r");
  struct reader* reader = make_reader(f);
  struct obj* stream = make_object(STREAM, reader);
  return stream;
}

struct obj* builtin_error(struct obj* operand, struct env* env) {
  struct obj* msgobj = operand->cell->first;
  char* msg = msgobj->string;
  return make_error(msg);
}

struct obj* builtin_eval(struct obj* operand, struct env* env) {
  return evaluate(operand->cell->first, env);
}

struct obj* builtin_apply(struct obj* operand, struct env* env) {
  // prologue() function isn't sophisticated enough to do FUNCTION | MACRO
  // so accept anything for first arg and fail later
  // (this will evaluate second argument even if first does not fit)
  return apply(LIST_FIRST(operand), LIST_SECOND(operand), env);
}

struct obj* builtin_read(struct obj* operand, struct env* env) {
  if (list_len(operand) == 0) {
    return next_object(stdin_reader);
  } else {
    struct cell* cell = operand->data;
    struct obj* ret = next_object(cell->first->data);

    if (ret->type == DONE) {
      if (reader_fclose(cell->first->data)) {
	return make_error("stream failed to close");
      }
    }
    return ret;
  }
}

struct obj* builtin_print(struct obj* operand, struct env* env) {
  print_obj(operand->cell->first);
  printf("\n");
  return operand->cell->first;
}

struct obj* load(struct obj* operand, struct env* env) {
  char* filename = operand->cell->first->string;
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

struct obj* set_first(struct obj* operand, struct env* env) {
  LIST_FIRST(operand)->cell->first = LIST_SECOND(operand);
  return make_object(NIL, 0);
}

struct obj* set_rest(struct obj* operand, struct env* env) {
  LIST_FIRST(operand)->cell->rest = LIST_SECOND(operand);
  return make_object(NIL, 0);
}
