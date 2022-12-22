#include "object.h"

Object *boolean(bool value) {
    static Boolean *T, *F;
    if (value) {
        if (T == NULL) {
            T = (Boolean *)object(BOOLEAN, sizeof *T);
            T->value = true;
        }
        return (Object *)T;
    } else {
        if (F == NULL) {
            F = (Boolean *)object(BOOLEAN, sizeof *F);
            F->value = false;
        }
        return (Object *)F;
    }
}
