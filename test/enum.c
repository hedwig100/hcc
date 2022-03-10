#include "test.h"

enum TypeKind {
    TP_A,
    TP_B = 1,
    TP_C = 0,
    TP_D,
    TP_E = 50 + 50,
};

int main() {
    ASSERT(0, TP_A);
    ASSERT(1, TP_B);
    ASSERT(0, TP_C);
    ASSERT(1, TP_D);
    ASSERT(100, TP_E);
    enum Type {
        TP_A = 43,
        TP_B,
        TP_C = 32,
    };
    ASSERT(43, TP_A);
    ASSERT(44, TP_B);
    ASSERT(32, TP_C);

    enum Type a = TP_A;
    ASSERT(43, a);
    enum TypeKind b = TP_A;
    ASSERT(43, b);
    enum TypeKind c = TP_E;
    ASSERT(100, c);

    enum TypeBB {
        TP_A = 10,
    };

    enum TypeBB x   = TP_A;
    enum TypeKind y = TP_A;
    ASSERT(10, x);
    ASSERT(10, x);
    ok();
    return 0;
}