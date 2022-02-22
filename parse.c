#include "9cc.h" 

// error_at reports error
void error_at(char *loc,char *fmt, ...) {
    va_list ap;
    va_start(ap,fmt); 

    int pos = loc - user_input; 
    fprintf(stderr,"%s\n",user_input); 
    fprintf(stderr,"%*s",pos," "); 
    fprintf(stderr,"^ "); 
    vfprintf(stderr,fmt,ap); 
    fprintf(stderr,"\n");
    exit(1);
}

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
        error_at(token->str,"not '%c'"); 
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

bool at_eof() {
    return token->kind == TK_EOF;
}

// new_token create a new token and attach it to cur
Token *new_token(TokenKind kind,Token *cur,char *str) {
    Token* tok = calloc(1,sizeof(Token)); 
    tok->kind = kind; 
    tok->str = str; 
    cur->next = tok; 
    return tok; 
}

bool startwith(char *p,char *q) {
    return memcmp(p,q,strlen(q)) == 0;
}

Token *tokenize(char *p) {
    Token head; 
    head.next = NULL; 
    Token *cur = &head; 

    while (*p) {
        if (isspace(*p)) {
            p++; 
            continue; 
        }

        if (startwith(p,"==") || startwith(p,"!=") || startwith(p,"<=") || startwith(p,">=")) {
            cur = new_token(TK_RESERVED,cur,p); p+=2;
            cur->len = 2;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>' || *p == ';' || *p == '=') {
            cur = new_token(TK_RESERVED,cur,p++);
            cur->len = 1; 
            continue; 
        }

        if ('a' <= *p && *p <= 'z') {
            cur = new_token(TK_IDENT,cur,p++); 
            cur->len = 1; 
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM,cur,p); 
            cur->val = strtol(p,&p,10); 
            continue; 
        }

        error_at(token->str,"cannot tokenize");
    }

    new_token(TK_EOF,cur,p); 
    return head.next; 
} 

// construct AST

Node *new_node(NodeKind kind,Node* lhs,Node* rhs) {
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
    Node* node = expr(); 
    expect(";"); 
    return node;
}

Node *expr() {
    return assign();
}

Node *assign() {
    Node* node = equality(); 
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
    Token* tok = consume_ident(); 
    if (tok) {
        Node *node = calloc(1,sizeof(Node)); 
        node->kind = ND_LVAR;
        // fprintf(stderr,"%c\n",tok->str[0]);
        node->offset = (tok->str[0] - 'a' + 1)*8;
        return node;
    }

    return new_node_num(expect_number()); 
}