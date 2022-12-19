#ifndef SYMBOL_H
#define SYMBOL_H

#include "object.h"

typedef struct Symbol {
    Object obj;
    size_t length;
    char text[];
} Symbol;

Object *symbol(char *text);

#endif
