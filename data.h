// ********** Derek Leung
// ********** Shared data definitions

#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

typedef enum {
  DONE = -1,
  ERROR = 0,
  SYMBOL, NUMBER, CELL, NIL, PRIMITIVE, FUNCTION,
  LITERAL, STREAM,
} obj_type;

struct obj {
  obj_type type;
  void* data; // @heap
}; // @heap

struct cell {
  struct obj* first, * rest;
}; // @heap

struct cell* make_cell(struct obj* first, struct obj* rest);
struct obj* make_object(obj_type type, void* data);
struct obj* make_error(char* data);

#endif
