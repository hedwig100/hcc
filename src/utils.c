#include "hcc.h"

char *add_quote(char *name, int len) {
    char *buf = calloc(1, sizeof(len + 2));
    buf[0]    = '"';
    memcpy(buf + 1, name, len);
    buf[len + 1] = '"';
    return buf;
}

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

void assert_at(bool cond, char *loc, char *fmt, ...) {
    if (cond) return;
    va_list ap;
    va_start(ap, fmt);

    char *line = loc;
    while (user_input < line && line[-1] != '\n')
        line--;

    char *end = loc;
    while (*end != '\n' && *end != '\0')
        end++;

    int line_num = 1;
    for (char *p = user_input; p < line; p++) {
        if (*p == '\n') line_num++;
    }

    int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
    fprintf(stderr, "%.*s\n", (int)(end - line), line);
    int pos = loc - line + indent;
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// error_at reports error
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    char *line = loc;
    while (user_input < line && line[-1] != '\n')
        line--;

    char *end = loc;
    while (*end != '\n' && *end != '\0')
        end++;

    int line_num = 1;
    for (char *p = user_input; p < line; p++) {
        if (*p == '\n') line_num++;
    }

    int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
    fprintf(stderr, "%.*s\n", (int)(end - line), line);
    int pos = loc - line + indent;
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

char *read_file(char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        errorf("cannot open %s: %s", path, strerror(errno));
    }

    // check file length
    if (fseek(fp, 0, SEEK_END) == -1)
        errorf("%s: fseek: %s", path, strerror(errno));
    int size = ftell(fp);
    if (fseek(fp, 0, SEEK_SET) == -1)
        errorf("%s: fseek: %s", path, strerror(errno));

    // read file
    char *buf = calloc(1, size + 2);
    fread(buf, size, 1, fp);
    if (size == 0 || buf[size - 1] != '\n')
        buf[size++] = '\n';
    buf[size] = '\0';
    fclose(fp);
    return buf;
}