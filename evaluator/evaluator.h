#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "../types/object.h"

Object *eval(Object *obj, Environment *e);

// primitives

Object *p_if(Object *args, Environment *e);
Object *p_quote(Object *args, Environment *e);
Object *p_throw(Object *args, Environment *e);
Object *p_quit(Object *args, Environment *e);

Object *p_define(Object *args, Environment *e);

Object *p_cons(Object *args, Environment *e);
Object *p_lambda(Object *args, Environment *e);

Object *p_eq(Object *args, Environment *e);
Object *p_null(Object *args, Environment *e);
Object *p_pair(Object *args, Environment *e);
Object *p_symbol(Object *args, Environment *e);
Object *p_operative(Object *args, Environment *e);
Object *p_applicative(Object *args, Environment *e);
Object *p_boolean(Object *args, Environment *e);
Object *p_dict(Object *args, Environment *e);
Object *p_environment(Object *args, Environment *e);
Object *p_number(Object *args, Environment *e);
Object *p_string(Object *args, Environment *e);

Object *p_plus(Object *args, Environment *e);
Object *p_minus(Object *args, Environment *e);
Object *p_times(Object *args, Environment *e);

#endif
