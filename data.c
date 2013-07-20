// ********** Derek Leung
// ********** Shared data definitions


#include <stdlib.h>

#include "util.h"
#include "data.h"
#include "eval.h" // for debug
#include "globals.h" // for debug

struct cell* make_cell(struct obj* first, struct obj* rest) {
  struct cell* cell = malloc(sizeof(struct cell));
  cell->first = first;
  cell->rest = rest;
  return cell;
}

struct obj* make_object(obj_type type, void* data) {
  struct obj* object = malloc(sizeof(struct obj));
  object->type = type;
  object->data = data;
  return object;
}

struct obj* make_error(char* data) {
  struct obj* err = make_object(ERROR, 0);
  if (DEBUG) print_obj(err);
  if (!data) return err;

  char* copy = malloc(str_len(data) + 1);
  str_cpy(data, copy);
  err->data = copy;
  if (DEBUG) print_obj(err);
  return err;
}
