#include "evaluator.h"

bool tail_call = false;

static Object *next_arg(Object **args, bool final) {
    if (*args == NULL) {
        return error("Evaluator: Too few args");
    } else if ((*args)->type != PAIR) {
        return error("Evaluator: Improper arg list");
    } else if (final && ((Pair *)(*args))->cdr != NULL) {
        return error("Evaluator: Too many args");
    }
    Object *arg = ((Pair *)(*args))->car;
    *args = ((Pair *)(*args))->cdr;
    return arg;
}

static Object *bind(Environment *e, Object *pars, Object *args) {
    while (pars != NULL) {
        if (pars->type == SYMBOL) {
            dict_set(e->bindings, (Symbol *)pars, args);
            return NULL;
        } else if (pars->type == PAIR) {
            if (args == NULL || args->type != PAIR) {
                return error("Evaluator: Cannot bind parameter tree % to argument tree %", pars, args);
            }
            Object *tmp = bind(e, ((Pair *)pars)->car, ((Pair *)args)->car);
            if (is_error(tmp)) return tmp;
            pars = ((Pair *)pars)->cdr;
            args = ((Pair *)args)->cdr;
        } else {
            return error("Evaluator: Malformed parameter tree: %", pars);
        }
    }
    if (args != NULL) {
        return error("Evaluator: Unbound argument tree: %", args);
    }
    return NULL;
}

static Object *eval_args(Object *args, Environment *e) {
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
        return error("Evaluator: Not a boolean: %", tmp);
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
    return error("%", msg);
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
        return error("Evaluator: Not a number: %", left);
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        return error("Evaluator: Not a number: %", right);
    }
    return number(((Number *)left)->value + ((Number *)right)->value);
}

Object *p_minus(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != NUMBER) {
        return error("Evaluator: Not a number: %", left);
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        return error("Evaluator: Not a number: %", right);
    }
    return number(((Number *)left)->value - ((Number *)right)->value);
}

Object *p_times(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != NUMBER) {
        return error("Evaluator: Not a number: %", left);
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        return error("Evaluator: Not a number: %", right);
    }
    return number(((Number *)left)->value * ((Number *)right)->value);
}

Object *p_num_eq(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != NUMBER) {
        return error("Evaluator: Not a number: %", left);
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        return error("Evaluator: Not a number: %", right);
    }
    return boolean(((Number *)left)->value == ((Number *)right)->value);
}

Object *p_num_less(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != NUMBER) {
        return error("Evaluator: Not a number: %", left);
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        return error("Evaluator: Not a number: %", right);
    }
    return boolean(((Number *)left)->value < ((Number *)right)->value);
}

Object *p_num_leq(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != NUMBER) {
        return error("Evaluator: Not a number: %", left);
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        return error("Evaluator: Not a number: %", right);
    }
    return boolean(((Number *)left)->value <= ((Number *)right)->value);
}

Object *p_num_greater(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != NUMBER) {
        return error("Evaluator: Not a number: %", left);
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        return error("Evaluator: Not a number: %", right);
    }
    return boolean(((Number *)left)->value > ((Number *)right)->value);
}

Object *p_num_geq(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != NUMBER) {
        return error("Evaluator: Not a number: %", left);
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != NUMBER) {
        return error("Evaluator: Not a number: %", right);
    }
    return boolean(((Number *)left)->value >= ((Number *)right)->value);
}

Object *p_concat(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != STRING) {
        return error("Evaluator: Not a string: %", left);
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != STRING) {
        return error("Evaluator: Not a string: %", right);
    }
    return string_concat(((String *)left)->text, ((String *)right)->text);
}

Object *p_str_length(Object *args, Environment *e) {
    Object *s = next_arg(&args, true);
    if (is_error(s)) return s;
    if (s == NULL || s->type != STRING) {
        return error("Evaluator: Not a string: %", s);
    }
    return number(((String *)s)->length);
}

Object *p_substr(Object *args, Environment *e) {
    Object *s = next_arg(&args, false);
    if (is_error(s)) return s;
    if (s == NULL || s->type != STRING) {
        return error("Evaluator: Not a string: %", s);
    }
    Object *start = next_arg(&args, false);
    if (is_error(start)) return start;
    if (start == NULL || start->type != NUMBER) {
        return error("Evaluator: Not a number: %", start);
    }
    Object *length = next_arg(&args, true);
    if (is_error(length)) return length;
    if (length == NULL || length->type != NUMBER) {
        return error("Evaluator: Not a number: %", length);
    }
    if ((((Number *)start)->value < 1) 
        || (((Number *)start)->value + ((Number *)length)->value - 1 > ((String *)s)->length)) {
        return error(
            "Evaluator: Invalid substring:\n"
            "   Source string:      %\n"
            "   Substring start:    %\n"
            "   Substring length:   %",
            s, start, length
        );
    }
    return string_substring(((String *)s)->text, ((Number *)start)->value, ((Number *)length)->value);
}

Object *p_str_eq(Object *args, Environment *e) {
    Object *left = next_arg(&args, false);
    if (left != NULL && left->type == ERROR) return left;
    if (left == NULL || left->type != STRING) {
        return error("Evaluator: Not a string: %", left);
    }
    Object *right = next_arg(&args, true);
    if (right != NULL && right->type == ERROR) return right;
    if (right == NULL || right->type != STRING) {
        return error("Evaluator: Not a string: %", right);;
    }
    size_t length = ((String *)left)->length;
    if (length != ((String *)right)->length) {
        return boolean(false);
    } 
    char *txt1 = ((String *)left)->text;
    char *txt2 = ((String *)right)->text;
    for (size_t i = 0; i < length; ++i) {
        if (txt1[i] != txt2[i]) {
            return boolean(false);
        }
    }
    return boolean(true);
}

// core evaluator

static Object *resolve(Symbol *s, Environment *e) {
    while (e != NULL) {
        Object **slot = dict_get(e->bindings, s);
        if (slot != NULL) return *slot;
        e = e->parent;
    }
    return error("Evaluator: Undefined symbol: %", (Object *)s);
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
            return error("Evaluator: % is not a function", fn);
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
            return error("Evaluator: % is not a function", fn);
        }
    } else {
        return obj;
    }
}
