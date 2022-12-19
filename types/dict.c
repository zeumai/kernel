#include "dict.h"

Object *dict(void) {
    Dict *d = (Dict *)object(DICT, sizeof *d);
    d->capacity = 0;
    d->used = 0;
    d->records = NULL;
    return (Object *)d;
}

static size_t hash(char *text) {
    size_t hash = 5381;
    for (int c = *text; (c = *text) != '\0'; ++text) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

static Record *get_record(Dict *d, Symbol *key) {
    size_t i = hash(key->text) % d->capacity;
    while (d->records[i].key != NULL && d->records[i].key != key) {
        i = (i + 1) % d->capacity;
    }
    return d->records + i;
}

static void resize(Dict *d, size_t new_capacity) {
    Dict old = *d;
    d->capacity = new_capacity;
    d->records = calloc(d->capacity, sizeof *(d->records));
    if (d->records == NULL) {
        fputs("Dictionary: Allocation failure\n", stderr);
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < old.capacity; i++) {
        if (old.records[i].key != NULL) {
            *get_record(d, old.records[i].key) = old.records[i];
        }
    }
    free(old.records);
}

void dict_set(Dict *d, Symbol *key, Object *value) {
    if (d->capacity == 0) resize(d, 16);
    Record *rec = get_record(d, key);
    if (rec->key == NULL) {
        if (++(d->used) > d->capacity / 2) {
            resize(d, d->capacity * 2);
            rec = get_record(d, key);
        }
        rec->key = key;
    }
    rec->value = value;
}

Object **dict_get(Dict *d, Symbol *key) {
    if (d->capacity != 0) {
        Record *rec = get_record(d, key);
        if (rec->key != NULL) return &(rec->value);
    }
    return NULL;
}
