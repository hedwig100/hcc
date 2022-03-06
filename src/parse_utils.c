#include "hcc.h"

Type *new_type(TypeKind kind) {
    Type *typ = calloc(1, sizeof(Type));
    typ->kind = kind;
    switch (kind) {
    case TP_INT:
        typ->size = 4;
        return typ;
    case TP_ARRAY:
        return typ;
    default:
        error_at(token->str, "kind isn't valid.");
    }
    return typ;
}

Type *new_type_ptr(Type *ptr_to) {
    Type *typ   = calloc(1, sizeof(Type));
    typ->kind   = TP_PTR;
    typ->size   = 8;
    typ->ptr_to = ptr_to;
    return typ;
}

// type_cmp checks if typ1 equals typ2. note thah array and ptr is regarded as the same type.
bool type_cmp(Type *typ1, Type *typ2) {
    TypeKind kind1 = typ1->kind == TP_ARRAY ? TP_PTR : typ1->kind;
    TypeKind kind2 = typ1->kind == TP_ARRAY ? TP_PTR : typ1->kind;

    if (kind1 != kind2) {
        return false;
    }
    if (kind1 != TP_PTR) {
        return true;
    }
    return type_cmp(typ1->ptr_to, typ2->ptr_to);
}

// can_add checks if "typ1 + typ2" is valid. it is valid when either of type is int for NOW.
Type *can_add(Type *typ1, Type *typ2) {
    if (typ1->kind == TP_INT) {
        return typ2;
    } else if (typ2->kind == TP_INT) {
        return typ1;
    }
    error_at(token->str, "cannot add here.");
}

bool is_ptr(Type *typ) {
    return typ->kind == TP_ARRAY || typ->kind == TP_PTR;
}

void register_func(Node *node) {
    if (node->kind != ND_FUNCDEF) {
        errorf("cannot register func when node->kind is not FUNCDEF");
    }
    Func *now = calloc(1, sizeof(Func));
    now->name = node->name;
    now->len  = node->len;
    now->ret  = node->typ;
    int i     = 0;
    for (Node *param = node->params; param; param = param->next) {
        now->params[i] = param->typ;
        i++;
    }
    now->next = funcs;
    funcs     = now;
}

Func *find_func(Node *node) {
    if (node->kind != ND_CALLFUNC) {
        errorf("cannot register func when node->kind is not FUNCDEF");
    }
    for (Func *fn = funcs; fn; fn = fn->next) {
        if (memcmp(node->name, fn->name, node->len) == 0) {
            return fn;
        }
    }
    error_at(token->str, "function '%s()' isn't defined", to_str(node->name, node->len));
}

// find_lvar searches local variables,if exists return the local variable
// otherwise raise error
LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            return var;
        }
    }
    error_at(tok->str, "local variable isn't defined.");
}

// find_gvar searches local variables,if exists return the local variable
// otherwise return NULL
GVar *find_gvar(Token *tok) {
    for (GVar *var = globals; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            return var;
        }
    }
    return NULL;
}