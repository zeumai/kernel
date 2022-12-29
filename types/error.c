#include "object.h"
#include <stdarg.h>

Object *error(char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    while (*fmt != '\0') {
        if (*fmt == '%') {
            Object *obj = va_arg(argp, Object *);
            write(obj, stderr);
        } else {
            fputc(*fmt, stderr);
        }
        ++fmt;
    }
    va_end(argp);
    fputs("\n", stderr);
    static Error *e;
    return (Object *)(e ? e : (e = (Error *)object(ERROR, sizeof *e)));
}

bool is_error(Object *obj) {
    return (obj != NULL) && (obj->type == ERROR);
}
