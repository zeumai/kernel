#include "object.h"

Object *end(void) {
    End *e = (End *)object(END, sizeof *e);
    return (End *)e;
}
