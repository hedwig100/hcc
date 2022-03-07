
int print(int x);

int fib(int n) {
    if (n <= 1) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

int main() {
    print(fib(10) + fib(10));
    return 0;
}