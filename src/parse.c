#include "hcc.h"

// counter create unique number
int counter() {
    static int count = 0;
    return count++;
}

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

// lookahead read a token and return true if next token is expected kind of token,
// otherwise,return false. THIS FUNCTION DOESN'T CONSUME token.
bool lookahead(char *op, Token *tok) {
    if (tok->kind != TK_RESERVED || tok->len != strlen(op) ||
        memcmp(tok->str, op, tok->len)) {
        return false;
    }
    return true;
}

bool is_type(Token *tok) {
    return lookahead("int", tok) || lookahead("char", tok) || lookahead("struct", tok) || lookahead("void", tok) || lookahead("enum", tok) || lookahead("typedef", tok) || lookahead_typdef(tok);
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

    Object *lvar = new_object(OBJ_LVAR);
    lvar->next   = scopes->lvar;
    scopes->lvar = lvar;
    lvar->name   = tok->str;
    lvar->len    = tok->len;
    add_offset(scopes, typ->size, byte_align(typ));
    node->offset = lvar->offset;
    lvar->typ    = typ;
    node->typ    = typ;
    return node;
}

Node *node_obj(Token *tok) {
    Object *obj = find_obj(tok);
    if (!obj) {
        return NULL;
    }
    if (obj->kind == OBJ_LVAR) {
        // lvar
        Node *node   = calloc(1, sizeof(Node));
        node->kind   = ND_LVAR;
        node->offset = obj->offset;
        node->typ    = obj->typ;
    } else if (obj->kind == OBJ_ENUM) {
        // enum
        return new_node_num(obj->val);
    }
}

Node *new_node_gvar(Token *tok, Type *typ) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_GVAR;

    // check if the same name global variable isn't defined
    Object *gvar = find_gvar(tok);
    assert_at(!gvar, token->str, "the same name global variable is defined ever.");

    gvar       = new_object(OBJ_GVAR);
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
    Object *gvar = find_gvar(tok);
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
    node->id        = str->id;
    str->val        = tok->str;
    node->name      = tok->str;
    str->len        = tok->len;
    node->len       = tok->len;
    node->typ       = new_type_ptr(new_type(TP_CHAR));
    node->type_size = tok->type_len;
    strs            = str;
    return node;
}

Node *new_node_mem(Token *tok, Type *typ) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_MEMBER;
    node->name = tok->str;
    node->len  = tok->len;
    node->typ  = typ;
    return node;
}

Member *new_mem(Node *node) {
    Member *mem = calloc(1, sizeof(Member));
    mem->name   = node->name;
    mem->len    = node->len;
    mem->typ    = node->typ;
    return mem;
}

// program = ext_def*
void program() {
    int i = 0;
    while (!at_eof()) {
        scopes->offset = 0;
        Node *node     = ext_def();
        if (node) {
            code[i++] = node;
            infof("finished ext definition(%d)", i);
        }
    }
    code[i] = NULL;
}

// ext_def = decl_spec ';'
//         | decl_spec declarator ';'
//         | decl_spec declarator '=' initializer ';'
//         | decl_spec declarator '{' cmp_stmt
Node *ext_def() {
    Type *typ = decl_spec();
    assert_at(typ, token->str, "type declaration expected.");
    Node *node = declarator(typ, GLOBAL);

    if (!node) {
        assert_at(is_typ(typ, TP_STRUCT) || is_typ(typ, TP_ENUM), token->str, "need declarator when you don't define struct,or enum");
        expect(";");
        // struct,enum definition
        return NULL;
    }

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
            // global variable declarator
            expect(";");
            return node;
        }
    } else if (node->kind == ND_TYPEDEF) {
        // typedef
        expect(";");
        return NULL;
    } else {
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

// decl_spec = ( "typedef" | type_spec )*
Type *decl_spec() {
    Type *typ      = NULL;
    bool is_typdef = false;
    for (;;) {
        if (consume("typedef")) {
            is_typdef = true;
            continue;
        } else if (is_type(token)) {
            Type *new = type_spec();
            assert_at(new, token->str, "type specifier expected.");
            assert_at(!typ, token->str, "two type specifier exists at the same time.");
            typ = new;
            continue;
        } else {
            break;
        }
    }
    typ->is_typdef = is_typdef;
    return typ;
}

// type_spec = "int"
//           | "char"
//           | "void"
//           | "enum" enum_spec
//           | "struct" struct_spec
//           | typedef_name
// if not,return NULL;
Type *type_spec() {
    if (consume("int")) {
        return new_type(TP_INT);
    } else if (consume("char")) {
        return new_type(TP_CHAR);
    } else if (consume("void")) {
        return new_type(TP_VOID);
    } else if (consume("enum")) {
        return enum_spec();
    } else if (consume("struct")) {
        return struct_spec();
    } else {
        Token *tok = consume_ident();
        if (!tok) return NULL;
        Object *td = find_typdef(tok);
        if (!td) return NULL;
        return td->typ;
    }
}

// struct_spec = ident
//             | ident '{' struct_decl+ '}'
Type *struct_spec() {
    Token *tok = expect_ident();
    if (consume("{")) {
        // struct definition
        Member head;
        head.next   = NULL;
        Member *cur = &head;
        while (cur) {
            cur->next = struct_decl();
            cur       = cur->next;
        }
        expect("}");
        assert_at(head.next, token->str, "there is no struct member.");
        assert_at(can_define_strct(tok), token->str, "double definition of struct.");
        Struct *st = find_struct(tok->str, tok->len);
        // if st->is_defined, this means st is out of scope, and defined there,
        // so we have to define the same name struct in this scope, but isn't defined,
        // there are declaration of struct in this scope.
        return (!st || st->is_defined) ? new_type_strct(tok, head.next) : define_type_strct(st, head.next);
    } else {
        Struct *st = find_struct(tok->str, tok->len);
        if (!st) {
            // this struct isn't declared yet
            return declare_type_strct(tok);
        } else if (st->is_defined) {
            // struct is already defined
            return st->typ;
        } else {
            // this struct is declared, but not defined.
            return st->typ;
        }
    }
}

// struct_decl = type_spec declarator ';'
// if not,return NULL
Member *struct_decl() {
    Type *typ = type_spec();
    if (!typ) {
        return NULL;
    }
    Node *node = declarator(typ, STRUCT);
    assert_at(!is_typ(node->typ, TP_VOID), token->str, "cannot use void here.");
    Member *mem = new_mem(node);
    expect(";");
    return mem;
}

// enum_spec = ident
//           | ident '{'( enumerator ',' )+ '}'
Type *enum_spec() {
    Token *tok = expect_ident();
    Object *en = new_object(OBJ_ENUM);
    en->name   = tok->str;
    en->len    = tok->len;

    if (!consume("{")) {
        Object *obj = find_enum(tok);
        assert_at(obj, token->str, "not enum specifier");
        return obj->typ;
    }

    int val = 0;
    Object head;
    head.next   = NULL;
    Object *now = &head;
    while (!consume("}")) {
        now->next = enumerator();
        now       = now->next;
        now->next = NULL;
        if (now->is_init) {
            val = now->val + 1;
        } else {
            now->val = val++;
        }
    }
    assert_at(head.next, token->str, "there is no enumerator.");
    en->enum_list = head.next;
    return new_type_enum(en);
}

// enumerator = ( ident | ident '=' expr ) ','
Object *enumerator() {
    Token *tok  = expect_ident();
    Object *en  = new_object(OBJ_ENUM);
    en->name    = tok->str;
    en->len     = tok->len;
    en->is_init = false;
    if (consume("=")) {
        Node *node = expr();
        node       = eval_const(node);
        assert_at(node->kind == ND_NUM, token->str, "cannot use not integer value as enum initializer.");
        en->val     = node->val;
        en->is_init = true;
    }
    expect(",");
    return en;
}

// declarator = pointer direct_decl
Node *declarator(Type *typ, Param p) {
    typ = pointer(typ);
    return direct_decl(typ, p);
}

// pointer = '*' *
Type *pointer(Type *typ) {
    while (consume("*")) {
        Type *next      = new_type_ptr(typ);
        next->is_typdef = typ->is_typdef;
        typ             = next;
    }
    return typ;
}

// direct_decl = ident
//             | ident typ_array
//             | ident '(' func_param
// if not,return NULL
Node *direct_decl(Type *typ, Param p) {
    Node *node;
    Token *tok = consume_ident();
    if (!tok) {
        return NULL;
    }
    typ = type_array(typ);

    if (consume("(")) {
        // function definition or declaration
        assert_at(p == GLOBAL, token->str, "function definition of declaration can't exist in function.");
        assert_at(!(typ->is_typdef), token->str, "typedef cannot be used for function declaration.");
        node       = calloc(1, sizeof(Node));
        node->name = tok->str;
        node->len  = tok->len;
        node->typ  = typ;
        node       = func_param(node);
        return node;
    } else if (p == STRUCT) {
        // struct member
        return new_node_mem(tok, typ);
    } else if (typ->is_typdef) {
        return new_typdef(tok, typ);
    } else {
        // variable
        return p == GLOBAL ? new_node_gvar(tok, typ) : new_node_lvar(tok, typ);
    }
}

// type_array = ( '[' num ']' )*
Type *type_array(Type *typ) {
    int arr_size[30]; // [] repeats at most 30
    int i = 0;
    while (consume("[")) {
        arr_size[i++] = expect_number();
        expect("]");
        Type *next      = new_type(TP_ARRAY);
        next->ptr_to    = typ;
        next->is_typdef = typ->is_typdef;
        typ             = next;
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
//            | type_spec declarator ( ',' type_spec declarator )* ')'
Node *func_param(Node *node) {
    enter_scope(false, false);
    if (consume(")")) {
        infof("finished until '()'.");
        node->params = NULL;
        return node;
    }

    Node head;
    head.next = NULL;
    Node *now = &head;
    while (1) {
        Type *typ = type_spec();
        assert_at(typ, token->str, "type declaration expected.");
        now->next = declarator(typ, LOCAL);
        now       = now->next;
        assert_at(!is_typ(now->typ, TP_VOID), token->str, "cannot use void here.");
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

// labeled_stmt = "case" expr ':' stmt* | "default" ':' stmt*
// expr must be constant expression
// otherwise return NULL
Node *labeled_stmt() {
    Node *node = calloc(1, sizeof(Node));
    Node head;
    head.next = NULL;
    Node *now = &head;
    if (consume("case")) {
        node->kind = ND_CASE;
        node->cond = expr();
        expect(":");
        while (1) {
            if (lookahead("case", token) || lookahead("default", token) || lookahead("}", token)) {
                break;
            }
            now->next = stmt();
            now       = now->next;
            now->next = NULL;
        }
        node->block = head.next;
        return node;
    } else if (consume("default")) {
        node->kind = ND_DEFAULT;
        expect(":");
        while (1) {
            if (lookahead("case", token) || lookahead("default", token) || lookahead("}", token)) {
                break;
            }
            now->next = stmt();
            now       = now->next;
            now->next = NULL;
        }
        node->block = head.next;
        return node;
    }
    return NULL;
}

// stmt = "if" '(' expr ')' stmt ( "else" stmt )?
//      | "switch" '(' expr ')' ( stmt | "case" expr ':' stmt | "default" ':' stmt | '{' stmt* labeled_stmt* '}' )
//      | "return" expr? ';'
//      | "while" '(' expr ')' stmt
//      | "for" '(' ( expr | declaration )? ';' expr? ';' expr; ')' stmt
//      | "break" ';'
//      | "continue" ';'
//      | '{' cmp_stmt
//      | declaration ';'
//      | expr ';'
Node *stmt() {
    Node *node;

    if (consume("if")) {
        expect("(");
        node        = calloc(1, sizeof(Node));
        node->kind  = ND_IF;
        node->label = counter();
        node->cond  = expr();
        expect(")");
        infof("finished until 'if ( cond )'");
        enter_scope(false, false);
        node->then = stmt();
        out_scope();
        infof("finished until 'if ( cond ) then'");

        if (consume("else")) {
            node->els = stmt();
        }

        return node;
    } else if (consume("switch")) {
        node        = calloc(1, sizeof(Node));
        node->kind  = ND_SWITCH;
        node->label = counter();
        expect("(");
        node->cond = expr();
        expect(")");
        enter_scope(true, false);
        scopes->label = node->label;

        if (!consume("{")) {
            Node *candi;
            if (consume("case")) {
                candi       = calloc(1, sizeof(Node));
                candi->kind = ND_CASE;
                candi->cond = expr();
                expect(":");
                candi->block = stmt();
                node->block  = candi;
            } else if (consume("default")) {
                candi       = calloc(1, sizeof(Node));
                candi->kind = ND_DEFAULT;
                expect(":");
                candi->block = stmt();
                node->block  = candi;
            } else {
                stmt();
            }
            out_scope();
            return node;
        }

        Node head;
        head.next        = NULL;
        Node *now        = &head;
        bool has_default = false;
        while (1) {
            now->next = labeled_stmt();
            if (now->next) {
                if (now->next->kind == ND_DEFAULT) {
                    // default
                    assert_at(!has_default, token->str, "two 'default' statement exist in one 'switch' statement.");
                    has_default = true;
                    now         = now->next;
                    now->next   = NULL;
                } else {
                    // case
                    now       = now->next;
                    now->next = NULL;
                }
            } else if (consume("}")) {
                break;
            } else {
                stmt(); // ignore
            }
        }
        out_scope();
        node->block = head.next;
        return node;
    } else if (consume("return")) {
        node       = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        if (consume(";")) {
            node->lhs = NULL;
            return node;
        }
        node->lhs = expr();
        expect(";");
        return node;
    } else if (consume("while")) {
        expect("(");
        node        = calloc(1, sizeof(Node));
        node->kind  = ND_WHILE;
        node->label = counter();
        node->cond  = expr();
        expect(")");
        enter_scope(true, true);
        scopes->label = node->label;
        node->then    = stmt();
        out_scope();
        return node;
    } else if (consume("for")) {
        expect("(");
        node        = calloc(1, sizeof(Node));
        node->kind  = ND_FOR;
        node->label = counter();
        enter_scope(true, true);
        scopes->label = node->label;
        if (!consume(";")) {
            if (is_type(token))
                node->ini = declaration();
            else
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
        out_scope();
        return node;
    } else if (consume("break")) {
        assert_at(scopes->can_break, token->str, "cannot 'break' here.");
        expect(";");
        node        = new_node(ND_BREAK, NULL, NULL, NULL);
        node->label = scopes->label;
        return node;
    } else if (consume("continue")) {
        assert_at(scopes->can_cont, token->str, "cannot 'continue' here.");
        expect(";");
        node        = new_node(ND_CONTINUE, NULL, NULL, NULL);
        node->label = scopes->label;
        return node;
    } else if (consume("{")) {
        enter_scope(false, false);
        node = cmp_stmt();
        out_scope();
        return node;
    } else if (is_type(token)) {
        node = declaration();
        expect(";");
        return node;
    }

    node = expr();
    expect(";");
    return node;
}

// declaration = decl_spec
//             | decl_spec init_decl ( ',' init_decl )*
//
// this is expression.
Node *declaration() {
    Type *typ = decl_spec();
    assert_at(typ, token->str, "declaration specifier expected.");

    if (lookahead(";", token)) {
        return new_node_num(0); // meaningless node
    }

    if (typ->is_typdef) {
        // typdef int a,b;
        init_decl(typ);
        while (consume(",")) {
            init_decl(typ);
        }
        return new_node_num(0); // meaningless node
    } else {
        // int a=0,*b=&a,c[3]={0,1},d transforms to ({ int a=0,*b=&a,c[3]={0,1},d }) like expression
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        Node head;
        Node *now = &head;
        now->next = init_decl(typ);
        now       = now->next;
        now->next = NULL;
        node->typ = now->typ;
        while (consume(",")) {
            now->next = init_decl(typ);
            now       = now->next;
            now->next = NULL;
            node->typ = now->typ;
        }
        node->block = head.next;
        return new_node(ND_STMTEXPR, node, NULL, node->typ);
    }
}

// init_decl = declarator | declarator '=' initializer
Node *init_decl(Type *typ) {
    Node *node = declarator(typ, LOCAL);
    assert_at(!is_typ(node->typ, TP_VOID), token->str, "cannot use void here.");
    if (consume("=")) { // initiliazer
        assert_at(node->kind != ND_TYPEDEF, token->str, "cannot use '=' here.");
        return eval(node, initializer(false));
    }
    return node;
}

// expr = assign
Node *expr() {
    return assign();
}

// assign = conditional
//        | conditional '=' assign
Node *assign() {
    Node *node = conditional();
    if (consume("=")) {
        node      = new_node(ND_ASSIGN, node, assign(), NULL);
        node->typ = can_assign(node->lhs->typ, node->rhs->typ);
        infof("finished until 'a = b'.");
    }
    return node;
}

// conditional = log_or
//             | log_or '?' expr ':' conditional
Node *conditional() {
    Node *node = log_or();
    if (consume("?")) {
        Node *cond  = node;
        node        = calloc(1, sizeof(Node));
        node->kind  = ND_TERNARY;
        node->label = counter();
        node->cond  = cond;
        node->then  = expr();
        consume(":");
        node->els = conditional();
        node->typ = can_assign(node->then->typ, node->els->typ);
        return node;
    }
    return node;
}

// log_or = log_and ( "||" log_and )*
Node *log_or() {
    Node *node = log_and();
    for (;;) {
        if (consume("||")) {
            Node *lhs = node;
            Node *rhs = log_and();
            assert_at(is_typ(lhs->typ, TP_INT) || is_typ(lhs->typ, TP_CHAR), token->str, "cannot use '||' here.");
            assert_at(is_typ(rhs->typ, TP_INT) || is_typ(rhs->typ, TP_CHAR), token->str, "cannot use '||' here.");
            lhs  = new_node(ND_NEQ, lhs, new_node_num(0), new_type(TP_INT));
            rhs  = new_node(ND_NEQ, rhs, new_node_num(0), new_type(TP_INT));
            node = new_node(ND_OR, lhs, rhs, new_type(TP_INT));
        } else {
            return node;
        }
    }
}

// log_and = or_expr ( "&&" or_expr )*
Node *log_and() {
    Node *node = or_expr();
    for (;;) {
        if (consume("&&")) {
            Node *lhs = node;
            Node *rhs = or_expr();
            assert_at(is_typ(lhs->typ, TP_INT) || is_typ(lhs->typ, TP_CHAR), token->str, "cannot use '&&' here.");
            assert_at(is_typ(rhs->typ, TP_INT) || is_typ(rhs->typ, TP_CHAR), token->str, "cannot use '&&' here.");
            lhs  = new_node(ND_NEQ, lhs, new_node_num(0), new_type(TP_INT));
            rhs  = new_node(ND_NEQ, rhs, new_node_num(0), new_type(TP_INT));
            node = new_node(ND_AND, lhs, rhs, new_type(TP_INT));
        } else {
            return node;
        }
    }
}

// or_expr = xor_expr ( '|' xor_expr )*
Node *or_expr() {
    Node *node = xor_expr();
    for (;;) {
        if (consume("|")) {
            node = new_node(ND_OR, node, xor_expr(), new_type(TP_INT));
            assert_at(is_typ(node->lhs->typ, TP_INT) || is_typ(node->lhs->typ, TP_CHAR), token->str, "cannot use '|' here.");
            assert_at(is_typ(node->rhs->typ, TP_INT) || is_typ(node->rhs->typ, TP_CHAR), token->str, "cannot use '|' here.");
        } else {
            return node;
        }
    }
}

// xor_expr = and_expr ( '^' and_expr )*
Node *xor_expr() {
    Node *node = and_expr();
    for (;;) {
        if (consume("^")) {
            node = new_node(ND_XOR, node, and_expr(), new_type(TP_INT));
            assert_at(is_typ(node->lhs->typ, TP_INT) || is_typ(node->lhs->typ, TP_CHAR), token->str, "cannot use '^' here.");
            assert_at(is_typ(node->rhs->typ, TP_INT) || is_typ(node->rhs->typ, TP_CHAR), token->str, "cannot use '^' here.");
        } else {
            return node;
        }
    }
}

// and_expr = equality ( '&' equality )*
Node *and_expr() {
    Node *node = equality();
    for (;;) {
        if (consume("&")) {
            node = new_node(ND_AND, node, equality(), new_type(TP_INT));
            assert_at(is_typ(node->lhs->typ, TP_INT) || is_typ(node->lhs->typ, TP_CHAR), token->str, "cannot use '&' here.");
            assert_at(is_typ(node->rhs->typ, TP_INT) || is_typ(node->rhs->typ, TP_CHAR), token->str, "cannot use '&' here.");
        } else {
            return node;
        }
    }
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

// mul = unary ( '*' unary | '/' unary | '%' unary )*
Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*")) {
            node      = new_node(ND_MUL, node, unary(), NULL);
            node->typ = node->lhs->typ;
        } else if (consume("/")) {
            node      = new_node(ND_DIV, node, unary(), NULL);
            node->typ = node->lhs->typ;
        } else if (consume("%")) {
            node = new_node(ND_MOD, node, unary(), node->typ);
        } else {
            return node;
        }
    }
}

// unary = "sizeof" unary
//       | "sizeof" decl_spec
//       | "++" unary
//       | "--" unary
//       | '&' unary
//       | '*' unary
//       | '+' unary
//       | '-' unary
//       | postfix
Node *unary() {
    if (consume("sizeof")) {
        if (lookahead("(", token) && is_type(token->next)) {
            expect("(");
            Type *typ = decl_spec();
            expect(")");
            assert_at(typ, token->str, "must be type?");
            return new_node_num(typ->size);
        }
        Node *node = unary();
        if (node->kind == ND_STR) return new_node_num(node->type_size);
        return new_node_num(node->typ->size);
    }
    if (consume("++")) {
        Node *lhs  = unary();
        Node *rhs  = add_helper(lhs, new_node_num(1), ND_ADD);
        Node *node = new_node(ND_ASSIGN, lhs, rhs, NULL);
        node->typ  = can_assign(lhs->typ, rhs->typ);
        return node;
    }
    if (consume("--")) {
        Node *lhs  = unary();
        Node *rhs  = add_helper(lhs, new_node_num(1), ND_SUB);
        Node *node = new_node(ND_ASSIGN, lhs, rhs, NULL);
        node->typ  = can_assign(lhs->typ, rhs->typ);
        return node;
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
        return unary();
    }
    if (consume("-")) {
        Node *node = new_node(ND_SUB, new_node_num(0), unary(), new_type(TP_INT));
        if (is_ptr(node->rhs->typ)) {
            error_at(token->str, "unary '-' cannot be used for address.");
        }
        return node;
    }
    return postfix();
}

// postfix = primary ( '[' expr ']' | '.' ident | "->" ident | "++" | "--" )*
Node *postfix() {
    Node *node = primary();
    for (;;) {
        if (consume("[")) {
            node = access(node, expr());
            expect("]");
            infof("finished until 'ident[ ]'(index access)");
        } else if (consume(".")) {
            // struct member access
            // x.a -> *( x + 'offset of a' ) (struxt x generates its address)
            Token *tok = expect_ident();
            assert_at(is_typ(node->typ, TP_STRUCT), token->str, "cannot use \".\" for not struct type variable.");
            Struct *st = node->typ->st;
            assert_at(st, token->str, "this struct isn't defined ever.");

            int offset = 0;
            Type *typ  = NULL;
            for (Member *now = st->mem; now; now = now->next) {
                if (now->len == tok->len && !memcmp(now->name, tok->str, tok->len)) {
                    offset = now->offset;
                    typ    = now->typ;
                }
            }

            assert_at(typ, token->str, "'%s' is not the member of struct '%s'", to_str(tok->str, tok->len), to_str(st->name, st->len));
            node = access_member(node, offset, typ);
        } else if (consume("->")) {
            // arrow operator
            // x->a -> (*x).a -> *( *x + 'offset of a')
            Token *tok = expect_ident();
            assert_at(is_typ(node->typ, TP_PTR), token->str, "cannot use '->' for not struct pointer type variable.");
            assert_at(is_typ(node->typ->ptr_to, TP_STRUCT), token->str, "cannot use '->' for not struct pointer type variable.");
            Struct *st = node->typ->ptr_to->st;

            int offset = 0;
            Type *typ  = NULL;
            for (Member *now = st->mem; now; now = now->next) {
                if (now->len == tok->len && !memcmp(now->name, tok->str, tok->len)) {
                    offset = now->offset;
                    typ    = now->typ;
                }
            }

            assert_at(typ, token->str, "'%s' is not the member of struct '%s'", to_str(tok->str, tok->len), to_str(st->name, st->len));
            node = new_node(ND_DEREF, node, NULL, node->typ->ptr_to);
            node = access_member(node, offset, typ);
        } else if (consume("++")) {
            node = add_helper(node, new_node_num(1), ND_INC);
        } else if (consume("--")) {
            node = add_helper(node, new_node_num(1), ND_DEC);
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
// otherwise return NULL
Node *primary() {
    Node *node;
    Token *tok;

    if (consume("(")) {
        if (consume("{")) { // statement expression
            enter_scope(false, false);
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

        node = node_obj(tok);
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