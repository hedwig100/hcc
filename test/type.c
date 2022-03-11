#include "test.h"

typedef int A;
typedef int *B;
typedef struct Node Node;

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
    ok();
    return 0;
}