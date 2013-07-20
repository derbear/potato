// ********** Derek Leung
// ********** POTATO evaluation

#ifndef EVAL_H_INCLUDED
#define EVAL_H_INCLUDED

#include "data.h"

struct table;
struct table* make_table(struct table* parent, int size);

struct obj* list(struct obj* operand); // this doubles as a shortcut for evaluating all operands
struct obj* apply(struct obj* operator, struct obj* operand);
struct obj* evaluate(struct obj* obj);

int list_len(struct obj* obj); // utility function for finding length of list
			       // if not list, returns -1
void bind(struct table* table, char* name, struct obj* obj);
void print_obj(struct obj* obj);


#endif
