#include <stdio.h> 
#include <stdlib.h> 
#include <ctype.h> 
#include <stdarg.h> 
#include <stdbool.h> 
#include <string.h> 


// tokenkind
typedef enum {  
    TK_RESERVED, // operator
    TK_IDENT,    // identifier
    TK_NUM,      // integer 
    TK_EOF,      // end of input
} TokenKind; 


// token 
typedef struct Token Token; 

struct Token {
    TokenKind kind; // token kind 
    Token *next;    // next input token 
    int val;        // if kind is TK_NUM,its number
    char *str;      // token string 
    int len; 
}; 

Token *token; 


// local variable
typedef struct LVar LVar;

struct LVar {
    LVar *next; // next local variable if exists,otherwise NULL
    char *name; // local variable name
    int len;    // local variable length
    int offset; // offset from RBP
}; 

// local variables
LVar *locals;


char *user_input;

void error_at(char *loc,char *fmt, ...);
bool consume(char *op); 
void expect(char *op); 
int expect_number(); 
LVar *find_lvar(Token* tok);
bool at_eof(); 
Token *new_token(TokenKind kind,Token *cur,char *str); 
bool startwith(char *p,char *q); 
Token *tokenize(char *p); 


// nodekind
typedef enum {
    ND_ADD,    // + 
    ND_SUB,    // - 
    ND_MUL,    // *
    ND_DIV,    // /
    ND_NUM,    // integer
    ND_EQ,     // ==
    ND_NEQ,    // != 
    ND_LT,     // < 
    ND_LEQ,    // <= 
    ND_LVAR,   // local variable
    ND_ASSIGN, // assign
} NodeKind; 


// node 
typedef struct Node Node; 

struct Node {
    NodeKind kind; 
    Node* lhs;  // left-hand side
    Node* rhs;  // right-hand side
    int val;    // if kind is ND_NUM,its number
    int offset; // if kind is ND_LVAR,its offset from sp
};

Node *code[100]; // AST

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