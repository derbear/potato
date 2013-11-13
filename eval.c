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

struct obj* list(struct obj*, struct env*); // forward declaration for apply()

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
      operand = list(operand, env);
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
  } else if (operator->type == PRIMITIVE) {
    struct primitive* wrapper = operator->data;
    struct obj* (*raw_func)(struct obj*, struct env*) = wrapper->c_func;
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
    case LITERAL:
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
    if (object->type != NIL) {
      printf(" ");
    }
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
  case MACRO:
    printf("<USER-DEFINED-MACRO>");
    break;
  case STREAM:
    printf("<DATA-STREAM>");
    break;
  case LIBRARY:
    printf("<BINARY-LIBRARY>");
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

// ********** Derek Leung
// ********** Built-in primitive operators

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

struct obj* equals(struct obj* operand, struct env* env) { // TODO generalize
  obj_type types[] = {NIL, NIL};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }

  if (processed[0]->type != processed[1]->type) {
    return make_object(NIL, 0);
  }

  if (processed[0]->type == NUMBER) {
    if (processed[0]->number == processed[1]->number) {
      return processed[0];
    }
  } else if (processed[0]->type == NIL) {
    return make_object(SYMBOL, "t");
  } else if (processed[0]->type == SYMBOL) {
    if (str_eq(processed[0]->string, processed[1]->string)) {
      return processed[0];
    }
  }
  return make_object(NIL, 0);
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
    TYPEOFCASE(MACRO);
    TYPEOFCASE(STREAM);
    TYPEOFCASE(CELL);
  default:
    return make_object(SYMBOL, "NONSTANDARD");
  }
}

struct obj* mark_macro(struct obj* operand, struct env* env) {
  obj_type types[] = {FUNCTION};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 1, types);
  if (!processed) {
    return operand;
  }

  struct obj* copy = make_object(MACRO, processed[0]->data);
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
  obj_type types[] = {LITERAL};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 1, types);
  if (!processed) {
    return operand;
  }

  FILE* f = fopen(processed[0]->string, "r");
  struct reader* reader = make_reader(f);
  struct obj* stream = make_object(STREAM, reader);
  return stream;
}

struct obj* builtin_error(struct obj* operand, struct env* env) {
  obj_type types[] = {LITERAL};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 1, types);
  if (!processed) {
    return operand;
  }

  struct obj* msgobj = processed[0];
  char* msg = msgobj->string;
  return make_error(msg);
}

struct obj* builtin_eval(struct obj* operand, struct env* env) {
  struct obj** processed = prologue(&operand, env, 1, 0, 1, 1, 1, 0);
  if (!processed) {
    return operand;
  }
  return evaluate(processed[0], env);
}

struct obj* builtin_apply(struct obj* operand, struct env* env) {
  // prologue() function isn't sophisticated enough to do FUNCTION | MACRO
  // so accept anything for first arg and fail later
  // (this will evaluate second argument even if first does not fit)
  obj_type types1[] = {NIL, CELL};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 2, types1);
  if (!processed) {
    return operand;
  } else if (!(processed[0]->type == FUNCTION ||
	       processed[0]->type == PRIMITIVE ||
	       processed[0]->type == MACRO)) {
    return make_error("apply requires a callable as its first argument");
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

struct obj* set_first(struct obj* operand, struct env* env) {
  obj_type types[] = {CELL, NIL};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }

  processed[0]->cell->first = processed[1];
  return make_object(NIL, 0);
}

struct obj* set_rest(struct obj* operand, struct env* env) {
  obj_type types[] = {CELL, NIL};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }

  processed[0]->cell->rest = processed[1];
  return make_object(NIL, 0);
}
