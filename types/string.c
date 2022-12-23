#include "object.h"
#include <string.h>

Object *string(char *text) {
    size_t length = strlen(text);
    String *s = (String *)object(STRING, (sizeof *s) + length + 1);
    s->length = length;
    memcpy(s->text, text, length);
    s->text[length] = '\0';
    return (Object *)s;
}

Object *string_concat(char *txt1, char *txt2) {
    size_t len1 = strlen(txt1), len2 = strlen(txt2);
    String *s = (String *)object(STRING, (sizeof *s) + len1 + len2 + 1);
    s->length = len1 + len2;
    memcpy(s->text, txt1, len1);
    memcpy(s->text + len1, txt2, len2);
    s->text[len1 + len2] = '\0';
    return (Object *)s;
}

Object *string_substring(char *text, size_t start, size_t length) {
    String *s = (String *)object(STRING, (sizeof *s) + length + 1);
    s->length = length;
    memcpy(s->text, text + start - 1, length);
    s->text[length] = '\0';
    return (Object *)s;
}
