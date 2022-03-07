#include "hcc.h"

Type *new_type(TypeKind kind) {
    Type *typ = calloc(1, sizeof(Type));
    typ->kind = kind;
    switch (kind) {
    case TP_INT:
        typ->size = 4;
        return typ;
    case TP_CHAR:
        typ->size = 1;
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

// type_cmp checks when 'typ1 = typ2' is valid.
// if it isn't valid, raise error else return assign type;
Type *can_assign(Type *typ1, Type *typ2) {
    switch (typ1->kind) {
    case TP_INT:
        switch (typ2->kind) {
        case TP_INT:
        case TP_CHAR:
            return new_type(TP_INT);
        case TP_PTR:
        case TP_ARRAY:
        default:
            error_at(token->str, "cannot assign here.");
        }
    case TP_CHAR:
        switch (typ2->kind) {
        case TP_INT:
        case TP_CHAR:
            return new_type(TP_CHAR);
        case TP_PTR:
        case TP_ARRAY:
        default:
            error_at(token->str, "cannot assign here.");
        }
    case TP_PTR:
        switch (typ2->kind) {
        case TP_INT:
        case TP_CHAR:
            error_at(token->str, "cannot assign here.");
        case TP_PTR:
            return typ1;
        case TP_ARRAY:
        default:
            error_at(token->str, "cannot assign here.");
        }
    case TP_ARRAY:
    default:
        error_at(token->str, "cannot assign here.");
    }
}

// can_add checks if "typ1 + typ2" is valid. it is valid when either of type is not pointer,
// return type after addition.
Type *can_add(Type *typ1, Type *typ2) {
    switch (typ1->kind) {
    case TP_INT:
        switch (typ2->kind) {
        case TP_INT:
            return new_type(TP_INT);
        case TP_CHAR:
            return new_type(TP_INT);
        case TP_PTR:
            return typ2;
        case TP_ARRAY:
            return typ2;
        default:
            error_at(token->str, "type isn't valid.");
        }
    case TP_CHAR:
        switch (typ2->kind) {
        case TP_INT:
            return new_type(TP_INT);
        case TP_CHAR:
            return new_type(TP_CHAR);
        case TP_PTR:
            return typ2;
        case TP_ARRAY:
            return typ2;
        default:
            error_at(token->str, "type isn't valid.");
        }
    case TP_PTR:
        switch (typ2->kind) {
        case TP_INT:
            return typ1;
        case TP_CHAR:
            return typ1;
        case TP_PTR:
        case TP_ARRAY:
        default:
            error_at(token->str, "type isn't valid.");
        }
    case TP_ARRAY:
        switch (typ2->kind) {
        case TP_INT:
            return typ1;
        case TP_CHAR:
            return typ1;
        case TP_PTR:
        case TP_ARRAY:
        default:
            error_at(token->str, "type isn't valid.");
        }
    default:
        error_at(token->str, "type isn't valid.");
    }
}

bool is_ptr(Type *typ) {
    return typ->kind == TP_ARRAY || typ->kind == TP_PTR;
}

void register_func(Node *node) {
    if (node->kind != ND_FUNCDEF && node->kind != ND_FUNCDECL) {
        errorf("cannot register func when node->kind is neither FUNCDEF nor FUNCDECL.");
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
// otherwise return NULL
LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            return var;
        }
    }
    return NULL;
}

// find_gvar searches local variables,if exists return the global variable
// otherwise return NULL
GVar *find_gvar(Token *tok) {
    for (GVar *var = globals; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            return var;
        }
    }
    return NULL;
}