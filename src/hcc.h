#ifndef HCC_H
#define HCC_H

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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

Token *tokenize(char *p);

/*
    for parser
*/

// typekind

typedef enum {
    TP_INT,
    TP_PTR,
    TP_ARRAY,
} TypeKind;

// type

typedef struct Type Type;

struct Type {
    TypeKind kind;

    // ptr_to is valid when kind = TP_PTR.
    Type *ptr_to;
    int size;
    size_t array_size;
};

Type *new_type(TypeKind kind);

// function
typedef struct Func Func;

struct Func {
    Func *next;
    char *name;
    int len;

    Type *ret;
    Type *params[6];
};

Func *funcs;

// local variable
typedef struct LVar LVar;

struct LVar {
    LVar *next; // next local variable if exists,otherwise NULL
    char *name; // local variable name
    int len;    // local variable length
    int offset; // offset from RBP
    Type *typ;
};

// local variables
LVar *locals;

// nodekind
typedef enum {
    ND_ADD,      // +
    ND_SUB,      // -
    ND_MUL,      // *
    ND_DIV,      // /
    ND_NUM,      // integer
    ND_EQ,       // ==
    ND_NEQ,      // !=
    ND_LT,       // <
    ND_LEQ,      // <=
    ND_LVAR,     // local variable
    ND_BLOCK,    // { }
    ND_CALLFUNC, // call func()
    ND_FUNCDEF,  // definition of function
    ND_ASSIGN,   // assign
    ND_RETURN,   // "return"
    ND_IF,       // "if"
    ND_WHILE,    // "while"
    ND_FOR,      // "for"
    ND_ADDR,     // &
    ND_DEREF,    // *
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
    Type *typ;  // when kind is ND_LVAR,ND_FUNCDEF,ND_FUNCCALL its type(or return type)

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

/*
    parse_utils.c
*/

Type *new_type(TypeKind kind);
bool type_cmp(Type *typ1, Type *typ2);
bool is_ptr(Type *typ);
Type *can_add(Type *typ1, Type *typ2);
void register_func(Node *node);
Func *find_func(Node *node);
LVar *find_lvar(Token *tok);

/*
    parse.c
*/

void program();
Node *cmp_stmt();
Type *type_declare();
Type *type_array(Type *typ);
Node *func_def();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *postfix();
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
int lprintf(FILE *fp, int level, const char *file, int line, const char *func,
            const char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
char *to_str(char *s, int len);

#define errorf(...) \
    lprintf(stderr, 'E', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define warnf(...) \
    lprintf(stderr, 'W', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define infof(...) \
    lprintf(stderr, 'I', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define debugf(...) \
    lprintf(stderr, 'D', __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif