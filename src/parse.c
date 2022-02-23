#include "hcc.h" 

// consume read a token and return true if next token is expected kind of token,
// otherwise,return false 
bool consume(char *op) {
    if (token->kind != TK_RESERVED || 
        token->len != strlen(op) || 
        memcmp(token->str,op,token->len)) {
        return false; 
    }
    token = token->next;
    return true; 
}

// consume_ident read a token and return the token if next token is identifier,
// otherwise return NULL
Token* consume_ident() {
    if (token->kind != TK_IDENT) {
        return NULL; 
    }
    Token* tok = token;
    token = token->next;  
    return tok; 
}

// expect read a token if next token is expected kind of token,
// otherwise print error
void expect(char *op) {
    if (token->kind != TK_RESERVED || 
        token->len != strlen(op) || 
        memcmp(token->str,op,token->len)) {
        error_at(token->str,"not '%s'"); 
    }
    token = token->next; 
}

// expect_number read a token if next token is number,
// otherwise print error
int expect_number() {
    if (token->kind != TK_NUM) {
        error_at(token->str,"not number"); 
    }
    int val = token->val; 
    token = token->next; 
    return val; 
}

// find_lvar searches local variables,if exists return the local variable
// otherwise return NULL
LVar *find_lvar(Token *tok) {
    for (LVar *var = locals;var;var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str,var->name,var->len)) {
            return var;
        }
    }
    return NULL;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// construct AST

Node *new_node(NodeKind kind,Node *lhs,Node *rhs) {
    Node* node = calloc(1,sizeof(Node)); 
    node->kind = kind; 
    node->lhs = lhs; 
    node->rhs = rhs;
    return node; 
}

Node *new_node_num(int val) {
    Node* node = calloc(1,sizeof(Node)); 
    node->kind = ND_NUM; 
    node->val = val; 
    return node; 
}

void program() {
    int i = 0; 
    while (!at_eof()) {
        code[i++] = stmt(); 
    }
    code[i] = NULL;
}

Node *stmt() {
    Node* node;

    if (consume("if")) {
        expect("(");
        node = calloc(1,sizeof(Node));
        node->kind = ND_IF;
        node->cond = expr();
        expect(")");
        node->then = stmt();
        
        if (consume("else")) {
            node->els = stmt();
        }

        return node;
    } else if (consume("return")) {
        node = calloc(1,sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
        expect(";");
        return node;
    } else if (consume("while")) {
        expect("(");
        node = calloc(1,sizeof(Node));
        node->kind = ND_WHILE;
        node->cond = expr();
        expect(")");
        node->then = stmt();
        return node;
    } else if (consume("for")) {
        expect("(");
        node = calloc(1,sizeof(Node));
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
        node = calloc(1,sizeof(Node));
        node->kind = ND_BLOCK;
        Node *end = node;
        while (!consume("}")) {
            end->next = stmt();
            end = end->next;
        }
        return node;
    }
    
    node = expr();
    expect(";"); 
    return node;
}

Node *expr() {
    return assign();
}

Node *assign() {
    Node *node = equality(); 
    if (consume("=")) {
        node = new_node(ND_ASSIGN,node,assign()); 
    }
    return node;
}

Node *equality() {
    Node* node = relational(); 

    for (;;) {
        if (consume("==")) {
            node = new_node(ND_EQ,node,relational());
        } else if (consume("!=")) {
            node = new_node(ND_NEQ,node,relational()); 
        } else {
            return node;
        }
    }
}

Node *relational() {
    Node* node = add(); 

    for (;;) {
        if (consume("<")) {
            node = new_node(ND_LT,node,add()); 
        } else if (consume("<=")) {
            node = new_node(ND_LEQ,node,add()); 
        } else if (consume(">")) {
            node = new_node(ND_LT,add(),node); 
        } else if (consume(">=")) {
            node = new_node(ND_LEQ,add(),node); 
        } else {
            return node; 
        }
    }
}

Node *add() {
    Node *node = mul(); 

    for (;;) {
        if (consume("+")) {
            node = new_node(ND_ADD,node,mul()); 
        } else if (consume("-")) {
            node = new_node(ND_SUB,node,mul()); 
        } else {
            return node;
        }
    }
}

Node *mul() {
    Node *node = unary(); 

    for (;;) {
        if (consume("*")) {
            node = new_node(ND_MUL,node,unary()); 
        } else if (consume("/")) {
            node = new_node(ND_DIV,node,unary()); 
        } else {
            return node; 
        }
    } 
}

Node *unary() {
    if (consume("+")) {
        return primary(); 
    } 
    if (consume("-")) {
        return new_node(ND_SUB,new_node_num(0),primary()); 
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
        Node *node = calloc(1,sizeof(Node)); 
        if (consume("(")) { // function call
            expect(")");
            node->kind = ND_CALLFUNC;
            node->name = tok->str;
            node->len = tok->len;
            return node;
        }
        node->kind = ND_LVAR;
        
        LVar *lvar = find_lvar(tok);
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            LVar *lvar = calloc(1,sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = locals->offset + 8;
            node->offset = lvar->offset;
            locals = lvar;
        }
        return node;
    }

    return new_node_num(expect_number()); 
}