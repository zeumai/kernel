#include "pair.h"

Object *pair(Object *car, Object *cdr) {
    Pair *p = (Pair *)object(PAIR, sizeof *p);
    p->car = car;
    p->cdr = cdr;
    return (Object *)p;
}
