#include "reader/reader.h"
#include "evaluator/evaluator.h"

int main(int argc, char *argv[]) {
    Environment *e = (Environment *)environment(NULL);
    dict_set(e->bindings, (Symbol *)symbol("define"), primitive(&prm_define));
    dict_set(e->bindings, (Symbol *)symbol("if"), primitive(&prm_if));
    dict_set(e->bindings, (Symbol *)symbol("lambda"), primitive(&prm_lambda));
    dict_set(e->bindings, (Symbol *)symbol("quote"), primitive(&prm_quote));
    dict_set(e->bindings, (Symbol *)symbol("eq?"), primitive(&prm_eq));
    dict_set(e->bindings, (Symbol *)symbol("cons"), primitive(&prm_cons));
    dict_set(e->bindings, (Symbol *)symbol("next"), primitive(&prm_next));
    Object *root = pair((Object *)e, NULL);
    for (;;) {
        fputs("> ", stdout);
        write(eval(read(stdin), e), stdout);
        fputs("\n", stdout);
        collect(root);
    }
    return 0;
}
