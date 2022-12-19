#ifndef CLOSURE_H
#define CLOSURE_H

#include "object.h"
#include "environment.h"

typedef struct Closure {
    Object obj;
    Environment *env;
    Object *pars;
    Object *body;
} Closure;

Object *closure(Environment *e, Object *pars, Object *body);

#endif
