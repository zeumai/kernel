#include "object.h"

Object *error(void) {
    static Error *e;
    return (Object *)(e ? e : (e = (Error *)object(ERROR, sizeof *e)));
}

bool is_error(Object *obj) {
    return (obj != NULL) && (obj->type == ERROR);
}
