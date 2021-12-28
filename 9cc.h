#include <stdio.h> 
#include <stdlib.h> 
#include <ctype.h> 
#include <stdarg.h> 
#include <stdbool.h> 
#include <string.h> 


// tokenkind
typedef enum { // enum型
    TK_RESERVED, // operator
    TK_NUM, // integer 
    TK_EOF, // end of input
} TokenKind; 


// token 
typedef struct Token Token; 

struct Token {
    TokenKind kind; // token kind 
    Token *next; // next input token 
    int val; // if kind is TK_NUM,number
    char *str; // token string 
    int len; 
}; 

Token *token; 
char *user_input; 

void error_at(char *loc,char *fmt, ...);
bool consume(char *op); 
void expect(char *op); 
int expect_number(); 
bool at_eof(); 
Token *new_token(TokenKind kind,Token *cur,char *str); 
bool startwith(char *p,char *q); 
Token *tokenize(char *p); 


// nodekind
typedef enum {
    ND_ADD, 
    ND_SUB, 
    ND_MUL,
    ND_DIV, 
    ND_NUM, 
    ND_EQ,
    ND_NEQ,
    ND_LT, 
    ND_LEQ,
} NodeKind; 


// node 
typedef struct Node Node; 

struct Node {
    NodeKind kind; 
    Node* lhs; 
    Node* rhs; 
    int val; 
};

Node *new_node(NodeKind kind,Node* lhs,Node* rhs); 
Node *new_node_num(int val); 
Node* expr();
Node* equality();
Node* relational();
Node* add();
Node* mul(); 
Node* unary();
Node* primary();


void gen(Node* node);