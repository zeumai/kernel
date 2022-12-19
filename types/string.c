#include "string.h"
#include <string.h>

Object *string(char *text) {
    size_t length = strlen(text);
    String *s = (String *)object(STRING, (sizeof *s) + length + 1);
    s->length = length;
    memcpy(s->text, text, length);
    s->text[length] = '\0';
    return (Object *)s;
}
