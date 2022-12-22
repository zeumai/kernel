#include "object.h"

Object *environment(Environment *parent) {
    Environment *e = (Environment *)object(ENVIRONMENT, sizeof *e);
    e->bindings = (Dict *)dict();
    e->parent = parent;
    return (Object *)e;
}
