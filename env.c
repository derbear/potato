#include "env.h"

#include <stdlib.h>
#include <stdio.h>

#include "util.h"
#include "globals.h"

typedef struct {
  char* name; // @heap
  struct obj* object;
} s_entry;

struct env {
  struct env* parent;
  s_entry* list; // @heap
  int next_free;
  int size;
}; // @heap

s_entry make_entry(char* name, struct obj* object) {
  s_entry entry;
  entry.name = name;
  entry.object = object;
  return entry;
}

struct env* make_env(struct env* parent, int size) {
  if (size == 0) {
    printf("ERROR: TRIED TO MAKE ENVIRONMENT OF ZERO SIZE\n");
    exit(1);
  }
  struct env* env = malloc(sizeof(struct env));
  env->parent = parent;
  env->list = malloc(sizeof(s_entry) * size);
  env->next_free = 0;
  env->size = size;
  return env;
}

void expand_env(struct env* env) {
  env->size *= 2;
  env->list = realloc(env->list, sizeof(s_entry) * env->size);
}

int bind(struct env* env, char* name, struct obj* obj) {
  if (env->next_free == env->size) {
    expand_env(env);
  }

  lookup_local(env, name);
  if (lookup_local(env, name)->type != ERROR) {
    return 0;
  }
  env->list[(env->next_free)++] = make_entry(name, obj);
  return 1;
}

struct obj* lookup_func(struct env* env, char* name, int recursive) {
  int i = 0;
  while (i < env->next_free) {
    s_entry entry = env->list[i];
    if (str_eq(name, entry.name)) {
      return entry.object;
    }
    i++;
  }
  if (recursive && env->parent) {
    return lookup(env->parent, name);
  }
  if (DEBUG) {
    // printf("Looked up: %s", name);
    return make_error(name);
  }
  return make_error("cannot find symbol");
}

struct env* parent(struct env* t) {
  return t->parent;
}
