#define ASSERT(x, y) assert(x, y, #y)

int assert(int expected, int actual, char *code);
int ok();
int printp(int *a);
int print8(int a, int b, int c, int d, int e, int f, int g, int h);
int printf(char *fmt, ...);