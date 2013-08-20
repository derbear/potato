#ifndef ENV_H
#define ENV_H

#include "data.h"

/**
 * Lookup environments which map strings to objects and have a pointer to parent
 * lookup environments (which is NULL if it doesn't have one).
 */
struct env;

/**
 * Create a lookup environment with a given parent and a given initial size.
 */
struct env* make_env(struct env* parent, int size);

/**
 * Binds the given name to an object in the given lookup environment unless it
 * is already bound.
 *
 * Returns 1 on success and 0 on failure.
 */
int bind(struct env*, char* name, struct obj* obj);

/**
 * Find the parent of some environment.
 */
struct env* parent(struct env*);

/**
 * Standard recursive name lookup.
 */
#define lookup(env,name) lookup_func((env),(name),1)

/**
 * Look up a name only in the current scope.
 */
#define lookup_local(env,name) lookup_func((env),(name),0)

struct obj* lookup_func(struct env* env, char* name, int recursive);


#endif
