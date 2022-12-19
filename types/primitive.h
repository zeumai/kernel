#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "object.h"
#include "environment.h"

typedef struct Primitive {
    Object obj;
    Object *(*function)(Object *, Environment *);
} Primitive;

Object *primitive(Object *(*function)(Object *, Environment *));

#endif
