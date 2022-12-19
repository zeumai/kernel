#include "symbol.h"
#include <string.h>

static Symbol **slots;
static size_t used, capacity;

static bool is_lowercase(char c) {
    return c >= 'a' && c <= 'z';
}

static char to_uppercase(char c) {
    return c + 'A' - 'a';
}

static size_t hash(char *text) {
    size_t hash = 5381;
    for (int c = *text; (c = *text) != '\0'; ++text) {
        hash = ((hash << 5) + hash) + (is_lowercase(c) ? to_uppercase(c) : c);
    }
    return hash;
}

static bool symbol_text_eq(Symbol *s, char *text, size_t length) {
    if (s->length != length) return false;
    for (size_t i = 0; i < length; i++) {
        char c = is_lowercase(text[i]) ? to_uppercase(text[i]) : text[i];
        if (s->text[i] != c) return false;
    }
    return true;
}

static Symbol **get_slot(char *text, size_t length) {
    size_t i = hash(text) % capacity;
    while (slots[i] != NULL && !symbol_text_eq(slots[i], text, length)) {
        i = (i + 1) % capacity;
    }
    return slots + i;
}

static void resize(size_t new_capacity) {
    Symbol **old_slots = slots;
    size_t old_capacity = capacity;
    slots = calloc(new_capacity, sizeof *slots);
    capacity = new_capacity;
    if (slots == NULL) {
        fputs("Allocation failure\n", stderr);
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < old_capacity; i++) {
        if (old_slots[i] != NULL) {
            Symbol **slot = get_slot(old_slots[i]->text, old_slots[i]->length);
            *slot = old_slots[i];
        }
    }
    free(old_slots);
}

static Symbol *errant_symbol(char *text, size_t length) {
    Symbol *s = (Symbol *)object(SYMBOL, (sizeof *s) + length + 1);
    s->length = length;
    memcpy(s->text, text, length);
    s->text[length] = '\0';
    for (size_t i = 0; i < length; ++i) {
        if (is_lowercase(s->text[i])) {
            s->text[i] = to_uppercase(s->text[i]);
        }
    }
    return s;
}

Object *symbol(char *text) {
    size_t length = strlen(text);
    if (capacity == 0) resize(16);
    Symbol **slot = get_slot(text, length);
    if (*slot == NULL) {
        if (++used > capacity / 2) {
            resize(capacity * 2);
            slot = get_slot(text, length);
        }
        *slot = errant_symbol(text, length);
    }
    return (Object *)*slot;
}
