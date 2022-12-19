#ifndef ERROR_H
#define ERROR_H

#include "object.h"

typedef struct Error {
    Object obj;
} Error;

Object *error(void);

bool is_error(Object *obj);

#endif
