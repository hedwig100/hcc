#ifndef HCC_H
#define HCC_H

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *user_input;
char *filename;

/*
    tokenize.c
*/

// tokenkind
typedef enum {
    TK_RESERVED, // operator
    TK_IDENT,    // identifier
    TK_NUM,      // integer
    TK_STR,      // string literal
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
    TP_INT,    // int
    TP_CHAR,   // char
    TP_PTR,    // pointer
    TP_ARRAY,  // array
    TP_STRUCT, // struct
} TypeKind;

// type

typedef struct Type Type;

struct Type {
    TypeKind kind;
    int size;

    // ptr_to is valid when kind = TP_PTR.
    Type *ptr_to;
    size_t array_size;

    Type *mem;
    Type *next;
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

// global variable
typedef struct GVar GVar;

struct GVar {
    GVar *next; // next glocal variable if exists,otherwise NULL
    char *name; // local variable name
    int len;    // local variable length
    Type *typ;
};

// global variables
GVar *globals;

// string literal
typedef struct Str Str;

struct Str {
    Str *next;
    char *val;
    int len;
    int id;
};

Str *strs;

// Scope
typedef struct Scope Scope;

struct Scope {
    Scope *next;
    Scope *before;
    LVar *lvar;

    // maximal offset in this scope
    int offset;
};

Scope *scopes;

// Member
typedef struct Member Member;

struct Member {
    Member *next;
    char *name;
    int len;
    Type *typ;
};

// Struct
typedef struct Struct Struct;

struct Struct {
    Struct *next;
    char *name;
    int len;
    Member *mem;
};

Struct *strcts;

// for function parameter

typedef enum {
    GLOBAL,
    LOCAL,
    STRUCT,
} Param;

// nodekind
typedef enum {
    ND_ADD,      // +
    ND_SUB,      // -
    ND_MUL,      // *
    ND_DIV,      // /
    ND_NUM,      // integer
    ND_STR,      // string literal
    ND_EQ,       // ==
    ND_NEQ,      // !=
    ND_LT,       // <
    ND_LEQ,      // <=
    ND_LVAR,     // local variable
    ND_GVAR,     // global variable
    ND_BLOCK,    // { }
    ND_CALLFUNC, // call func()
    ND_FUNCDEF,  // definition of function
    ND_FUNCDECL, // declaration of function
    ND_ASSIGN,   // assign
    ND_RETURN,   // "return"
    ND_IF,       // "if"
    ND_WHILE,    // "while"
    ND_FOR,      // "for"
    ND_ADDR,     // &
    ND_DEREF,    // *
    ND_STMTEXPR, // "({" block "})"
    ND_INIT,     // initializer
    ND_ARRAY,    // initializer list
    ND_MEMBER,   // struct member
} NodeKind;

// node
typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *next;
    char *name; // if kind is ND_CALLFUNC,ND_FUNCDEF,its name
    int len;    // name's length

    int val;    // if kind is ND_NUM,its number
    int id;     // if kind is ND_STR,its id
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

    // ND_ARRAY
    Node *initlist;
};

Node *code[100]; // AST

/*
    parse_utils.c
*/

Type *new_type(TypeKind kind);
Type *new_type_ptr(Type *ptr_to);
Type *can_assign(Type *typ1, Type *typ2);
Type *can_add(Type *typ1, Type *typ2);
bool is_ptr(Type *typ);
bool is_typ(Type *typ, TypeKind kind);
int type_size(Type *typ);

void register_func(Node *node);
Func *find_func(Node *node);

LVar *find_lvar(Token *tok);
bool can_defined_lvar(Token *tok);

GVar *find_gvar(Token *tok);

void enter_scope();
void out_scope();
void add_offset(Scope *scope, int size);

Node *eval_const(Node *node);
Node *eval(Node *lhs, Node *rhs);

Node *add_helper(Node *lhs, Node *rhs, NodeKind kind);
Node *access(Node *ptr, Node *expr);

/*
    parse.c
*/

Node *new_node(NodeKind kind, Node *lhs, Node *rhs, Type *typ);
Node *new_node_num(int val);
Node *new_node_lvar(Token *tok, Type *typ);
Node *node_lvar(Token *tok);
Node *new_node_gvar(Token *tok, Type *typ);
Node *node_gvar(Token *tok);
Node *new_node_str(Token *tok);
Node *new_node_mem(Token *tok, Type *typ);
Member *new_mem(Node *node);
Type *new_type_strct(Token *tok, Member *mem);

void program();
Node *ext_def();
Type *decl_spec();
Type *struct_spec();
Member *struct_decl();
Node *declarator(Type *typ, Param p);
Type *pointer(Type *typ);
Node *direct_decl(Type *typ, Param p);
Type *type_array(Type *typ);
Node *func_param(Node *node);
Node *initializer(bool constant);
Node *initializer_list(bool is_constant);
Node *cmp_stmt();
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
void gen_ext_def(Node *node);
void gen_program();

/*
    utils.c
*/

// log for debug
int lprintf(FILE *fp, int level, const char *file, int line, const char *func,
            const char *fmt, ...);
void assert_at(bool cond, char *loc, char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
char *to_str(char *s, int len);
void input_from_stdin();
char *read_file(char *path);

#define errorf(...) \
    lprintf(stderr, 'E', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define warnf(...) \
    lprintf(stderr, 'W', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define infof(...) \
    lprintf(stderr, 'I', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define debugf(...) \
    lprintf(stderr, 'D', __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif