#include "hcc.h"

// consume read a token and return true if next token is expected kind of token,
// otherwise,return false
bool consume(char *op) {
    if (token->kind != TK_RESERVED || token->len != strlen(op) ||
        memcmp(token->str, op, token->len)) {
        return false;
    }
    token = token->next;
    return true;
}

// consume_ident read a token and return the token if next token is identifier,
// otherwise return NULL
Token *consume_ident() {
    if (token->kind != TK_IDENT) {
        return NULL;
    }
    Token *tok = token;
    token      = token->next;
    return tok;
}

// consume_str read a token and return the token if next token is string literal,
// otherwise return NULL
Token *consume_str() {
    if (token->kind != TK_STR) {
        return NULL;
    }
    Token *tok = token;
    token      = token->next;
    return tok;
}

// expect read a token if next token is expected kind of token,
// otherwise print error
void expect(char *op) {
    if (token->kind != TK_RESERVED || token->len != strlen(op) ||
        memcmp(token->str, op, token->len)) {
        error_at(token->str, "not '%s'");
    }
    token = token->next;
}

// expect_number read a token if next token is number,
// otherwise print error
int expect_number() {
    if (token->kind != TK_NUM) {
        error_at(token->str, "not number");
    }
    int val = token->val;
    token   = token->next;
    return val;
}

Token *expect_ident() {
    if (token->kind != TK_IDENT) {
        error_at(token->str, "not ident");
    }
    Token *tok = token;
    token      = token->next;
    return tok;
}

bool at_eof() { return token->kind == TK_EOF; }

// construct AST

Node *new_node(NodeKind kind, Node *lhs, Node *rhs, Type *typ) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs  = lhs;
    node->rhs  = rhs;
    node->typ  = typ;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val  = val;
    node->typ  = new_type(TP_INT);
    return node;
}

Node *new_node_lvar(Token *tok, Type *typ) {
    Node *node   = calloc(1, sizeof(Node));
    node->kind   = ND_LVAR;
    LVar *lvar   = calloc(1, sizeof(LVar));
    lvar->next   = locals;
    lvar->name   = tok->str;
    lvar->len    = tok->len;
    lvar->offset = locals->offset + typ->size;
    node->offset = lvar->offset;
    lvar->typ    = typ;
    node->typ    = typ;
    locals       = lvar;
    return node;
}

Node *node_lvar(Token *tok) {
    Node *node   = calloc(1, sizeof(Node));
    node->kind   = ND_LVAR;
    LVar *lvar   = find_lvar(tok);
    node->offset = lvar->offset;
    node->typ    = lvar->typ;
    return node;
}

Node *new_node_gvar(Token *tok, Type *typ) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_GVAR;
    GVar *gvar = calloc(1, sizeof(GVar));
    gvar->next = globals;
    gvar->name = tok->str;
    node->name = tok->str;
    gvar->len  = tok->len;
    node->len  = tok->len;
    gvar->typ  = typ;
    node->typ  = typ;
    globals    = gvar;
    return node;
}

Node *node_gvar(Token *tok) {
    GVar *gvar = find_gvar(tok);
    if (!gvar) {
        return NULL;
    }
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_GVAR;
    node->name = gvar->name;
    node->len  = gvar->len;
    node->typ  = gvar->typ;
    return node;
}

Node *new_node_str(Token *tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_STR;
    Str *str   = calloc(1, sizeof(Str));
    str->next  = strs;
    if (strs) {
        str->id = strs->id + 1;
    } else {
        str->id = 0;
    }
    node->id   = str->id;
    str->val   = tok->str;
    node->name = tok->str;
    str->len   = tok->len;
    node->len  = tok->len;
    node->typ  = new_type_ptr(new_type(TP_CHAR));
    strs       = str;
    new_node_lvar(tok, node->typ);
    return node;
}

void program() {
    int i = 0;
    while (!at_eof()) {
        locals         = calloc(1, sizeof(LVar));
        locals->offset = 0;
        code[i++]      = ext_def();
        infof("finished ext definition(%d)", i);
    }
    code[i] = NULL;
}

// cmp_stmt = stmt* "}"
Node *cmp_stmt() {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    Node head;
    head.next = NULL;
    Node *now = &head;
    while (!consume("}")) {
        now->next = stmt();
        now       = now->next;
        now->next = NULL;
    }
    node->block = head.next;
    infof("finished '{ }'");
    return node;
}

// type_declare reads 'int **' like token.
// if not,return NULL;
Type *type_declare() {
    Type *typ;
    if (consume("int")) {
        typ = new_type(TP_INT);
    } else if (consume("char")) {
        typ = new_type(TP_CHAR);
    } else {
        return NULL;
    }

    while (consume("*")) {
        Type *next = new_type_ptr(typ);
        typ        = next;
    }
    return typ;
}

// type_array reads '[43][43]' like token.
Type *type_array(Type *typ) {
    while (consume("[")) {
        int array_size = expect_number();
        expect("]");
        Type *next       = new_type(TP_ARRAY);
        next->ptr_to     = typ;
        next->array_size = array_size;
        next->size       = typ->size * array_size;
        typ              = next;
    }
    return typ;
}

Node *ext_def() {
    Type *typ = type_declare();
    if (!typ) {
        error_at(token->str, "type declaration expected.");
    }
    Token *tok = expect_ident();
    if (consume("(")) {
        // function
        Node *node = calloc(1, sizeof(Node));
        node->name = tok->str;
        node->len  = tok->len;
        node->typ  = typ;
        node->kind = ND_FUNCDEF;
        register_func(node);
        node         = func_def(node);
        node->offset = locals->offset;
    } else {
        // global variable
        typ = type_array(typ);
        expect(";");
        Node *node = new_node_gvar(tok, typ);
        return node;
    }
}

Node *func_def(Node *node) {
    if (consume(")")) {
        infof("finished until '()'.");
        node->params = NULL;
        expect("{");
        node->body = cmp_stmt();
        return node;
    }

    Node head;
    head.next = NULL;
    Node *now = &head;
    while (1) {
        Type *typ = type_declare();
        if (!typ) {
            error_at(token->str, "type declaration expected.");
        }
        Token *tok = expect_ident();
        typ        = type_array(typ);
        now->next  = new_node_lvar(tok, typ);
        now        = now->next;
        if (now->typ->kind == TP_ARRAY) {
            now->typ->kind = TP_PTR;
        }

        if (!consume(",")) {
            expect(")");
            break;
        }
    }

    infof("finished until '(param)'.");
    node->params = head.next;
    expect("{");
    node->body = cmp_stmt();
    return node;
}

Node *stmt() {
    Node *node;

    if (consume("if")) {
        expect("(");
        node       = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        node->cond = expr();
        expect(")");
        infof("finished until 'if ( cond )'");
        node->then = stmt();
        infof("finished until 'if ( cond ) then'");

        if (consume("else")) {
            node->els = stmt();
        }

        return node;
    } else if (consume("return")) {
        node       = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs  = expr();
        expect(";");
        infof("finished until 'return expr;'");
        return node;
    } else if (consume("while")) {
        expect("(");
        node       = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        node->cond = expr();
        expect(")");
        node->then = stmt();
        return node;
    } else if (consume("for")) {
        expect("(");
        node       = calloc(1, sizeof(Node));
        node->kind = ND_FOR;
        if (!consume(";")) {
            node->ini = expr();
            consume(";");
        }
        if (!consume(";")) {
            node->cond = expr();
            consume(";");
        }
        if (!consume(")")) {
            node->step = expr();
            consume(")");
        }
        node->then = stmt();
        return node;
    } else if (consume("{")) {
        return cmp_stmt();
    }

    node = expr();
    expect(";");
    return node;
}

Node *expr() {
    Type *typ = type_declare();
    if (typ) {
        Token *tok = expect_ident();
        typ        = type_array(typ);
        Node *node = new_node_lvar(tok, typ);
        infof("finished until 'typ ident'(local variable declaration).");
        return node;
    }
    return assign();
}

Node *assign() {
    Node *node = equality();
    if (consume("=")) {
        node      = new_node(ND_ASSIGN, node, assign(), NULL);
        node->typ = can_assign(node->lhs->typ, node->rhs->typ);
        infof("finished until 'a = b'.");
    }
    return node;
}

Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("==")) {
            node = new_node(ND_EQ, node, relational(), new_type(TP_INT)); // actually bool
        } else if (consume("!=")) {
            node = new_node(ND_NEQ, node, relational(), new_type(TP_INT)); // actually bool
        } else {
            return node;
        }
    }
}

Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume("<")) {
            node = new_node(ND_LT, node, add(), new_type(TP_INT)); // actually bool
        } else if (consume("<=")) {
            node = new_node(ND_LEQ, node, add(), new_type(TP_INT)); // actually bool
        } else if (consume(">")) {
            node = new_node(ND_LT, add(), node, new_type(TP_INT)); // actually bool
        } else if (consume(">=")) {
            node = new_node(ND_LEQ, add(), node, new_type(TP_INT)); // actually bool
        } else {
            return node;
        }
    }
}

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

Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+")) {
            node = add_helper(node, mul(), ND_ADD);
        } else if (consume("-")) {
            node = add_helper(node, mul(), ND_SUB);
        } else {
            return node;
        }
    }
}

Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*")) {
            node      = new_node(ND_MUL, node, unary(), NULL);
            node->typ = node->lhs->typ;
        } else if (consume("/")) {
            node      = new_node(ND_DIV, node, unary(), NULL);
            node->typ = node->lhs->typ;
        } else {
            return node;
        }
    }
}

Node *unary() {
    if (consume("sizeof")) {
        Node *node = unary();
        return new_node_num(node->typ->size);
    }
    if (consume("&")) {
        Node *lhs = unary();
        return new_node(ND_ADDR, lhs, NULL, new_type_ptr(lhs->typ));
    }
    if (consume("*")) {
        Node *node = new_node(ND_DEREF, unary(), NULL, NULL);
        if (!is_ptr(node->lhs->typ)) {
            error_at(token->str, "cannot dereference not pointer type.");
        }
        node->typ = node->lhs->typ->ptr_to;
        return node;
    }
    if (consume("+")) {
        return postfix();
    }
    if (consume("-")) {
        Node *node = new_node(ND_SUB, new_node_num(0), postfix(), new_type(TP_INT));
        if (is_ptr(node->rhs->typ)) {
            error_at(token->str, "unary '-' cannot be used for address.");
        }
        return node;
    }
    return postfix();
}

Node *postfix() {
    Node *node = primary();
    for (;;) {
        if (consume("[")) {
            node = add_helper(node, expr(), ND_ADD);
            node = new_node(ND_DEREF, node, NULL, NULL);
            if (!is_ptr(node->lhs->typ)) {
                error_at(token->str, "cannot dereference not pointer type.");
            }
            node->typ = node->lhs->typ->ptr_to;
            expect("]");
            infof("finished until 'ident[ ]'(index access)");
        } else {
            return node;
        }
    }
}

Node *primary() {
    Node *node;
    Token *tok;

    // if next token is '(', primary should be '(' expr ')'
    if (consume("(")) {
        node = expr();
        expect(")");
        return node;
    }

    // next token is an identifier
    tok = consume_ident();
    if (tok) {
        node = calloc(1, sizeof(Node));

        if (consume("(")) { // function call
            node->kind = ND_CALLFUNC;
            node->name = tok->str;
            node->len  = tok->len;

            Func *func = find_func(node);
            node->typ  = func->ret;

            if (consume(")")) {
                return node;
            }

            Node head;
            head.next = NULL;
            Node *now = &head;
            while (1) {
                now->next = expr();
                now       = now->next;
                // TODO:type check of argment
                if (!consume(",")) {
                    expect(")");
                    break;
                }
            }
            node->params = head.next;
            return node;
        }

        node = node_gvar(tok);
        if (node) {
            return node;
        }
        node = node_lvar(tok);
        return node;
    }

    tok = consume_str();
    if (tok) {
        node = new_node_str(tok);
        return node;
    }
    return new_node_num(expect_number());
}