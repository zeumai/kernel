#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum {
    PAIR,
    SYMBOL,
    STRING,
    NUMBER,
    PRIMITIVE,
    ENVIRONMENT,
    CLOSURE,
    ERROR,
    DICT,
    BOOLEAN,
    APPLICATIVE,
} Type;

typedef struct Object {
    Type type;
    struct Object *next;
    bool marked;
} Object;

extern Object *gc_head;

Object *object(Type type, size_t size);

#endif
