#include "object.h"

void write(Object *obj, FILE *out) {
    if (obj == NULL) {
        fputs("()", out);
    } else {
        switch (obj->type) {
            case PAIR: {
                fputs("(", out);
                for (;;) {
                    Object *car = ((Pair *)obj)->car;
                    Object *cdr = ((Pair *)obj)->cdr;
                    write(car, out);
                    if (cdr == NULL) break;
                    if (cdr->type != PAIR) {
                        fputs(" . ", out);
                        write(cdr, out);
                        break;
                    }
                    fputs(" ", out);
                    obj = cdr;
                }
                fputs(")", out);
                break;
            }
            case SYMBOL: {
                fputs(((Symbol *)obj)->text, out);
                break;
            }
            case STRING: {
                fputs("\"", out);
                fputs(((String *)obj)->text, out);
                fputs("\"", out);
                break;
            }
            case NUMBER: {
                fprintf(out, "%ld", ((Number *)obj)->value);
                break;
            }
            case PRIMITIVE: {
                fputs("#<primitive>", out);
                break;
            }
            case DICT: {
                fputs("#<dictionary>", out);
                break;
            }
            case ENVIRONMENT: {
                fputs("#<environment>", out);
                break;
            }
            case CLOSURE: {
                fputs("#<closure>", out);
                break;
            }
            case ERROR: {
                fputs("#<error>", out);
                break;
            }
            case BOOLEAN: {
                fputs(((Boolean *)obj)->value ? "#t" : "#f", out);
                break;
            }
            case APPLICATIVE: {
                fputs("#<applicative>", out);
                break;
            }
            default: {
                fputs("Writer: Unrecognized type\nAborting\n", stderr);
                exit(EXIT_FAILURE);
            }
        }
    }
}

Object *gc_head = NULL;

static void mark(Object *root) {
    while (root != NULL) {
        if (root->marked) return;   // avoids reference cycles (and unnecessary work)
        root->marked = true;
        switch (root->type) {
            case PAIR: {
                mark(((Pair *)root)->car);
                root = ((Pair *)root)->cdr;
                continue;
            }
            case SYMBOL: {
                return;
            }
            case STRING: {
                return;
            }
            case NUMBER: {
                return;
            }
            case PRIMITIVE: {
                return;
            }
            case DICT: {
                for (size_t i = 0; i < ((Dict *)root)->capacity; ++i) {
                    mark((Object *)(((Dict *)root)->records[i].key));
                    mark(((Dict *)root)->records[i].value);
                }
                return;
            }
            case ENVIRONMENT: {
                mark((Object *)(((Environment *)root)->bindings));
                root = (Object *)(((Environment *)root)->parent);
                continue;
            }
            case CLOSURE: {
                mark((Object *)(((Closure *)root)->env));
                mark(((Closure *)root)->pars);
                root = ((Closure *)root)->body;
                continue;
            }
            case ERROR: {
                return;
            }
            case BOOLEAN: {
                return;
            }
            case APPLICATIVE: {
                root = (Object *)(((Applicative *)root)->fn);
                continue;
            }
            default: {
                fputs("Garbage collector: Unrecognized type\nAborting\n", stderr);
                exit(EXIT_FAILURE);
            }
        }
    }
}

static void sweep(Object **cursor) {
    while (*cursor != NULL) {
        if ((*cursor)->marked || (*cursor)->type == SYMBOL || (*cursor)->type == BOOLEAN || (*cursor)->type == ERROR) {
            (*cursor)->marked = false;
            cursor = &((*cursor)->next);
        } else {
            switch ((*cursor)->type) {
                case DICT: {
                    free(((Dict *)*cursor)->records);
                    break;
                }
                default: {
                    break;
                }
            }
            Object *next = (*cursor)->next;
            free(*cursor);
            *cursor = next;
        }
    }
}

static void collect(Object *head, Object *root) {
    mark(root);
    sweep(&head);
}

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
