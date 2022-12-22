#include "reader/reader.h"
#include "evaluator/evaluator.h"

int main(int argc, char *argv[]) {
    Environment *e = (Environment *)environment(NULL);
    dict_set(e->bindings, (Symbol *)symbol("if"), primitive(&p_if));
    dict_set(e->bindings, (Symbol *)symbol("quote"), primitive(&p_quote));
    dict_set(e->bindings, (Symbol *)symbol("quit"), applicative(primitive(&p_quit)));
    dict_set(e->bindings, (Symbol *)symbol("define"), primitive(&p_define));
    dict_set(e->bindings, (Symbol *)symbol("cons"), applicative(primitive(&p_cons)));
    dict_set(e->bindings, (Symbol *)symbol("lambda"), primitive(&p_lambda));
    dict_set(e->bindings, (Symbol *)symbol("eq?"), applicative(primitive(&p_eq)));
    dict_set(e->bindings, (Symbol *)symbol("null?"), applicative(primitive(&p_null)));
    dict_set(e->bindings, (Symbol *)symbol("pair?"), applicative(primitive(&p_pair)));
    dict_set(e->bindings, (Symbol *)symbol("symbol?"), applicative(primitive(&p_symbol)));
    dict_set(e->bindings, (Symbol *)symbol("operative?"), applicative(primitive(&p_operative)));
    dict_set(e->bindings, (Symbol *)symbol("applicative?"), applicative(primitive(&p_applicative)));
    dict_set(e->bindings, (Symbol *)symbol("boolean?"), applicative(primitive(&p_boolean)));
    dict_set(e->bindings, (Symbol *)symbol("dict?"), applicative(primitive(&p_dict)));
    dict_set(e->bindings, (Symbol *)symbol("environment?"), applicative(primitive(&p_environment)));
    dict_set(e->bindings, (Symbol *)symbol("number?"), applicative(primitive(&p_number)));
    dict_set(e->bindings, (Symbol *)symbol("string?"), applicative(primitive(&p_string)));
    for (;;) {
        fputs("> ", stdout);
        Object *result = eval(read(stdin), e);
        if (quit) break;
        write(result, stdout);
        fputs("\n", stdout);
    }
    return 0;
}
