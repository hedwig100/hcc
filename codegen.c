#include "9cc.h" 

void error(char *msg) {
    fprintf(stderr,"%s\n",msg); 
    exit(1);
}

// counter create unique number
int counter() {
    static int count = 0;
    return count++;
}

// gen_lval generates local variable (expression)
void gen_lval(Node *node) {
    if (node->kind != ND_LVAR) {
        error("Left side value of assignment is not variable.");
    }

    printf("    mov rax,rbp\n"); 
    printf("    sub rax,%d\n",node->offset); 
    printf("    push rax\n");
}

// gen_expression generates expression 
void gen_expression(Node *node) {
    switch (node->kind) {
    case ND_NUM:
        printf("    push %d\n",node->val); 
        return; 
    case ND_LVAR:
        gen_lval(node); 
        printf("    pop rax\n");
        printf("    mov rax,[rax]\n"); 
        printf("    push rax\n");
        return; 
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen_expression(node->rhs); 
        printf("    pop rdi\n");
        printf("    pop rax\n"); 
        printf("    mov [rax],rdi\n");
        printf("    push rdi\n");
        return;
    }

    gen_expression(node->lhs); 
    gen_expression(node->rhs); 

    printf("    pop rdi\n"); 
    printf("    pop rax\n");

    switch (node->kind) {
    case ND_ADD:
        printf("    add rax,rdi\n"); 
        break; 
    case ND_SUB: 
        printf("    sub rax,rdi\n"); 
        break; 
    case ND_MUL:
        printf("    imul rax,rdi\n"); 
        break; 
    case ND_DIV:
        printf("    cqo\n"); 
        printf("    idiv rdi\n"); 
        break; 
    case ND_EQ:
        printf("    cmp rax,rdi\n"); 
        printf("    sete al\n"); 
        printf("    movzb rax,al\n"); 
        break;
    case ND_NEQ:
        printf("    cmp rax,rdi\n"); 
        printf("    setne al\n"); 
        printf("    movzb rax,al\n"); 
        break;
    case ND_LT:
        printf("    cmp rax,rdi\n"); 
        printf("    setl al\n"); 
        printf("    movzb rax,al\n"); 
        break;
    case ND_LEQ:
        printf("    cmp rax,rdi\n"); 
        printf("    setle al\n"); 
        printf("    movzb rax,al\n"); 
        break;
    default:
        error("not type of expression");
        break;
    }

    printf("    push rax\n"); 
}

// gen_statement generates statement
void gen_statement(Node *node) {
    switch (node->kind) {
    case ND_IF:
        gen_expression(node->cond);
        printf("    pop rax\n");
        printf("    cmp rax,0\n");
        int cnt = counter();

        if (node->els) {
            printf("    je .Lelse%d\n",cnt);
            gen_statement(node->then);
            printf("    jmp .Lend%d\n",cnt);
            printf(".Lelse%d:\n",cnt);
            gen_statement(node->els);
            printf(".Lend%d:\n",cnt);
        } else {
            printf("    je .Lend%d\n",cnt);
            gen_statement(node->then);
            printf(".Lend%d:\n",cnt);
        }

        return;
    case ND_RETURN:
        gen_expression(node->lhs);
        printf("    pop rax\n");
        printf("    mov rsp,rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;
    default:
        gen_expression(node);
        printf("    pop rax\n");
        return;
    }
}