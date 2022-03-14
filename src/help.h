
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

#define SEEK_END 2
#define SEEK_SET 0
struct _IO_FILE;
typedef struct _IO_FILE FILE;
FILE *stderr;
int printf(const char *__format, ...);
int fprintf(FILE *__stream, const char *__format, ...);
int vfprintf(FILE *__s, const char *__format, __gnuc_va_list __arg);
FILE *fopen(const char *__filename, const char *__modes);
int fread(void *__ptr, int __size, int __n, FILE *__stream);
int fclose(FILE *__stream);
int ftell(FILE *__stream);
int *__errno_location();
#define errno (*__errno_location())

// extern int printf(const char *__restrict __format, ...);
// extern int fprintf(FILE *__restrict __stream,
//                    const char *__restrict __format, ...);
// extern int vfprintf(FILE *__restrict __s, const char *__restrict __format,
//                     __gnuc_va_list __arg);
// extern FILE *fopen(const char *__restrict __filename,
//                    const char *__restrict __modes);
// extern size_t fread(void *__restrict __ptr, size_t __size,
//                     size_t __n, FILE *__restrict __stream);
// extern int fclose(FILE *__stream);
// extern long int ftell(FILE *__stream);
// extern int *__errno_location (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));

int memcmp(const void *__s1, const void *__s2, int __n);
int strlen(const char *__s);
int strncmp(const char *__s1, const char *__s2, int __n);
char *strstr(const char *__haystack, const char *__needle);
int strtol(const char *__nptr, char **__endptr, int __base);
int strcmp(const char *__s1, const char *__s2);
void *memcpy(void *__dest, const void *__src, int __n);
char *strerror(int __errnum);

// extern int memcmp (const void *__s1, const void *__s2, size_t __n)
//      __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
// extern size_t strlen(const char *__s)
//     __attribute__((__nothrow__, __leaf__)) __attribute__((__pure__)) __attribute__((__nonnull__(1)));
// extern int strncmp(const char *__s1, const char *__s2, size_t __n)
//     __attribute__((__nothrow__, __leaf__)) __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
// extern char *strstr(const char *__haystack, const char *__needle)
//     __attribute__((__nothrow__, __leaf__)) __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
// extern long int strtol(const char *__restrict __nptr,
//                        char **__restrict __endptr, int __base)
//     __attribute__((__nothrow__, __leaf__)) __attribute__((__nonnull__(1)));
// extern int strcmp(const char *__s1, const char *__s2)
//     __attribute__((__nothrow__, __leaf__)) __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
// extern void *memcpy(void *__restrict __dest, const void *__restrict __src,
//                     size_t __n) __attribute__((__nothrow__, __leaf__)) __attribute__((__nonnull__(1, 2)));
// extern char *strerror(int __errnum) __attribute__((__nothrow__, __leaf__));

int isalnum(int x);
int isspace(int x);
int isalpha(int x);
int isdigit(int x);
// extern int isalnum(int) __attribute__((__nothrow__, __leaf__));
// extern int isspace (int) __attribute__ ((__nothrow__ , __leaf__));
// extern int isalpha(int) __attribute__((__nothrow__, __leaf__));
// extern int isdigit(int) __attribute__((__nothrow__, __leaf__));

void *calloc(int __nmemb, int __size);
void exit(int __status);
// extern void *calloc (size_t __nmemb, size_t __size)
//      __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__alloc_size__ (1, 2))) ;
// extern void exit(int __status) __attribute__((__nothrow__, __leaf__)) __attribute__((__noreturn__));

/* stddef.h */
#define NULL 0

/* stdbool */
#define bool int
#define true 1
#define false 0