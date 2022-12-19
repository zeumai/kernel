#include "primitive.h"

Object *primitive(Object *(*function)(Object *, Environment *)) {
    Primitive *p = (Primitive *)object(PRIMITIVE, sizeof *p);
    p->function = function;
    return (Object *)p;
}
