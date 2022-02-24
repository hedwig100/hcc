#ifndef HCC_H
#define HCC_H

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
    ND_ADD,     // + 
    ND_SUB,     // - 
    ND_MUL,     // *
    ND_DIV,     // /
    ND_NUM,     // integer
    ND_EQ,      // ==
    ND_NEQ,     // != 
    ND_LT,      // < 
    ND_LEQ,     // <= 
    ND_LVAR,    // local variable
    ND_BLOCK,   // { }
    ND_CALLFUNC,// call func()
    ND_FUNCDEF, // definition of function
    ND_ASSIGN,  // assign
    ND_RETURN,  // "return"
    ND_IF,      // "if"
    ND_WHILE,   // "while" 
    ND_FOR,     // "for"
} NodeKind; 


// node 
typedef struct Node Node; 

struct Node {
    NodeKind kind; 
    Node *next;
    char *name; // if kind is ND_CALLFUNC,ND_FUNCDEF,its name
    int len;    // name's length

    int val;    // if kind is ND_NUM,its number
    int offset; // if kind is ND_LVAR,ND_FUNCDEF,its offset from sp

    // operator's left-hand side and right-hand side
    Node *lhs;  
    Node *rhs;

    // "if" ( cond ) then "else" els
    // "while" ( cond ) then
    // "for" ( ini ";" cond ";" step ) then 
    Node *cond;
    Node *then;
    Node *els;
    Node *ini;
    Node *step;

    // ND_BLOCK
    // { block }
    Node *block;

    // ND_CALLFUNC,ND_FUNCDEF
    // name(params),name(params) body
    Node *params;
    Node *body;
};

Node *code[100]; // AST


void program();
Node *cmp_stmt();
Node *func_def();
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

int align;

void gen_lval(Node *node);
int gen_param_set(Node *node);
void gen_param_get(Node *node);
void gen_expression(Node *node);
void gen_statement(Node *node);
void gen_func_def(Node *node);

/*
    utils.c
*/

// log for debug
int lprintf(FILE *fp, int level, const char *file, int line, const char *func, const char *fmt, ...);
char *to_str(char *s,int len);
#define errorf(...) lprintf(stderr, 'E', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define warnf(...) lprintf(stderr, 'W', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define infof(...) lprintf(stderr, 'I', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define debugf(...) lprintf(stderr, 'D', __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif