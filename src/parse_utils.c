#include "hcc.h"

/*
    type
*/

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
    case TP_VOID:
        typ->size = 1;
        return typ;
    case TP_ARRAY:
        return typ;
    case TP_STRUCT:
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

Type *new_type_strct(Token *tok, Member *mem) {
    // register struct
    Struct *strct = calloc(1, sizeof(Struct));
    strct->name   = tok->str;
    strct->len    = tok->len;
    strct->mem    = mem;
    strct->next   = strcts;
    strcts        = strct;

    // type
    Type *typ = calloc(1, sizeof(Type));
    typ->kind = TP_STRUCT;
    typ->name = tok->str;
    typ->len  = tok->len;

    Type head;
    head.next      = NULL;
    Type *cur      = &head;
    int min_offset = 0;
    for (Member *now = mem; now; now = now->next) {
        cur->next = now->typ;
        cur       = cur->next;

        int offset  = calc_aligment_offset(min_offset, byte_align(cur));
        cur->offset = offset;
        now->offset = offset;
        min_offset  = offset + cur->size;
    }

    typ->mem    = head.next;
    typ->size   = min_offset;
    strct->size = min_offset;
    strct->typ  = typ;
    return typ;
}

Type *new_type_enum(Object *en) {
    // register enum
    en->next   = scopes->en;
    scopes->en = en;

    // type
    Type *typ = calloc(1, sizeof(Type));
    typ->kind = TP_ENUM;
    typ->size = 4;
    typ->name = en->name;
    typ->len  = en->len;
    return typ;
}

int byte_align(Type *typ) {
    int aligment;

    switch (typ->kind) {
    case TP_INT:
    case TP_ENUM:
        return 4;
    case TP_CHAR:
        return 1;
    case TP_PTR:
        return 4;
    case TP_ARRAY:
        return byte_align(typ->ptr_to);
    case TP_STRUCT:
        aligment = 0;
        for (Type *now = typ->mem; now; now = now->next) {
            aligment = max(aligment, byte_align(now)); // actually lcm but max is enough
        }
        return aligment;
    case TP_VOID:
    default:
        error_at(token->str, "type isn't valid.");
    }
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
        case TP_VOID:
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
        case TP_VOID:
        case TP_PTR:
        case TP_ARRAY:
        default:
            error_at(token->str, "cannot assign here.");
        }
    case TP_VOID:
        error_at(token->str, "cannot assign here.");
    case TP_PTR:
        switch (typ2->kind) {
        case TP_INT:
        case TP_CHAR:
            error_at(token->str, "cannot assign here.");
        case TP_VOID:
            error_at(token->str, "cannot assign here.");
        case TP_PTR:
            return typ1;
        case TP_ARRAY:
            return typ1;
        default:
            error_at(token->str, "cannot assign here.");
        }
    case TP_ARRAY:
        error_at(token->str, "cannot assign here.");
    case TP_STRUCT:
        if (is_typ(typ2, TP_STRUCT) && typ1->len == typ2->len && !memcmp(typ1->name, typ2->name, typ1->len))
            return typ1;
        error_at(token->str, "cannot assign here.");
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

bool is_typ(Type *typ, TypeKind kind) {
    return typ->kind == kind;
}

// type_size decide type size of array recursively
int type_size(Type *typ) {
    if (is_typ(typ, TP_ARRAY)) {
        typ->size = type_size(typ->ptr_to) * typ->array_size;
        return typ->size;
    }
    return typ->size;
}

/*
    func
*/

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

/*
    local variable
*/

// find_lvar searches local variables,if exists return the local variable
// otherwise return NULL
Object *find_lvar(Token *tok) {
    for (Scope *scp = scopes; scp; scp = scp->before) {
        for (Object *var = scp->lvar; var; var = var->next) {
            if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
                return var;
            }
        }
    }
    return NULL;
}

// can_defined_lvar checks if the tok->name local variable can be defined
// if OK (the same name local variable is not defined), return true;
// else return false;
bool can_defined_lvar(Token *tok) {
    for (Object *var = scopes->lvar; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            return false;
        }
    }
    return true;
}

/*
    gvar
*/

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

/*
    scope
*/

void enter_scope(bool can_break, bool can_cont) {
    Scope *new        = calloc(1, sizeof(Scope));
    new->before       = scopes;
    scopes->next      = new;
    new->lvar         = new_object(OBJ_LVAR);
    new->lvar->next   = NULL;
    new->offset       = scopes->offset;
    new->lvar->offset = new->offset;

    // break or continue
    new->can_break = can_break | scopes->can_break;
    new->can_cont  = can_cont | scopes->can_cont;
    new->label     = scopes->label;
    scopes         = new;
}

void out_scope() {
    if (scopes->before == NULL) {
        error_at(token->str, "cannot get out of this scope.");
    }
    scopes->before->offset = scopes->offset;
    scopes                 = scopes->before;
}

int calc_aligment_offset(int min_offset, int alignment) {
    return (min_offset + alignment - 1) / alignment * alignment;
}

void add_offset(Scope *scope, int size, int alignment) {
    int offset          = calc_aligment_offset(scope->offset + size, alignment);
    scope->offset       = offset;
    scope->lvar->offset = offset;
}

/*
    eval
*/

// eval_const evaluate constant expression.
// ok-example) 4,(2+3)-2,(1*3)*3,&x+2,x-43
// ng-example) &x*2,(&x + 1) - 1
Node *eval_const(Node *node) {
    Node *lhs;
    Node *rhs;

    switch (node->kind) {
    case ND_NUM:
        return new_node_num(node->val);
    case ND_ADD:
        lhs = eval_const(node->lhs);
        rhs = eval_const(node->rhs);
        if (lhs->kind == ND_ADDR || lhs->kind == ND_GVAR) {
            if (rhs->kind != ND_NUM) {
                error_at(token->str, "cannot eval this value.");
            }
            return new_node(ND_ADD, lhs, rhs, lhs->typ);
        } else if (rhs->kind == ND_ADDR || rhs->kind == ND_GVAR) {
            if (lhs->kind != ND_NUM) {
                error_at(token->str, "cannot eval this value.");
            }
            return new_node(ND_ADD, rhs, lhs, rhs->typ);
        } else if (lhs->kind != ND_NUM || rhs->kind != ND_NUM) {
            error_at(token->str, "cannot eval this value.");
        }
        return new_node_num(lhs->val + rhs->val);
    case ND_SUB:
        lhs = eval_const(node->lhs);
        rhs = eval_const(node->rhs);
        if (lhs->kind == ND_ADDR || lhs->kind == ND_GVAR) {
            if (rhs->kind != ND_NUM) {
                error_at(token->str, "cannot eval this value.");
            }
            return new_node(ND_SUB, lhs, rhs, lhs->typ);
        } else if (rhs->kind == ND_ADDR || rhs->kind == ND_GVAR) {
            error_at(token->str, "cannot eval this value.");
        } else if (lhs->kind != ND_NUM || rhs->kind != ND_NUM) {
            error_at(token->str, "cannot eval this value.");
        }
        return new_node_num(lhs->val - rhs->val);
    case ND_MUL:
        lhs = eval_const(node->lhs);
        rhs = eval_const(node->rhs);
        if (lhs->kind != ND_NUM || rhs->kind != ND_NUM) {
            error_at(token->str, "cannot eval this value.");
        }
        return new_node_num(lhs->val * rhs->val);
    case ND_DIV:
        lhs = eval_const(node->lhs);
        rhs = eval_const(node->rhs);
        if (lhs->kind != ND_NUM || rhs->kind != ND_NUM) {
            error_at(token->str, "cannot eval this value.");
        }
        return new_node_num(lhs->val / rhs->val);
    case ND_EQ:
        lhs = eval_const(node->lhs);
        rhs = eval_const(node->rhs);
        if (lhs->kind != ND_NUM || rhs->kind != ND_NUM) {
            error_at(token->str, "cannot eval this value.");
        }
        return new_node_num(lhs->val == rhs->val);
    case ND_NEQ:
        lhs = eval_const(node->lhs);
        rhs = eval_const(node->rhs);
        if (lhs->kind != ND_NUM || rhs->kind != ND_NUM) {
            error_at(token->str, "cannot eval this value.");
        }
        return new_node_num(lhs->val != rhs->val);
    case ND_LT:
        lhs = eval_const(node->lhs);
        rhs = eval_const(node->rhs);
        if (lhs->kind != ND_NUM || rhs->kind != ND_NUM) {
            error_at(token->str, "cannot eval this value.");
        }
        return new_node_num(lhs->val < rhs->val);
    case ND_LEQ:
        lhs = eval_const(node->lhs);
        rhs = eval_const(node->rhs);
        if (lhs->kind != ND_NUM || rhs->kind != ND_NUM) {
            error_at(token->str, "cannot eval this value.");
        }
        return new_node_num(lhs->val <= rhs->val);
    case ND_ADDR:
        return node;
    case ND_GVAR:
        if (is_typ(node->typ, TP_ARRAY)) {
            return node;
        }
        error_at(token->str, "cannot evaluate this value.");
    case ND_ARRAY:
        for (Node *now = node->initlist; now; now = now->next) {
            now = eval_const(now);
        }
        return node;
    case ND_STR:
        node->typ         = new_type(TP_ARRAY);
        node->typ->ptr_to = new_type(TP_CHAR);
        return node;
    default:
        error_at(token->str, "cannot evaluate this value.");
    }
}

// eval transforms
// int x[4] = {0,1,3,3};
// to
// int x[4];x[0]=0;x[1]=1;x[2]=3;x[3]=3;
Node *eval(Node *lhs, Node *rhs) {
    Type *typ;
    if (!is_typ(lhs->typ, TP_ARRAY) || !is_typ(rhs->typ, TP_ARRAY)) {
        typ = can_assign(lhs->typ, rhs->typ);
        if (!typ) {
            error_at(token->str, "cannot assign to this value.");
        }
    }

    Node head;
    head.next  = NULL;
    Node *node = &head;
    int i;
    switch (rhs->kind) {
    case ND_ARRAY:
        i = 0;
        for (Node *now = rhs->initlist; now; now = now->next) {
            Node *new  = eval(access(lhs, new_node_num(i++)), now);
            node->next = new;
            node       = new;
        }
        rhs->initlist = head.next;
        return rhs;
    default:
        return new_node(ND_ASSIGN, lhs, rhs, typ);
    }
}

/*
    other
*/

Node *add_helper(Node *lhs, Node *rhs, NodeKind kind) {
    Node *node;
    Type *typ = can_add(lhs->typ, rhs->typ);
    if (!is_ptr(typ)) {
        node = new_node(kind, lhs, rhs, typ);
    } else if (is_ptr(lhs->typ)) {
        rhs  = new_node(ND_MUL, rhs, new_node_num(lhs->typ->ptr_to->size), new_type(TP_INT));
        node = new_node(kind, lhs, rhs, typ);
    } else {
        lhs  = new_node(ND_MUL, lhs, new_node_num(rhs->typ->ptr_to->size), new_type(TP_INT));
        node = new_node(kind, lhs, rhs, typ);
    }
    return node;
}

// access parse ptr[expr] -> *(ptr + expr)
Node *access(Node *ptr, Node *expr) {
    Node *node = add_helper(ptr, expr, ND_ADD);
    node       = new_node(ND_DEREF, node, NULL, NULL);
    if (!is_ptr(ptr->typ)) {
        error_at(token->str, "cannot dereference not pointer type.");
    }
    node->typ = ptr->typ->ptr_to;
    return node;
}

// access_member parse expr.ident -> *( expr + 'offset of ident' ) (expr must be struct)
Node *access_member(Node *expr, int offset, Type *typ) {
    if (!is_typ(expr->typ, TP_STRUCT)) {
        error_at(token->str, "cannot access member when expr is not struct.");
    }
    Node *node = new_node(ND_ADD, expr, new_node_num(offset), NULL);
    return new_node(ND_DEREF, node, NULL, typ);
}

// find_struct searches struct whose name is the same as tok->str
// otherwise return NULL
Struct *find_struct(char *name, int len) {
    for (Struct *strct = strcts; strct; strct = strct->next) {
        if (len == strct->len && !memcmp(name, strct->name, strct->len)) {
            return strct;
        }
    }
    return NULL;
}

Object *new_object(ObjectKind kind) {
    Object *obj = calloc(1, sizeof(Object));
    obj->kind   = kind;
    return obj;
}