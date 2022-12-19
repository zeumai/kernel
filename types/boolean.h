#include "object.h"

typedef struct Boolean {
    Object obj;
    bool value;
} Boolean;

Object *boolean(bool value);
