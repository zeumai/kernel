#include "object.h"

Object *gc_head = NULL;

Object *object(Type type, size_t size) {
    Object *obj = malloc(size);
    if (obj == NULL) {
        fputs("Object constructor: Allocation failure\nAborting\n", stderr);
        exit(EXIT_FAILURE);
    }
    obj->type = type;
    obj->marked = false;
    obj->next = gc_head;
    gc_head = obj;
    return obj;
}

