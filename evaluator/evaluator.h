#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "../types/object.h"

Object *eval(Object *obj, Environment *e);

// primitives
Object *prm_define(Object *args, Environment *e);
Object *prm_if(Object *args, Environment *e);
Object *prm_lambda(Object *args, Environment *e);
Object *prm_quote(Object *args, Environment *e);
Object *prm_eq(Object *args, Environment *e);
Object *prm_cons(Object *args, Environment *e);
Object *prm_quit(Object *args, Environment *e);

#endif
