#include "hcc.h"

Type *new_type(TypeKind kind) {
    Type *typ = calloc(1, sizeof(Type));
    typ->kind = kind;
    return typ;
}

bool type_cmp(Type *typ1, Type *typ2) {
    if (typ1->kind != typ2->kind) {
        return false;
    }
    if (typ1->kind != TP_PTR) {
        return true;
    }
    return type_cmp(typ1->ptr_to, typ2->ptr_to);
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