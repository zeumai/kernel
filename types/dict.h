#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "object.h"
#include "symbol.h"

typedef struct Record {
    Symbol *key;
    Object *value;
} Record;

typedef struct Dict {
    Object obj;
    size_t capacity, used;
    Record *records;
} Dict;

Object *dict(void);

void dict_set(Dict *d, Symbol *key, Object *value);
Object **dict_get(Dict *d, Symbol *key);

#endif
