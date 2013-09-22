// ********** Derek Leung
// ********** Shared data definitions

#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

struct env; // forward declaration

typedef enum {
  DONE = -1, // returned on EOF
  ERROR = 0,
  SYMBOL, NUMBER, CELL, NIL, PRIMITIVE, FUNCTION, MACRO,
  LITERAL, STREAM,
  THUNK, // used to implement tail recursion
  LIBRARY, // dynamically loaded binary libraries
  EXTENSION, // for extensions
} obj_type;

struct cell;
struct obj {
  obj_type type;
  union {
    void* data; // @heap
    int number; // TODO use macros to limit size of this to pointer size
    char* string;
    struct cell* cell;
  };
}; // @heap

struct cell {
  struct obj* first, * rest;
}; // @heap

struct primitive {
  struct obj* (*c_func)(struct obj*, struct env*);
  char* name;
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
 */
struct obj* make_object(obj_type type, void* data);

/**
 * Same as make_object(), but accepts a numerical data-type.
 *
 * Intended for use on things smaller than a pointer.
 */
struct obj* make_small_object(obj_type type, int data); // TODO make macro

/**
 * Convenience function to create an error object, copying the given argument
 * to memory given by malloc().
 */
struct obj* make_error(char* error_message);

#endif
