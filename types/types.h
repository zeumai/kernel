#ifndef TYPES_H
#define TYPES_H

#include "pair.h"
#include "symbol.h"
#include "string.h"
#include "number.h"
#include "primitive.h"
#include "error.h"
#include "dict.h"
#include "closure.h"
#include "environment.h"
#include "boolean.h"
#include "applicative.h"

void write(Object *obj, FILE *out);
void collect(Object *root);

#endif
