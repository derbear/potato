// ********** Derek Leung
// ********** The POTATO interpreter (until I think of a better name)

#include <stdio.h>

#include "globals.h"
#include "builtins.h"
#include "eval.h"
#include "io.h"

int main(int argc, char* argv[]) {
  initialize();

  if (argc == 2) {
    FILE* f = fopen(argv[1], "r");
    if (!f) {
      printf("ERROR: cannot open file \"%s\"\n", argv[1]);
      return 1;
    }
    fclose(f);
    
    struct obj* str = make_object(LITERAL, argv[1]);
    struct obj* arg = make_object(CELL, make_cell(str, make_object(NIL, 0)));
    struct obj* result = load(arg);
    if (result->type == NIL) {
      return 0;
    }
    print_obj(result); // error
    printf("\n");
    return 1;
  }

  struct obj* next;
  printf("POTATO v.0.1 INTERACTIVE REPL \n");
  printf("pot> ");
  while ((next = evaluate(next_object(stdin_reader)))->type != DONE) {
    print_obj(next);
    printf("\npot> ");
  }
  printf("\n*** Reached end of input ***\n");
  return 0;
}
