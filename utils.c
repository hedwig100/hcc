#include "9cc.h"

int lprintf(FILE *fp, int level, const char *file, int line, const char *func, const char *fmt, ...) {
    int n = 0;
    va_list ap;
    n += fprintf(fp, "[%c] %s: ", level, func);
    va_start(ap, fmt);
    n += vfprintf(fp, fmt, ap);
    va_end(ap);
    n += fprintf(fp, " (%s:%d)\n", file, line);
    return n;
}