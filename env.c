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
  struct env* env = malloc(sizeof(struct env));
  env->parent = parent;
  env->list = malloc(sizeof(s_entry) * size);
  env->next_free = 0;
  env->size = size;
  return env;
}

void bind(struct env* env, char* name, struct obj* obj) {
  if (env->next_free == env->size) {
    printf("ERROR: ENV FULL\n");
    exit(1);
  }
  env->list[(env->next_free)++] = make_entry(name, obj);
}

#define lookup(x,y) lookup_helper((x),(y),1)
#define lookup_local(x,y) lookup_helper((x),(y),0)

struct obj* lookup_helper(struct env* env, char* name, int recursive) {
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
    printf("Looked up: %s", name);
  }
  return make_error("cannot find symbol");
}

struct env* parent(struct env* t) {
  return t->parent;
}
