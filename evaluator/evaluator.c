#include "evaluator.h"
#include <string.h>

bool tail_call = false;

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
        cursor = &(((Pair *)*cursor)->cdr);
    }
    return head;
}

// primitives

Object *p_if(Object *args, Environment *e) {
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
        ret = next_arg(&args, false);
        if (is_error(ret)) return ret;
        tmp = next_arg(&args, true);
        if (is_error(tmp)) return tmp;
    } else {
        tmp = next_arg(&args, false);
        if (is_error(tmp)) return tmp;
        ret = next_arg(&args, true);
    }
    tail_call = true;
    return ret;
}

Object *p_quote(Object *args, Environment *e) {
    return next_arg(&args, true);
}

Object *p_quit(Object *args, Environment *e) {
    quit = true;
    return NULL;
}

Object *p_define(Object *args, Environment *e) {
    Object *names = next_arg(&args, false);
    if (is_error(names)) return names;
    Object *values = eval(next_arg(&args, true), e);
    if (is_error(values)) return values;
    return bind(e, names, values);
}

Object *p_cons(Object *args, Environment *e) {
    Object *car = next_arg(&args, false);
    if (is_error(car)) return car;
    Object *cdr = next_arg(&args, true);
    if (is_error(cdr)) return cdr;
    return pair(car, cdr);
}


Object *p_lambda(Object *args, Environment *e) {
    Object *pars = next_arg(&args, false);
    if (is_error(pars)) return pars;
    Object *body = next_arg(&args, true);
    if (is_error(body)) return body;
    return applicative(closure(e, pars, body));
}

Object *p_throw(Object *args, Environment *e) {
    Object *msg = next_arg(&args, true);
    if (is_error(msg)) return msg;
    write(msg, stderr);
    fputs("\n", stderr);
    return error();
}

Object *p_eq(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    return boolean(left == right);
}

Object *p_null(Object *args, Environment *e) {
    Object *obj = next_arg(&args, true);
    if (is_error(obj)) return obj;
    return boolean(obj == NULL);
}

Object *p_pair(Object *args, Environment *e) {
    Object *obj = next_arg(&args, true);
    if (is_error(obj)) return obj;
    return boolean(obj != NULL && obj->type == PAIR);
}

Object *p_symbol(Object *args, Environment *e) {
    Object *obj = next_arg(&args, true);
    if (is_error(obj)) return obj;
    return boolean(obj != NULL && obj->type == SYMBOL);
}

Object *p_operative(Object *args, Environment *e) {
    Object *obj = next_arg(&args, true);
    if (is_error(obj)) return obj;
    return boolean(obj != NULL && (obj->type == PRIMITIVE || obj->type == CLOSURE));
}

Object *p_applicative(Object *args, Environment *e) {
    Object *obj = next_arg(&args, true);
    if (is_error(obj)) return obj;
    return boolean(obj != NULL && obj->type == APPLICATIVE);
}

Object *p_boolean(Object *args, Environment *e) {
    Object *obj = next_arg(&args, true);
    if (is_error(obj)) return obj;
    return boolean(obj != NULL && obj->type == BOOLEAN);
}

Object *p_dict(Object *args, Environment *e) {
    Object *obj = next_arg(&args, true);
    if (is_error(obj)) return obj;
    return boolean(obj != NULL && obj->type == DICT);
}

Object *p_environment(Object *args, Environment *e) {
    Object *obj = next_arg(&args, true);
    if (is_error(obj)) return obj;
    return boolean(obj != NULL && obj->type == ENVIRONMENT);
}

Object *p_number(Object *args, Environment *e) {
    Object *obj = next_arg(&args, true);
    if (is_error(obj)) return obj;
    return boolean(obj != NULL && obj->type == NUMBER);
}

Object *p_string(Object *args, Environment *e) {
    Object *obj = next_arg(&args, true);
    if (is_error(obj)) return obj;
    return boolean(obj != NULL && obj->type == STRING);
}

Object *p_plus(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(left, stderr);
        fputs("\n", stderr);
        return error();
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(right, stderr);
        fputs("\n", stderr);
        return error();
    }
    return number(((Number *)left)->value + ((Number *)right)->value);
}

Object *p_minus(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(left, stderr);
        fputs("\n", stderr);
        return error();
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(right, stderr);
        fputs("\n", stderr);
        return error();
    }
    return number(((Number *)left)->value - ((Number *)right)->value);
}

Object *p_times(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(left, stderr);
        fputs("\n", stderr);
        return error();
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(right, stderr);
        fputs("\n", stderr);
        return error();
    }
    return number(((Number *)left)->value * ((Number *)right)->value);
}

Object *p_num_eq(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(left, stderr);
        fputs("\n", stderr);
        return error();
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(right, stderr);
        fputs("\n", stderr);
        return error();
    }
    return boolean(((Number *)left)->value == ((Number *)right)->value);
}

Object *p_num_less(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(left, stderr);
        fputs("\n", stderr);
        return error();
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(right, stderr);
        fputs("\n", stderr);
        return error();
    }
    return boolean(((Number *)left)->value < ((Number *)right)->value);
}

Object *p_num_leq(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(left, stderr);
        fputs("\n", stderr);
        return error();
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(right, stderr);
        fputs("\n", stderr);
        return error();
    }
    return boolean(((Number *)left)->value <= ((Number *)right)->value);
}

Object *p_num_greater(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(left, stderr);
        fputs("\n", stderr);
        return error();
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(right, stderr);
        fputs("\n", stderr);
        return error();
    }
    return boolean(((Number *)left)->value > ((Number *)right)->value);
}

Object *p_num_geq(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(left, stderr);
        fputs("\n", stderr);
        return error();
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(right, stderr);
        fputs("\n", stderr);
        return error();
    }
    return boolean(((Number *)left)->value >= ((Number *)right)->value);
}

Object *p_concat(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != STRING) {
        fputs("Evaluator: Not a string: ", stderr);
        write(left, stderr);
        fputs("\n", stderr);
        return error();
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != STRING) {
        fputs("Evaluator: Not a string: ", stderr);
        write(right, stderr);
        fputs("\n", stderr);
        return error();
    }
    return string_concat(((String *)left)->text, ((String *)right)->text);
}

Object *p_str_length(Object *args, Environment *e) {
    Object *s = next_arg(&args, true);
    if (is_error(s)) return s;
    if (s == NULL || s->type != STRING) {
        fputs("Evaluator: Not a string: ", stderr);
        write(s, stderr);
        fputs("\n", stderr);
        return error();
    }
    return number(((String *)s)->length);
}

Object *p_substr(Object *args, Environment *e) {
    Object *s = next_arg(&args, false);
    if (is_error(s)) return s;
    if (s == NULL || s->type != STRING) {
        fputs("Evaluator: Not a string: ", stderr);
        write(s, stderr);
        fputs("\n", stderr);
        return error();
    }
    Object *start = next_arg(&args, false);
    if (is_error(start)) return start;
    if (start == NULL || start->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(start, stderr);
        fputs("\n", stderr);
        return error();
    }
    Object *length = next_arg(&args, true);
    if (is_error(length)) return length;
    if (length == NULL || length->type != NUMBER) {
        fputs("Evaluator: Not a number: ", stderr);
        write(length, stderr);
        fputs("\n", stderr);
        return error();
    }
    if ((((Number *)start)->value < 1) 
        || (((Number *)start)->value + ((Number *)length)->value - 1 > ((String *)s)->length)) {
        fputs("Evaluator: Invalid substring:\n   Source string:    ", stderr);
        write(s, stderr);
        fputs("\n   Substring start:  ", stderr);
        write(start, stderr);
        fputs("\n   Substring length: ", stderr);
        write(length, stderr);
        fputs("\n", stderr);
        return error();
    }
    return string_substring(((String *)s)->text, ((Number *)start)->value, ((Number *)length)->value);
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
            obj = ((Primitive *)fn)->function(args, e);
            if (tail_call) {
                tail_call = false;
                goto START_EVAL;
            }
            return obj;
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
