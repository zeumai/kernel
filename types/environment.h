#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "object.h"
#include "dict.h"

typedef struct Environment {
    Object obj;
    Dict *bindings;
    struct Environment *parent;
} Environment;

Object *environment(Environment *parent);

#endif
