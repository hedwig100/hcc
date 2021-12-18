#include <stdio.h> 
#include <stdlib.h> 
#include <ctype.h> 
#include <stdarg.h> 
#include <stdbool.h> 
#include <string.h> 

typedef enum { // enum型
    TK_RESERVED, // operator
    TK_NUM, // integer 
    TK_EOF, // end of input
} TokenKind; 

typedef struct Token Token; 

struct Token {
    TokenKind kind; // token kind 
    Token *next; // next input token 
    int val; // if kind is TK_NUM,number
    char *str; // token string 
}; 

Token *token; 
char *user_input; 

void error_at(char *loc,char *fmt, ...) {
    va_list ap; // 可変引数
    va_start(ap,fmt); 

    int pos = loc - user_input; 
    fprintf(stderr,"%s\n",user_input); 
    fprintf(stderr,"%*s",pos," "); 
    fprintf(stderr,"^ "); 
    vfprintf(stderr,fmt,ap); 
    fprintf(stderr,"\n");
    exit(1);
}

// 次のトークンが期待している記号の時にはトークンを一つ読み進めて真を返す。それ以外の場合は偽を返す
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) {
        return false; 
    }
    token = token->next;
    return true; 
}

// 次のトークンが期待している記号の時はトークンを一つ読み進める
// それ以外の場合はエラーを報告する
void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) {
        error_at(token->str,"'%c'ではありません"); 
    }
    token = token->next; 
}

// 次のトークンが数値の場合､トークンを一つ読み進めてその数値を返す
// それ以外の場合はエラーを報告する

int expect_number() {
    if (token->kind != TK_NUM) {
        error_at(token->str,"数ではありません"); 
    }
    int val = token->val; 
    token = token->next; 
    return val; 
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurにつなげる
Token *new_token(TokenKind kind,Token *cur,char *str) {
    Token* tok = calloc(1,sizeof(Token)); 
    tok->kind = kind; 
    tok->str = str; 
    cur->next = tok; 
    return tok; 
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

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
            cur = new_token(TK_RESERVED,cur,p++); 
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

typedef enum {
    ND_ADD, 
    ND_SUB, 
    ND_MUL,
    ND_DIV, 
    ND_NUM, 
} NodeKind; 

typedef struct Node Node; 

struct Node {
    NodeKind kind; 
    Node* lhs; 
    Node* rhs; 
    int val; 
}; 

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


Node* expr(); 
Node* mul(); 
Node* primary(); 

Node *expr() {
    Node *node = mul();
    
    for (;;) {
        if (consume('+')) {
            node = new_node(ND_ADD,node,mul()); 
        } else if (consume('-')) {
            node = new_node(ND_SUB,node,mul()); 
        } else {
            return node; 
        }
    }
}

Node *mul() {
    Node *node = primary(); 

    for (;;) {
        if (consume('*')) {
            node = new_node(ND_MUL,node,primary()); 
        } else if (consume('/')) {
            node = new_node(ND_DIV,node,primary()); 
        } else {
            return node; 
        }
    } 
}

Node *primary() {
    // 次のトークンが'('なら'(' expr ')'のはず
    if (consume('(')) {
        Node *node = expr();
        expect(')'); 
        return node; 
    } 
    // そうでなければ数値のはず
    return new_node_num(expect_number()); 
}

void gen(Node* node) {
    if (node->kind == ND_NUM) {
        printf("    push %d\n",node->val); 
        return; 
    }

    gen(node->lhs); 
    gen(node->rhs); 

    printf("  pop rdi\n"); 
    printf("  pop rax\n");

    switch (node->kind) {
        case ND_ADD:
            printf("    add rax,rdi\n"); 
            break; 
        case ND_SUB: 
            printf("    sub rax,rdi\n"); 
            break; 
        case ND_MUL:
            printf("    imul rax,rdi\n"); 
            break; 
        case ND_DIV:
            printf("    cqo\n"); 
            printf("    idiv rdi\n"); 
            break; 
    }

    printf("  push rax\n"); 
}

int main(int argc,char **argv) {
    if (argc != 2) {
        fprintf(stderr,"number of args is not valid"); 
        return 1; 
    }

    user_input = argv[1];
    token = tokenize(user_input);
    Node *node = expr();

    printf(".intel_syntax noprefix\n"); 
    printf(".globl main\n"); 
    printf("main:\n");

    gen(node); 

    printf("    pop rax\n"); 
    printf("    ret\n");
    return 0;
}