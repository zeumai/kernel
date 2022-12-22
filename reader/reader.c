#include "reader.h"

bool is_blank(int c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

int skip_blanks(FILE *in) {
    int c;
    for (c = fgetc(in); is_blank(c); c = fgetc(in));
    return c;
}

Object *read_symbol(FILE *in) {
    char *text = NULL;
    size_t capacity = 0, i = 0;
    for(int c = fgetc(in);; c = fgetc(in)) {
        if (i == capacity) {
            capacity = (capacity == 0 ? 8 : capacity * 2);
            text = realloc(text, capacity);
            if (text == NULL) {
                fputs("Reader: Allocation failure\nAborting\n", stderr);
                exit(EXIT_FAILURE);
            }
        }
        if (is_blank(c) || c == EOF || c == '(' || c == ')' || c == '.') {
            ungetc(c, in);
            text[i] = '\0';
            Object *s = symbol(text);
            free(text);
            return s;            
        } else {
            text[i++] = c;
        }
    }
}

Object *read_list(FILE *in) {
    Object *list = NULL;
    Pair *cursor = NULL;
    for (int c = skip_blanks(in); c != EOF; c = skip_blanks(in)) {
        if (c == ')') {
            return list;
        } else if (c == '.') {
            if (cursor == NULL) {
                fputs("Reader: Unexpected '.'\n", stderr);
                return error();
            }
            if (is_error(cursor->cdr = read(in))) {
                return cursor->cdr;
            }
            if (skip_blanks(in) != ')') {
                break;                
            }
            return list;
        } else {
            ungetc(c, in);
            cursor = (cursor == NULL) ?
                (Pair *)(list = pair(read(in), NULL)) :
                (Pair *)(cursor->cdr = pair(read(in), NULL));  
            if (is_error(cursor->car)) {
                return cursor->car;
            }
        }
    }
    fputs("Reader: Unterminated list: ", stderr);
    write(list, stderr);
    fputs("\n", stderr);
    return error();
}

Object *read_string(FILE *in) {
    char *text = NULL;
    size_t capacity = 0, i = 0;
    for(int c = fgetc(in); c != EOF; c = fgetc(in)) {
        if (i == capacity) {
            capacity = (capacity == 0 ? 8 : capacity * 2);
            text = realloc(text, capacity);
            if (text == NULL) {
                fputs("Reader: Allocation failure\nAborting\n", stderr);
                exit(EXIT_FAILURE);
            }
        }
        if (c == '\"') {
            text[i] = '\0';
            Object *s = string(text);
            free(text);
            return s;            
        } else {
            text[i++] = c;
        }
    }
    fputs("Reader: Unterminated string", stderr);
    return error();
}

Object *read_number(FILE *in) {
    int64_t value = 0;
    for(int c = fgetc(in);; c = fgetc(in)) {
        if (is_blank(c) || c == EOF || c == '(' || c == ')' || c == '.') {
            ungetc(c, in);
            return number(value);            
        } else if (c >= '0' && c <= '9') {
            value *= 10;
            value += c - '0';
        } else if (c == ',') {
            continue;
        } else {
            fprintf(stderr, "Reader: Non-numeric character in number literal: '%c'\n", c);
            return error();
        }
    }
}

Object *read_special(FILE *in) {
    static Object *t, *f;
    int c = skip_blanks(in);
    if (c == '<') {
        while ((c = fgetc(in)) != '>');
        fputs("Reader: Unreadable representation\n", stderr);
        return error();
    }
    ungetc(c, in);
    Object *s = read(in);
    if (s == (t ? t : (t = symbol("t")))) {
        return boolean(true);
    } else if (s == (f ? f : (f = symbol("f")))) {
        return boolean(false);
    }
    fputs("Reader: Not a special dispatch symbol: ", stderr);
    write(s, stderr);
    fputs("\n", stderr);
    return error();
}

Object *read(FILE *in) {
    int c = skip_blanks(in);
    if (c == EOF) {
        quit = true;
        return NULL;
    } else if (c == '(') {
        return read_list(in);
    } else if (c == ')') {
        fputs("Reader: Unexpected ')'\n", stderr);
        return error();
    } else if (c == '.') {
        fputs("Reader: Unexpected '.'\n", stderr);
        return error();
    } else if (c == '\"') {
        return read_string(in);
    } else if (c == '#') {
        return read_special(in);
    } else if (c >= '0' && c <= '9') {
        ungetc(c, in);
        return read_number(in);
    } else {
        ungetc(c, in);
        return read_symbol(in);
    }
}
