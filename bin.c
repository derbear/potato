// TODO only works on Unix-like systems
// TODO allow initialization function?

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

#include "globals.h"
#include "data.h"
#include "util.h"

struct library {
  int open;
  void* ref;
};

/* always do global loads */
struct obj* bin_load(struct obj* operand, struct env* env) {
  obj_type types[] = {LITERAL, NUMBER};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }

  int mode;
  if (processed[1]->number) {
    mode = RTLD_LAZY | RTLD_GLOBAL;
  } else {
    mode = RTLD_NOW | RTLD_GLOBAL;
  }

  void* lib = dlopen(processed[0]->string, mode);
  if (lib) {
    struct library* wrapper = malloc(sizeof(struct library*));
    wrapper->open = 1;
    wrapper->ref = lib;
    return make_object(LIBRARY, wrapper);
  } else {
    if (DEBUG) {
      printf("DEBUG: %s\n", dlerror());
    }
    return make_object(NIL, 0);
  }
}

struct obj* bin_get(struct obj* operand, struct env* env) {
  obj_type types[] = {LIBRARY, LITERAL};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 2, types);
  if (!processed) {
    return operand;
  }

  struct library* wrapped = processed[0]->data;
  if (!wrapped->open) {
    return make_error("library has been closed already");
  }
  void* primitive = dlsym(wrapped->ref, processed[1]->string);
  if (primitive) {
    return make_object(PRIMITIVE, primitive);
  } else {
    return make_object(NIL, 0);
  }
}

struct obj* bin_close(struct obj* operand, struct env* env) {
  obj_type types[] = {LIBRARY};
  struct obj** processed = prologue(&operand, env, 1, 1, 1, 1, 1, types);
  if (!processed) {
    return operand;
  }

  struct library* wrapped = processed[0]->data;
  if (!wrapped->open) {
    return make_error("library has been closed already");
  }
  if (!dlclose(wrapped->ref)) {
    wrapped->open = 0;
    char* success_symbol = malloc(sizeof(char) * 2);
    success_symbol[0] = 't'; success_symbol[1] = 0;
    return make_object(SYMBOL, success_symbol);
  } else {
    if (DEBUG) {
      printf("DEBUG: %s\n", dlerror());
    }
    return make_object(NIL, 0);
  }
}
