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

void program() {
    int i = 0;
    while (!at_eof()) {
        locals->offset    = 0;
        code[i]           = func_def();
        code[i++]->offset = locals->offset;
        infof("finished function(%d)", i);
    }
    code[i] = NULL;
}

Node *cmp_stmt() {
    expect("{");
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
    return node;
}

Type *type_declare() {
    expect("int");
    Type *typ = new_type(TP_INT);
    while (consume("*")) {
        Type *next   = new_type(TP_PTR);
        next->ptr_to = typ;
        typ          = next;
    }
    return typ;
}

Node *func_def() {
    Type *typ  = type_declare();
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_FUNCDEF;
    Token *tok = expect_ident();
    node->name = tok->str;
    node->len  = tok->len;
    node->typ  = typ;
    register_func(node);

    expect("(");
    if (consume(")")) {
        node->params = NULL;
        node->body   = cmp_stmt();
        return node;
    }

    Node head;
    head.next = NULL;
    Node *now = &head;
    while (1) {
        Type *typ  = type_declare();
        Token *tok = expect_ident();
        now->next  = new_node_lvar(tok, typ);
        now        = now->next;

        if (!consume(",")) {
            expect(")");
            break;
        }
    }

    infof("finished until { }.");
    node->params = head.next;
    node->body   = cmp_stmt();
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
        node       = calloc(1, sizeof(Node));
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
        return node;
    }

    node = expr();
    expect(";");
    return node;
}

Node *expr() {
    if (consume("int")) {
        Type *typ = new_type(TP_INT);
        while (consume("*")) {
            Type *next   = new_type(TP_PTR);
            next->ptr_to = typ;
            typ          = next;
        }

        Token *tok = expect_ident();
        Node *node = new_node_lvar(tok, typ);
        return node;
    }
    return assign();
}

Node *assign() {
    Node *node = equality();
    if (consume("=")) {
        node = new_node(ND_ASSIGN, node, assign(), NULL);
        if (!type_cmp(node->lhs->typ, node->rhs->typ)) {
            error_at(token->str, "cannot assign different type.");
        }
        node->typ = node->lhs->typ;
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

Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+")) {
            node      = new_node(ND_ADD, node, mul(), NULL);
            node->typ = node->lhs->typ;
        } else if (consume("-")) {
            node      = new_node(ND_SUB, node, mul(), NULL);
            node->typ = node->lhs->typ;
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
    if (consume("&")) {
        Node *node        = new_node(ND_ADDR, unary(), NULL, new_type(TP_PTR));
        node->typ->ptr_to = node->lhs->typ;
        return node;
    }
    if (consume("*")) {
        Node *node = new_node(ND_DEREF, unary(), NULL, NULL);
        if (node->lhs->typ->kind == TP_INT) {
            error_at(token->str, "cannot dereference not pointer type.");
        }
        node->typ = node->lhs->typ->ptr_to;
        return node;
    }
    if (consume("+")) {
        return primary();
    }
    if (consume("-")) {
        Node *node = new_node(ND_SUB, new_node_num(0), primary(), new_type(TP_INT));
        if (node->rhs->typ->kind != TP_INT) {
            error_at(token->str, "unary '-' cannot be used for address.");
        }
        return node;
    }
    return primary();
}

Node *primary() {
    // if next token is '(', primary should be '(' expr ')'
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    // next token is an identifier
    Token *tok = consume_ident();
    if (tok) {
        Node *node = calloc(1, sizeof(Node));

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

        node = node_lvar(tok);
        return node;
    }

    return new_node_num(expect_number());
}