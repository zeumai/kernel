#ifndef PAIR_H
#define PAIR_H

#include "object.h"

typedef struct Pair {
    Object obj;
    Object *car;
    Object *cdr;
} Pair;

Object *pair(Object *car, Object *cdr);

#endif
