#include "object.h"

typedef struct Number {
    Object obj;
    int64_t value;
} Number;

Object *number(int64_t value);
