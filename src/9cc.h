#ifndef CC_H
#define CC_H

#include <stdio.h> 
#include <stdlib.h> 
#include <ctype.h> 
#include <stdarg.h> 
#include <stdbool.h> 
#include <string.h> 

char *user_input;

/*
    tokenize.c
*/

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

// token sequence
Token *token; 


void error_at(char *loc,char *fmt, ...);
Token *tokenize(char *p); 

/*
    parse.c
*/

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
    ND_RETURN, // "return"
    ND_IF,     // "if"
    ND_WHILE,  // "while" 
    ND_FOR,    // "for"
} NodeKind; 


// node 
typedef struct Node Node; 

struct Node {
    NodeKind kind; 
    Node *lhs;  // left-hand side
    Node *rhs;  // right-hand side
    int val;    // if kind is ND_NUM,its number
    int offset; // if kind is ND_LVAR,its offset from sp

    // "if" ( cond ) then "else" els
    // "while" ( cond ) then
    // "for" ( ini ";" cond ";" step ) then 
    Node *cond;
    Node *then;
    Node *els;
    Node *ini;
    Node *step;
};

Node *code[100]; // AST


void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul(); 
Node *unary();
Node *primary();

/*
    codegen.c
*/

void gen_statement(Node *node);

/*
    utils.c
*/

// log for debug
int lprintf(FILE *fp, int level, const char *file, int line, const char *func, const char *fmt, ...);
#define errorf(...) lprintf(stderr, 'E', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define warnf(...) lprintf(stderr, 'W', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define infof(...) lprintf(stderr, 'I', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define debugf(...) lprintf(stderr, 'D', __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif