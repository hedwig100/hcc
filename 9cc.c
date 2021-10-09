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

void error(char *fmt, ...) {
    va_list ap; // 可変引数
    va_start(ap,fmt); 
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
bool expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) {
        error("'%c'ではありません"); 
    }
    token = token->next; 
}

// 次のトークンが数値の場合､トークンを一つ読み進めてその数値を返す
// それ以外の場合はエラーを報告する

int expect_number() {
    if (token->kind != TK_NUM) {
        error("数ではありません"); 
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

        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED,cur,p++); 
            continue; 
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM,cur,p); 
            cur->val = strtol(p,&p,10); 
            continue; 
        }

        error("cannot tokenize");
    }

    new_token(TK_EOF,cur,p); 
    return head.next; 
}

int main(int argc,char **argv) {
    if (argc != 2) {
        fprintf(stderr,"number of args is not valid"); 
        return 1; 
    }

    token = tokenize(argv[1]);  

    printf(".intel_syntax noprefix\n"); 
    printf(".globl main\n"); 
    printf("main:\n");

    // 最初は数である必要がある
    printf("    mov rax, %d\n",expect_number()); 

    while (!at_eof()) {
        if (consume('+')) {
            printf("    add rax, %d\n",expect_number()); 
            continue; 
        }

        expect('-'); 
        printf("    sub rax, %d\n",expect_number()); 
    }

    printf("    ret\n");
    return 0;
}