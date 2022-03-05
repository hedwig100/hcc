#include "hcc.h"

int lprintf(FILE *fp, int level, const char *file, int line, const char *func,
            const char *fmt, ...) {
    int n = 0;
    va_list ap;
    n += fprintf(fp, "[%c] %s: ", level, func);
    va_start(ap, fmt);
    n += vfprintf(fp, fmt, ap);
    va_end(ap);
    n += fprintf(fp, " (%s:%d)\n", file, line);
    if (level == 'E') exit(1);
    return n;
}

// error_at reports error
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

char *to_str(char *s, int len) {
    char *ret = calloc(len + 1, sizeof(char));
    memcpy(ret, s, len);
    ret[len] = '\0';
    return ret;
}