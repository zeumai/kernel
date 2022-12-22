#include "object.h"

Object *number(int64_t value) {
    Number *n = (Number *)object(NUMBER, sizeof *n);
    n->value = value;
    return (Object *)n;
}
