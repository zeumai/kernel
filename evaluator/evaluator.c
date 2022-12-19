#include "evaluator.h"

static Object *next_arg(Object **args, bool final) {
    if (*args == NULL) {
        fputs("Evaluator: Too few args\n", stderr);
        return error();
    } else if ((*args)->type != PAIR) {
        fputs("Evaluator: Improper arg list\n", stderr);
        return error();
    } else if (final && ((Pair *)(*args))->cdr != NULL) {
        fputs("Evaluator: Too many args\n", stderr);
        return error();
    }
    Object *arg = ((Pair *)(*args))->car;
    *args = ((Pair *)(*args))->cdr;
    return arg;
}

Object *bind(Environment *e, Object *pars, Object *args) {
    while (pars != NULL) {
        if (pars->type == SYMBOL) {
            dict_set(e->bindings, (Symbol *)pars, args);
            return NULL;
        } else if (pars->type == PAIR) {
            if (args == NULL || args->type != PAIR) {
                fputs("Evaluator: Cannot bind parameter tree ", stderr);
                write(pars, stderr);
                fputs(" to argument tree ", stderr);
                write(args, stderr);
                fputs("\n", stderr);
                return error();
            }
            Object *tmp = bind(e, ((Pair *)pars)->car, ((Pair *)args)->car);
            if (is_error(tmp)) return tmp;
            pars = ((Pair *)pars)->cdr;
            args = ((Pair *)args)->cdr;
        } else {
            fputs("Evaluator: Malformed parameter tree: ", stderr);
            write(pars, stderr);
            fputs("\n", stderr);
            return error();
        }
    }
    if (args != NULL) {
        fputs("Evaluator: Unbound argument tree: ", stderr);
        write(args, stderr);
        fputs("\n", stderr);
        return error();
    }
    return NULL;
}

Object *eval_args(Object *args, Environment *e) {
    Object *head = NULL;
    Object **cursor = &head;
    while (args != NULL) {
        Object *tmp = eval(next_arg(&args, false), e);
        if (is_error(tmp)) return tmp;
        *cursor = pair(tmp, NULL);
        cursor = &(((Pair *)cursor)->cdr);
    }
    return head;
}

// primitives

Object *prm_define(Object *args, Environment *e) {
    Object *names = next_arg(&args, false);
    if (is_error(names)) return names;
    Object *values = eval(next_arg(&args, true), e);
    if (is_error(values)) return values;
    return bind(e, names, values);
}

Object *prm_if(Object *args, Environment *e) {
    Object *tmp, *ret;
    tmp = eval(next_arg(&args, false), e);
    if (is_error(tmp)) return tmp;
    if (tmp == NULL || tmp->type != BOOLEAN) {
        fputs("Evaluator: Not a boolean: ", stderr);
        write(tmp, stderr);
        fputs("\n", stderr);
        return error();
    }
    if (((Boolean *)tmp)->value) {
        ret = eval(next_arg(&args, false), e);
        if (is_error(ret)) return ret;
        tmp = next_arg(&args, true);
        if (is_error(tmp)) return tmp;
    } else {
        tmp = next_arg(&args, false);
        if (is_error(tmp)) return tmp;
        ret = eval(next_arg(&args, true), e);
    }
    return ret;
}

Object *prm_lambda(Object *args, Environment *e) {
    Object *pars = next_arg(&args, false);
    if (is_error(pars)) return pars;
    Object *body = next_arg(&args, true);
    if (is_error(body)) return body;
    return applicative(closure(e, pars, body));
}

Object *prm_quote(Object *args, Environment *e) {
    return next_arg(&args, true);
}

Object *prm_eq(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    return boolean(left == right);
}

Object *prm_cons(Object *args, Environment *e) {
    Object *car = eval(next_arg(&args, false), e);
    if (is_error(car)) return car;
    Object *cdr = eval(next_arg(&args, true), e);
    if (is_error(cdr)) return cdr;
    return pair(car, cdr);
}

Object *prm_next(Object *args, Environment *e) {
    Object *obj = next_arg(&args, true); 
    if (obj == NULL) {
        fputs("Evaluator: Not a heap-allocated object: ", stderr);
        write(obj, stderr);
        fputs("\n", stderr);
        return error();
    }
    return obj->next;
}

// core evaluator

static Object *resolve(Symbol *s, Environment *e) {
    while (e != NULL) {
        Object **slot = dict_get(e->bindings, s);
        if (slot != NULL) return *slot;
        e = e->parent;
    }
    fputs("Evaluator: Undefined symbol: ", stderr);
    write((Object *)s, stderr);
    fputs("\n", stderr);
    return error();
}

Object *eval(Object *obj, Environment *e) {
START_EVAL:
    if (obj == NULL) {
        return NULL;
    } else if (obj->type == SYMBOL) {
        return resolve((Symbol *)obj, e); 
    } else if (obj->type == PAIR) {
        Object *fn = eval(((Pair *)obj)->car, e);
        if (is_error(fn)) return fn;
        Object *args = ((Pair *)obj)->cdr;
START_APPLY:
        if (fn == NULL) {
            fputs("Evaluator: () is not a function\n", stderr);
            return error();
        } else if (fn->type == PRIMITIVE) {
            return ((Primitive *)fn)->function(args, e);
        } else if (fn->type == APPLICATIVE) {
            fn = ((Applicative *)fn)->fn;
            args = eval_args(args, e);
            if (is_error(args)) return args;
            goto START_APPLY;
        } else if (fn->type == CLOSURE) {
            obj = ((Closure *)fn)->body;
            e = (Environment *)environment(((Closure *)fn)->env);
            Object *tmp = bind(e, ((Closure *)fn)->pars, args);
            if (is_error(tmp)) return tmp;
            goto START_EVAL;
        } else {
            fputs("Evaluator: ", stderr);
            write(fn, stderr);
            fputs(" is not a function\n", stderr);
            return error();
        }
    } else {
        return obj;
    }
}
