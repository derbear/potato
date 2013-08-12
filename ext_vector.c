#include "ext_vector.h"

#include <stdlib.h>

#include "data.h"
#include "eval.h" // for list_len
#include "util.h"

typedef struct {
  struct obj** data;
  int size;
  int capacity;
} vector;

#define INITIAL_SIZE 1

void ext_vector() {
  register_primitive("vector", &ext_vector_new);
  register_primitive("vector-from-string", &ext_vector_from_string);
  register_primitive("vector-length", &ext_vector_length);
  register_primitive("vector-index", &ext_vector_index);
  register_primitive("vector-set", &ext_vector_set);
  register_primitive("vector-insert", &ext_vector_insert);
  register_primitive("vector-remove", &ext_vector_remove);
}

struct obj* make_out_of_bounds() {
  return make_error("vector index out of bounds");
}

struct obj* ext_vector_new(struct obj* operand) {
  obj_type types[] = {NUMBER};
  struct obj** processed = prologue(&operand, 1, 1, 1, 1, 1, types);

  int size;
  if (!processed) {
    if (list_len(operand) == 0) {
      size = INITIAL_SIZE;
    } else {
      return operand;
    }
  } else {
    size = processed[0]->number;
  }

  vector* v = malloc(sizeof(vector));
  v->data = malloc(sizeof(struct obj*) * size);
  v->size = 0;
  v->capacity = size;
  return make_object(EXTENSION, v);
}

struct obj* ext_vector_from_string(struct obj* operand) {
  obj_type types[] = {LITERAL};
  struct obj** processed = prologue(&operand, 1, 1, 1, 1, 1, types);
  if (!processed) {
    return operand;
  }
  
  char* str = processed[0]->string;
  int length = str_len(str);
  vector* v = malloc(sizeof(vector));
  v->data = malloc(sizeof(struct obj*) * length);
  v->size = length;
  v->capacity = length;

  for (int i = 0; i < length; i++) {
    v->data[i] = make_small_object(NUMBER, str[i]);
  }
  return make_object(EXTENSION, v);
}

struct obj* ext_vector_length(struct obj* operand) {
  obj_type types[] = {EXTENSION};
  struct obj** processed = prologue(&operand, 1, 1, 1, 1, 1, types);
  if (!processed) {
    return operand;
  }
  
  vector* v = processed[0]->data;
  return make_small_object(NUMBER, v->size);
}

struct obj* ext_vector_index(struct obj* operand) {
  obj_type types[] = {EXTENSION, NUMBER};
  struct obj** processed = prologue(&operand, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }
  
  vector* v = processed[0]->data;
  int index = processed[1]->number;
  if (index >= v->size) {
    return make_out_of_bounds();
  }
  return v->data[index];
}

struct obj* ext_vector_set(struct obj* operand) {
  obj_type types[] = {EXTENSION, NUMBER, NIL};
  struct obj** processed = prologue(&operand, 1, 1, 1, 1, 3, types);
  if (!processed) {
    return operand;
  }
  
  vector* v = processed[0]->data;
  int index = processed[1]->number;
  if (index >= v->size) {
    return make_out_of_bounds();
  }
  struct obj* replaced = v->data[index];
  v->data[index] = processed[2];
  return replaced;
}

void expand(vector* v) {
  v->capacity *= 2;
  v->data = realloc(v->data, v->capacity);
}

struct obj* ext_vector_insert(struct obj* operand) {
  obj_type types[] = {EXTENSION, NUMBER, NIL};
  struct obj** processed = prologue(&operand, 1, 1, 1, 1, 3, types);
  if (!processed) {
    return operand;
  }
  
  vector* v = processed[0]->data;
  int index = processed[1]->number;
  if (index > v->size) {
    return make_out_of_bounds();
  }
  
  if (v->size == v->capacity) {
    expand(v);
  }
  v->size++;
  for (int i = v->size; i > index; i--) {
    v->data[i] = v->data[i-1];
  }
  v->data[index] = processed[2];
  return processed[2];
}

struct obj* ext_vector_remove(struct obj* operand) {
  obj_type types[] = {EXTENSION, NUMBER};
  struct obj** processed = prologue(&operand, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }
  
  vector* v = processed[0]->data;
  int index = processed[1]->number;
  if (index >= v->size) {
    return make_out_of_bounds();
  }
  for (int i = index; i < v->size; i++) {
    v->data[i] = v->data[i+1];
  }
  v->size--;
  return v->data[index];
}
