#include "reader/reader.h"
#include "evaluator/evaluator.h"

int main(int argc, char *argv[]) {
    Environment *e = (Environment *)environment(NULL);
    dict_set(e->bindings, (Symbol *)symbol("define"), primitive(&prm_define));
    dict_set(e->bindings, (Symbol *)symbol("if"), primitive(&prm_if));
    dict_set(e->bindings, (Symbol *)symbol("lambda"), primitive(&prm_lambda));
    dict_set(e->bindings, (Symbol *)symbol("quote"), primitive(&prm_quote));
    dict_set(e->bindings, (Symbol *)symbol("eq?"), applicative(primitive(&prm_eq)));
    dict_set(e->bindings, (Symbol *)symbol("cons"), applicative(primitive(&prm_cons)));
    dict_set(e->bindings, (Symbol *)symbol("quit"), applicative(primitive(&prm_quit)));
    for (;;) {
        fputs("> ", stdout);
        Object *result = eval(read(stdin), e);
        if (quit) break;
        write(result, stdout);
        fputs("\n", stdout);
    }
    return 0;
}
