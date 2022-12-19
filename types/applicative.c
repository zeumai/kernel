#include "applicative.h"

Object *applicative(Object *fn) {
    Applicative *a = (Applicative *)object(APPLICATIVE, sizeof *a);
    a->fn = fn;
    return (Object *)a;
}
