#include "hcc.h"

bool startwith(char *p, char *q) { return memcmp(p, q, strlen(q)) == 0; }

bool is_alnum(char c) { return isalnum(c) || (c == '_'); }

int ident_len(char *p) {
    int len = 0;
    while (is_alnum(*p)) {
        len++;
        p++;
    }
    return len;
}

// new_token create a new token and attach it to cur
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind  = kind;
    tok->str   = str;
    cur->next  = tok;
    return tok;
}

Token *tokenize(char *p) {
    Token head;
    head.next  = NULL;
    Token *cur = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (startwith(p, "==") || startwith(p, "!=") || startwith(p, "<=") ||
            startwith(p, ">=")) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 2;
            p += 2;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
            *p == ')' || *p == '<' || *p == '>' || *p == ';' || *p == '=' ||
            *p == '{' || *p == '}' || *p == ',' || *p == '&') {
            cur      = new_token(TK_RESERVED, cur, p++);
            cur->len = 1;
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && !isalnum(p[6])) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 6;
            p += 6;
            continue;
        }

        if (strncmp(p, "if", 2) == 0 && !isalnum(p[2])) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 2;
            p += 2;
            continue;
        }

        if (strncmp(p, "else", 4) == 0 && !isalnum(p[4])) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 4;
            p += 4;
            continue;
        }

        if (strncmp(p, "while", 5) == 0 && !isalnum(p[5])) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 5;
            p += 5;
            continue;
        }

        if (strncmp(p, "for", 3) == 0 && !isalnum(p[3])) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 3;
            p += 3;
            continue;
        }

        if (strncmp(p, "int", 3) == 0 && !isalnum(p[3])) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 3;
            p += 3;
            continue;
        }

        if (isalpha(*p)) {
            cur      = new_token(TK_IDENT, cur, p);
            cur->len = ident_len(p);
            p += cur->len;
            continue;
        }

        if (isdigit(*p)) {
            cur      = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(token->str, "cannot tokenize");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}