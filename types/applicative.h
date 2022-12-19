#ifndef APPLICATIVE_H
#define APPLICATIVE_H

#include "object.h"

typedef struct Applicative {
    Object obj;
    Object *fn;
} Applicative;

Object *applicative(Object *fn);

#endif
