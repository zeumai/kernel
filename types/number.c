#ifndef NUMBER_H
#define NUMBER_H

#include "number.h"

Object *number(int64_t value) {
    Number *n = (Number *)object(NUMBER, sizeof *n);
    n->value = value;
    return (Object *)n;
}

#endif
