#ifndef STRING_H
#define STRING_H

#include "object.h"

typedef struct String {
    Object obj;
    size_t length;
    char text[];
} String;

Object *string(char *text);

#endif
