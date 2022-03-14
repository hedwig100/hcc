#ifndef HCC_H
#define HCC_H

#if _HOST
#include "help.h"
#define stderr err_file
#else
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) > (b) ? (b) : (a))

char *user_input;
char *filename;

typedef struct Token Token;
typedef struct Type Type;
typedef struct Func Func;
typedef struct Object Object;
typedef struct Str Str;
typedef struct Scope Scope;
typedef struct Member Member;
typedef struct Struct Struct;

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
struct Token {
    TokenKind kind; // token kind
    Token *next;    // next input token
    int val;        // if kind is TK_NUM,its number
    char *str;      // token string
    int len;
    int type_len;
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
    TP_VOID,   // void
    TP_PTR,    // pointer
    TP_ARRAY,  // array
    TP_STRUCT, // struct
    TP_ENUM,   // enum
} TypeKind;

// type
struct Type {
    TypeKind kind;
    int size;
    bool is_typdef;
    bool is_static;

    // ptr_to is valid when kind = TP_PTR.
    Type *ptr_to;
    int array_size;

    // if kind = TP_STRUCT or TP_ENUM
    char *name;
    int len;
    Type *mem;
    Type *next;
    int offset;
    Struct *st;
};

Type *new_type(TypeKind kind);

// function
struct Func {
    Func *next;
    char *name;
    int len;

    Type *ret;
    int param_num;
    bool is_varargs;
    Type *params;
};

Func *funcs;

// Object kind
typedef enum {
    OBJ_LVAR,
    OBJ_GVAR,
    OBJ_STRUCT,
    OBJ_ENUM,
    OBJ_TYPEDEF,
} ObjectKind;

// Object
struct Object {
    ObjectKind kind;
    Object *next;
    char *name;
    int len;

    // lvar,gvar
    int offset;
    Type *typ;
    bool is_static;

    // enum
    Object *enum_list;
    int val;
    bool is_init;
};

// global variables
Object *globals;

// string literal
struct Str {
    Str *next;
    char *val;
    int len;
    int id;
};

Str *strs;

// Scope
struct Scope {
    Scope *next;
    Scope *before;
    Object *lvar;
    Object *en;
    Object *td;
    Struct *strct;

    // maximal offset in this scope
    int offset;
    int stack_offset;

    bool can_break; // truee if you can break
    bool can_cont;  // true if you can continue
    int label;
    int named_param; // in function definition its number of named parameter
};

Scope *scopes;

// Member
struct Member {
    Member *next;
    char *name;
    int len;
    Type *typ;
    int offset;
};

// Struct
struct Struct {
    Struct *next;
    char *name;
    int len;
    Member *mem;
    int size;
    Type *typ;
    bool is_defined;
};

// for function parameter

typedef enum {
    GLOBAL,
    LOCAL,
    STRUCT,
    NEST_TYPE,
    PARAM_STACK,
} Param;

// nodekind
typedef enum {
    ND_ADD,      // +
    ND_SUB,      // -
    ND_MUL,      // *
    ND_DIV,      // /
    ND_MOD,      // %
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
    ND_BREAK,    // "break"
    ND_CONTINUE, // "continue"
    ND_ADDR,     // &
    ND_DEREF,    // *
    ND_STMTEXPR, // "({" block "})"
    ND_INIT,     // initializer
    ND_ARRAY,    // initializer list
    ND_MEMBER,   // struct member
    ND_SWITCH,   // "switch"
    ND_CASE,     // "case"
    ND_DEFAULT,  // "default"
    ND_TYPEDEF,  // typdef
    ND_INC,      // ident++
    ND_DEC,      // ident--
    ND_AND,      // &
    ND_XOR,      // ^
    ND_OR,       // |
    ND_TERNARY,  // cond '?' then ':' else
    ND_LSHIFT,   // <<
    ND_RSHIFT,   // >>
    ND_QUESTION, // !
    ND_DOWHILE,  // do while
    ND_VASTART,  // va_start
} NodeKind;

// node
typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *next;
    char *name; // if kind is ND_CALLFUNC,ND_FUNCDEF,its name
    int len;    // name's length

    int val;       // if kind is ND_NUM,its number
    int id;        // if kind is ND_STR,its id
    int type_size; // if kind is ND_STR,its length
    int offset;    // if kind is ND_LVAR,ND_FUNCDEFits offset from sp, ND_DOT its offset
    Type *typ;     // when kind is ND_LVAR,ND_FUNCDEF,ND_FUNCCALL its type(or return type)
    int label;     // used when kind is ND_WHEN,ND_WHILE

    // operator's left-hand side and right-hand side
    Node *lhs;
    Node *rhs;

    // "if" ( cond ) then "else" els
    // "while" ( cond ) then
    // "do" then "while" '(' cond ')' ';'
    // "for" ( ini ";" cond ";" step ) then
    // "switch" '(' cond ')' block
    // "case" cond ':' block
    // "default" ':' block
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
    int param_num;
    bool is_varargs;
    Node *body;

    // ND_ARRAY
    Node *initlist;

    // if (ND_LVAR or ND_GVAR) and TP_STRUCT
    Struct *st;

    // ND_VASTART
    int named_param;
};

Node *code[1000]; // AST
Node *static_datas;

/*
    parse_utils.c
*/

Type *new_type(TypeKind kind);
Type *new_type_ptr(Type *ptr_to);
Type *new_type_strct(Token *tok, Member *mem);
Type *declare_type_strct(Token *tok);
Type *define_type_strct(Struct *st, Member *mem);
Type *new_type_enum(Object *en);
Type *type_copy(Type *org);
int byte_align(Type *typ);
Type *can_assign(Type *typ1, Type *typ2);
Type *can_add(Type *typ1, Type *typ2);
bool is_ptr(Type *typ);
bool is_typ(Type *typ, TypeKind kind);
int type_size(Type *typ);
Node *new_typdef(Token *tok, Type *typ);
bool lookahead_typdef(Token *tok);
Object *find_typdef(Token *tok);

void register_func(Node *node);
Func *find_func(Node *node);

Object *find_obj(Token *tok);
bool can_defined_lvar(Token *tok);

Object *find_gvar(Token *tok);
Node *register_static_data(Node *node);

void enter_scope(bool can_break, bool can_cont);
void out_scope();
int calc_aligment_offset(int min_offset, int alignment);
void add_offset(Scope *scope, int size, int alignment);
int calc_offset(Node *node, int min_offset);

Node *eval_const(Node *node);
Node *eval(Node *lhs, Node *rhs);

Node *add_helper(Node *lhs, Node *rhs, NodeKind kind);
Node *access(Node *ptr, Node *expr);
Node *access_member(Node *expr, int offset, Type *typ);
Struct *find_struct(char *name, int len);
bool can_define_strct(Token *tok);
Object *new_object(ObjectKind kind);
Object *find_enum(Token *tok);

/*
    parse.c
*/

Node *new_node(NodeKind kind, Node *lhs, Node *rhs, Type *typ);
Node *new_node_num(int val);
Node *new_node_lvar(Token *tok, Type *typ);
Node *node_obj(Token *tok);
Node *new_node_gvar(Token *tok, Type *typ);
Node *node_gvar(Token *tok);
Node *new_node_str(Token *tok);
Node *new_node_mem(Token *tok, Type *typ);
Member *new_mem(Node *node);

void program();
Node *ext_def();
Type *decl_spec();
Type *type_spec();
Type *struct_spec();
Member *struct_decl();
Type *enum_spec();
Object *enumerator();
Node *declarator(Type *typ, Param p);
Type *pointer(Type *typ);
Node *direct_decl(Type *typ, Param p);
Type *type_array(Type *typ);
Node *func_param(Node *node);
Node *initializer(bool constant);
Node *initializer_list(bool is_constant);
Node *cmp_stmt();
Node *labeled_stmt();
Node *stmt();
Node *declaration();
Node *init_decl(Type *typ);
Node *expr();
Node *assign();
Node *constant();
Node *conditional();
Node *log_or();
Node *log_and();
Node *or_expr();
Node *xor_expr();
Node *and_expr();
Node *equality();
Node *relational();
Node *shift();
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

FILE *err_file;
// NOTE: err_file should be stderr, however prepare this variable for not using stdio.h
//       should change stderr after this compiler can parse stdio.h
#define errorf(...) \
    lprintf(stderr, 'E', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define warnf(...) \
    lprintf(stderr, 'W', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define infof(...) \
    lprintf(stderr, 'I', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define debugf(...) \
    lprintf(stderr, 'D', __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif