#include <stdio.h> 
#include <stdlib.h> 
#include <ctype.h> 
#include <stdarg.h> 
#include <stdbool.h> 
#include <string.h> 


// tokenkind
typedef enum { // enum型
    TK_RESERVED, // operator
    TK_IDENT, // 識別子
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
    ND_ADD,  // + 
    ND_SUB,  // - 
    ND_MUL,  // *
    ND_DIV,  // /
    ND_NUM,  // 整数
    ND_EQ,   // ==
    ND_NEQ,  // != 
    ND_LT,   // < 
    ND_LEQ,  // <= 
    ND_LVAR, // ローカル変数
    ND_ASSIGN, // 代入
} NodeKind; 


// node 
typedef struct Node Node; 

struct Node {
    NodeKind kind; 
    Node* lhs; 
    Node* rhs; 
    int val; // kindが整数の時のみ利用
    int offset; // kindがND_LVARの時のみ利用
};

Node *code[100]; // astの配列

Node *new_node(NodeKind kind,Node* lhs,Node* rhs); 
Node *new_node_num(int val); 
void program(); 
Node* stmt(); 
Node* expr();
Node* assign();
Node* equality();
Node* relational();
Node* add();
Node* mul(); 
Node* unary();
Node* primary();

void error(char* msg);
void gen_lval(Node* node);
void gen(Node* node);