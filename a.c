#include <stdio.h>

int main() {
    int ans = 0;
    int i;
    for (i = 0; i < 50; ++i) {
        int j;
        if (i == 5) continue;
        for (j = 0; j < 10; ++j) {
            if (j / 4 == 2) break;
            ans = ans + i - 20;
        }
    }
    printf("%d\n", ans);
    return 0;
}