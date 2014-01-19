// ********** Derek Leung
// ********** Shared data definitions

#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#include <stdint.h>

struct env; // forward declaration

typedef enum {
  DONE = -1, // returned on EOF
  ERROR = 0,
  SYMBOL, NUMBER, CELL, NIL, PRIMITIVE, SPCFORM, FUNCTION, MACRO,
  STRING, STREAM,
  THUNK, // used to implement tail recursion (not lazy eval, currently)
  LIBRARY, // dynamically loaded binary libraries
  UNSPECIFIED, // i.e. unknown type
} obj_type;

struct cell;
struct obj {
  obj_type type;
  union {
    void* data; // @heap
    intptr_t number;
    char* string;
    struct cell* cell;
  };
}; // @heap

struct cell {
  struct obj* first, * rest;
}; // @heap

struct thunk {
  struct env* env;
  struct obj* obj;
}; // @heap

struct function {
  struct obj* body;
  struct obj* params;
  struct env* parent;
}; // @heap

/**
 * Create on the heap a cell with a first and second argument.
 */
struct cell* make_cell(struct obj* first, struct obj* rest);

/**
 * Create on the heap an object with the given type and data.
 *
 * The object's data should point to somewhere on the heap unless otherwise
 * guaranteed to be safe.
 *
 * This is implemented as a macro to cast the second argument to a void pointer
 * always before calling make_object_from_ptr.
 */
#define make_object(type, data) (make_object_from_ptr((type), (void*) (data)))

/**
 * See make_object(type, data).
 */
struct obj* make_object_from_ptr(obj_type type, void* data);

/**
 * Convenience function to create an error object, copying the given argument
 * to memory given by malloc().
 */
struct obj* make_error(char* error_message);

#endif
