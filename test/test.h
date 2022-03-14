#define ASSERT(x, y) assert(x, y, #y)

struct __builtin_myva {
    int gp_offset;
    int fp_offset;
    void *overflow_arg_area;
    void *reg_save_area;
};
typedef struct __builtin_myva __builtin_myva_list[1];
typedef __builtin_myva_list __gnuc_va_list;
typedef __gnuc_va_list va_list;
#define va_start(v, l) __builtin_myva_start(v, l)
#define __builtin_myva_end(v) 0
#define va_end(v) __builtin_myva_end(v)

struct _IO_FILE;
typedef struct _IO_FILE FILE;
FILE *stderr;
FILE *fopen(const char *__filename, const char *__modes);
int vfprintf(FILE *__s, const char *__format, __gnuc_va_list __arg);
void *calloc(int __nmemb, int __size);

int assert(int expected, int actual, char *code);
int ok();
int printp(int *a);
int print8(int a, int b, int c, int d, int e, int f, int g, int h);
int printf(char *fmt, ...);