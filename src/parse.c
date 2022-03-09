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
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    // check if the same name local variable isn't defined
    if (!can_defined_lvar(tok)) {
        error_at(token->str, "the same name local variable is defined in this scope.");
    }

    LVar *lvar   = calloc(1, sizeof(LVar));
    lvar->next   = scopes->lvar;
    scopes->lvar = lvar;
    lvar->name   = tok->str;
    lvar->len    = tok->len;
    add_offset(scopes, typ->size);
    node->offset = lvar->offset;
    lvar->typ    = typ;
    node->typ    = typ;
    return node;
}

Node *node_lvar(Token *tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    LVar *lvar = find_lvar(tok);
    if (!lvar) {
        return NULL;
    }
    node->offset = lvar->offset;
    node->typ    = lvar->typ;
    return node;
}

Node *new_node_gvar(Token *tok, Type *typ) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_GVAR;

    // check if the same name global variable isn't defined
    GVar *gvar = find_gvar(tok);
    assert_at(!gvar, token->str, "the same name global variable is defined ever.");

    gvar       = calloc(1, sizeof(GVar));
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
        error_at(token->str, "this variable isn't defined.");
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
    return node;
}

// program = ext_def*
void program() {
    int i = 0;
    while (!at_eof()) {
        scopes->offset = 0;
        code[i++]      = ext_def();
        infof("finished ext definition(%d)", i);
    }
    code[i] = NULL;
}

// ext_def = decl_spec declarator ';'
//         | decl_spec declarator '=' initializer ';'
//         | decl_spec declarator '{' cmp_stmt
Node *ext_def() {
    Type *typ = decl_spec();
    assert_at(typ, token->str, "type declaration expected.");
    Node *node = declarator(typ, true);

    if (node->kind == ND_GVAR) {
        if (consume("=")) {
            // global variable definition
            Node *rhs = initializer(true);
            expect(";");
            // array assignment is only possible during initialization.
            if (is_typ(node->typ, TP_ARRAY) && is_typ(rhs->typ, TP_ARRAY)) {
                return new_node(ND_INIT, node, rhs, node->typ);
            } else {
                Type *typ = can_assign(node->typ, rhs->typ);
                assert_at(typ, token->str, "cannot initialize with this value.");
                return new_node(ND_INIT, node, rhs, typ);
            }
        } else {
            // global variablee declarator
            expect(";");
            return node;
        }
    }

    if (node->kind != ND_GVAR) {
        if (consume("{")) {
            // function definition
            node->kind = ND_FUNCDEF;
            register_func(node);
            node->body = cmp_stmt();
            out_scope();
            node->offset = scopes->offset;
            return node;
        } else {
            // function declaration
            expect(";");
            node->kind = ND_FUNCDECL;
            register_func(node);
            out_scope();
            return node;
        }
    }
    return node;
}

// decl_spec = "int"
//           | "char"
// if not,return NULL;
Type *decl_spec() {
    Type *typ;
    if (consume("int")) {
        typ = new_type(TP_INT);
    } else if (consume("char")) {
        typ = new_type(TP_CHAR);
    } else {
        return NULL;
    }
    return typ;
}

// declarator = pointer direct_decl
Node *declarator(Type *typ, bool is_global) {
    typ = pointer(typ);
    return direct_decl(typ, is_global);
}

// pointer = '*' *
Type *pointer(Type *typ) {
    while (consume("*")) {
        Type *next = new_type_ptr(typ);
        typ        = next;
    }
    return typ;
}

// direct_decl = ident
//             | ident typ_array
//             | ident '(' func_param
Node *direct_decl(Type *typ, bool is_global) {
    Node *node;
    Token *tok = expect_ident();
    typ        = type_array(typ);

    if (consume("(")) {
        // function definition or declaration
        assert_at(is_global, token->str, "function definition of declaration can't exist in function.");
        node       = calloc(1, sizeof(Node));
        node->name = tok->str;
        node->len  = tok->len;
        node->typ  = typ;
        node       = func_param(node);
        return node;
    } else {
        // variable
        return is_global ? new_node_gvar(tok, typ) : new_node_lvar(tok, typ);
    }
}

// type_array = ( '[' num ']' )*
Type *type_array(Type *typ) {
    int arr_size[30]; // [] repeats at most 30
    int i = 0;
    while (consume("[")) {
        arr_size[i++] = expect_number();
        expect("]");
        Type *next   = new_type(TP_ARRAY);
        next->ptr_to = typ;
        typ          = next;
    }
    Type *now = typ;
    for (int j = 0; j < i; j++) {
        now->array_size = arr_size[j];
        now             = now->ptr_to;
    }
    type_size(typ);
    return typ;
}

// func_param = ')'
//            | decl_spec declarator ( ',' decl_spec declarator )* ')'
Node *func_param(Node *node) {
    enter_scope();
    if (consume(")")) {
        infof("finished until '()'.");
        node->params = NULL;
        return node;
    }

    Node head;
    head.next = NULL;
    Node *now = &head;
    while (1) {
        Type *typ = decl_spec();
        assert_at(typ, token->str, "type declaration expected.");
        now->next = declarator(typ, false);
        now       = now->next;
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
    return node;
}

// initializer = expr
//             = "{" initializer_list ','? "}"
Node *initializer(bool is_constant) {
    if (consume("{")) {
        Node *node = initializer_list(is_constant);
        consume(",");
        expect("}");
        return is_constant ? eval_const(node) : node;
    }
    Node *node = expr();
    return is_constant ? eval_const(node) : node;
}

// initiaizer_list =  initializer (',' initializer)*
Node *initializer_list(bool is_constant) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_ARRAY;
    Node head;
    head.next = initializer(is_constant);
    Node *now = head.next;
    while (consume(",")) {
        now->next = initializer(is_constant);
        now       = now->next;
    }
    node->initlist    = head.next;
    node->typ         = new_type(TP_ARRAY);
    node->typ->ptr_to = node->initlist->typ;
    return node;
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

// stmt = "if" '(' expr ')' stmt ( "else" stmt )?
//      | "return" expr ';'
//      | "while" '(' expr ')' stmt
//      | "for" '(' expr? ';' expr? ';' expr; ')' stmt
//      | '{' cmp_stmt
//      | expr ';'
Node *stmt() {
    Node *node;

    if (consume("if")) {
        expect("(");
        node       = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        node->cond = expr();
        expect(")");
        infof("finished until 'if ( cond )'");
        enter_scope();
        node->then = stmt();
        out_scope();
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
        enter_scope();
        node->then = stmt();
        out_scope();
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
        enter_scope();
        node->then = stmt();
        out_scope();
        return node;
    } else if (consume("{")) {
        enter_scope();
        node = cmp_stmt();
        out_scope();
        return node;
    }

    node = expr();
    expect(";");
    return node;
}

// expr = decl_spec declarator
//      | decl_spec declarator '=' initializer
//      | assign
Node *expr() {
    Type *typ = decl_spec();
    Node *node;

    if (typ) {
        node = declarator(typ, false);
        if (consume("=")) { // initiliazer
            return eval(node, initializer(false));
        }
        return node;
    }
    return assign();
}

// assign = equality
//        | equality '=' assign
Node *assign() {
    Node *node = equality();
    if (consume("=")) {
        node      = new_node(ND_ASSIGN, node, assign(), NULL);
        node->typ = can_assign(node->lhs->typ, node->rhs->typ);
        infof("finished until 'a = b'.");
    }
    return node;
}

// equality = relational ( "==" relational | "!=" relational )*
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

// relational = add ( '<' add | "<=" add | '>' add | ">=" add)*
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

// add = mul ( '+' mul | '-' mul)*
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

// mul = unary ( '*' unary | '/' unary )*
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

// unary = "sizeof" unary
//       | '&' unary
//       | '*' unary
//       | '+' postfix
//       | '-' postfix
//       | postfix
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

// postfix = primary ( '[' expr ']' )*
Node *postfix() {
    Node *node = primary();
    for (;;) {
        if (consume("[")) {
            node = access(node, expr());
            expect("]");
            infof("finished until 'ident[ ]'(index access)");
        } else {
            return node;
        }
    }
}

// primary = "({" cmp_stmt ')'
//         | '(' expr ')'
//         | ident
//         | ident "()"
//         | ident '(' expr ( ',' expr )* ')'
//         | string
//         | num
Node *primary() {
    Node *node;
    Token *tok;

    if (consume("(")) {
        if (consume("{")) { // statement expression
            enter_scope();
            node = cmp_stmt();
            out_scope();
            Type *typ;
            for (Node *nd = node->block; nd; nd = nd->next) {
                typ = nd->typ;
            }
            node = new_node(ND_STMTEXPR, node, NULL, typ);
        } else {
            node = expr(); // '(' + expr ')'
        }
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

        node = node_lvar(tok);
        if (node) {
            return node;
        }
        node = node_gvar(tok);
        return node;
    }

    tok = consume_str();
    if (tok) {
        node = new_node_str(tok);
        return node;
    }
    return new_node_num(expect_number());
}