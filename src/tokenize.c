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

char read_escape_char(char *p) {
    switch (*p) {
    case 'a':
        return '\a';
    case 'b':
        return '\b';
    case 't':
        return '\t';
    case 'n':
        return '\n';
    case 'v':
        return '\v';
    case 'f':
        return '\f';
    case 'r':
        return '\r';
    case 'e':
        return 27;
    case '0':
        return 0;
    default:
        return *p;
    }
}

char *read_string_end(char *start) {
    char *p = start + 1;
    while ((*p) != '"') {
        if (*p == '\0' || *p == '\n') {
            error_at(p, "missing terminationg \" character ");
        }
        if (*p == '\\') {
            p++;
        }
        p++;
    }
    return p;
}

int count_escase(char *start) {
    char *p = start + 1;
    int ans = 0;
    while ((*p) != '"') {
        if (*p == '\0' || *p == '\n') {
            error_at(p, "missing terminationg \" character ");
        }
        if (*p == '\\') {
            p++;
            ans++;
        }
        p++;
    }
    return ans;
}

Token *read_string(Token *cur, char *p) {
    Token *tok    = calloc(1, sizeof(Token));
    tok->kind     = TK_STR;
    tok->str      = p;
    char *start   = p;
    char *end     = read_string_end(p);
    tok->len      = end - start + 1;
    tok->str      = p;
    tok->type_len = tok->len - 2 - count_escase(p);
    cur->next     = tok;
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

        if (strncmp(p, "//", 2) == 0) {
            p += 2;
            while (*p != '\n') {
                p++;
            }
            continue;
        }

        if (strncmp(p, "/*", 2) == 0) {
            char *q = strstr(p + 2, "*/");
            if (!q) {
                error_at(p, "comment not closed.");
            }
            p = q + 2;
            continue;
        }

        if (startwith(p, "==") || startwith(p, "!=") || startwith(p, "<=") ||
            startwith(p, ">=") || startwith(p, "->") || startwith(p, "++") ||
            startwith(p, "--")) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 2;
            p += 2;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
            *p == ')' || *p == '<' || *p == '>' || *p == ';' || *p == '=' ||
            *p == '{' || *p == '}' || *p == ',' || *p == '&' || *p == '[' ||
            *p == ']' || *p == '.' || *p == ':') {
            cur      = new_token(TK_RESERVED, cur, p++);
            cur->len = 1;
            continue;
        }

        if (*p == '\'') {
            cur = new_token(TK_NUM, cur, ++p);
            if (*p == '\\') {
                cur->val = read_escape_char(++p);
            } else {
                cur->val = *p - '\0';
            }
            assert_at(*(++p) == '\'', cur->str, "char literal must be 1-character.");
            p++;
            continue;
        }

        if (*p == '"') {
            cur = read_string(cur, p);
            p += cur->len;
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && !isalnum(p[6])) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 6;
            p += 6;
            continue;
        }

        if (strncmp(p, "struct", 6) == 0 && !isalnum(p[6])) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 6;
            p += 6;
            continue;
        }

        if (strncmp(p, "switch", 6) == 0 && !isalnum(p[6])) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 6;
            p += 6;
            continue;
        }

        if (strncmp(p, "continue", 8) == 0 && !isalnum(p[8])) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 8;
            p += 8;
            continue;
        }

        if (strncmp(p, "default", 7) == 0 && !isalnum(p[7])) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 7;
            p += 7;
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

        if (strncmp(p, "void", 4) == 0 && !isalnum(p[4])) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 4;
            p += 4;
            continue;
        }

        if (strncmp(p, "enum", 4) == 0 && !isalnum(p[4])) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 4;
            p += 4;
            continue;
        }

        if (strncmp(p, "case", 4) == 0 && !isalnum(p[4])) {
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

        if (strncmp(p, "break", 5) == 0 && !isalnum(p[5])) {
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

        if (strncmp(p, "sizeof", 6) == 0 && !isalnum(p[6])) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 6;
            p += 6;
            continue;
        }

        if (strncmp(p, "char", 4) == 0 && !isalnum(p[4])) {
            cur      = new_token(TK_RESERVED, cur, p);
            cur->len = 4;
            p += 4;
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

        error_at(cur->str, "cannot tokenize");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}