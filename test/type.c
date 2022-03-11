#include "test.h"

typedef int A;
typedef int *B;
typedef struct Node Node;
typedef int C[3];

struct Node {
    Node *next;
    int label;
};

int main() {
    A x = 10;
    ASSERT(10, x);
    B y = &x;
    ASSERT(10, *y);
    Node *node;
    Node z;
    node        = &z;
    node->next  = node;
    node->label = 10;
    ASSERT(10, z.label);
    ASSERT(10, (node->next)->label);
    C arr[5] = {{0, 1, 2}, {3, 4, 5}};
    ASSERT(0, arr[0][0]);
    ASSERT(1, arr[0][1]);
    ASSERT(2, arr[0][2]);
    ASSERT(3, arr[1][0]);
    ASSERT(4, arr[1][1]);
    ASSERT(5, arr[1][2]);
    ok();
    return 0;
}