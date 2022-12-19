#include "closure.h"

Object *closure(Environment *e, Object *pars, Object *body) {
    Closure *c = (Closure *)object(CLOSURE, sizeof *c);
    c->env = e;
    c->pars = pars;
    c->body = body;
    return (Object *)c;
}
