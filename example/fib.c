int printf(const char *__format, ...);

int memo[25];

int fib(int n) {
    if (memo[n] >= 0) return memo[n];
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}

int main() {
    for (int i = 0; i < 25; i++) {
        memo[i] = -1;
    }
    for (int i = 0; i < 25; i++) {
        printf("fib(%d): %d \n", i, fib(i));
    }
}