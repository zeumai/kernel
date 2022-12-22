#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

extern bool quit;

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

Object *object(Type type, size_t size);
void write(Object *obj, FILE *out);

typedef struct Symbol {
    Object obj;
    size_t length;
    char text[];
} Symbol;

Object *symbol(char *text);

typedef struct Applicative {
    Object obj;
    Object *fn;
} Applicative;

Object *applicative(Object *fn);

typedef struct Boolean {
    Object obj;
    bool value;
} Boolean;

Object *boolean(bool value);

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

typedef struct Environment {
    Object obj;
    Dict *bindings;
    struct Environment *parent;
} Environment;

Object *environment(Environment *parent);

typedef struct Closure {
    Object obj;
    Environment *env;
    Object *pars;
    Object *body;
} Closure;

Object *closure(Environment *e, Object *pars, Object *body);

typedef struct Error {
    Object obj;
} Error;

Object *error(void);

bool is_error(Object *obj);

typedef struct Number {
    Object obj;
    int64_t value;
} Number;

Object *number(int64_t value);

typedef struct Pair {
    Object obj;
    Object *car;
    Object *cdr;
} Pair;

Object *pair(Object *car, Object *cdr);

typedef struct Primitive {
    Object obj;
    Object *(*function)(Object *, Environment *);
} Primitive;

Object *primitive(Object *(*function)(Object *, Environment *));

typedef struct String {
    Object obj;
    size_t length;
    char text[];
} String;

Object *string(char *text);

#endif
