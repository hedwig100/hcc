#include "hcc.h" 

// for 16byte alignment
int align = 0;

// counter create unique number
int counter() {
    static int count = 0;
    return count++;
}

// gen_lval generates local variable (expression)
void gen_lval(Node *node) {
    if (node->kind != ND_LVAR) {
        errorf("Left side value of assignment is not variable.");
    }

    printf("    mov rax,rbp # local variable address\n"); 
    printf("    sub rax,%d\n",node->offset); 
    printf("    push rax\n");
    align++;
}

const char *PARAM_REG[6] = {"rdi","rsi","rdx","rcx","r8","r9"};

int gen_param(Node *node) {
    int param_cnt = 0;
    for (Node *now = node->params;now;now = now->params) {
        gen_expression(now);
        param_cnt++;
    }
    for (int i = 0;i < param_cnt;i++) {
        printf("    pop %s\n",PARAM_REG[i]);
        align--;
    }

    if (align%2 == 1) {
        printf("    push 0 # for 16byte alignment\n"); // dummy data
        align++;
        return 1;
    }
    return 0;
}

// gen_expression generates expression 
void gen_expression(Node *node) {
    char ident[101]; // TODO: support function name longer than 100
    int is_pop;

    switch (node->kind) {
    case ND_NUM:
        printf("    push %d # num\n",node->val);
        align++; 
        return; 
    case ND_LVAR:
        gen_lval(node); 
        printf("    pop rax # get local variable\n");
        printf("    mov rax,[rax]\n"); 
        printf("    push rax\n");
        return; 
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen_expression(node->rhs); 
        printf("    pop rdi # assign\n");
        printf("    pop rax\n"); 
        printf("    mov [rax],rdi\n");
        printf("    push rdi\n");
        align--;
        return;
    case ND_CALLFUNC:
        is_pop = gen_param(node);
        strncpy(ident,node->name,node->len);
        ident[node->len] = '\0';
        printf("    call %s\n",ident);
        printf("    push rax\n");
        if (is_pop) {
            printf("    pop rdi # for 16byte alignment\n");
            align--;
        }
        return;
    }

    gen_expression(node->lhs); 
    gen_expression(node->rhs); 

    printf("    pop rdi\n"); 
    printf("    pop rax\n");
    align-=2;

    switch (node->kind) {
    case ND_ADD:
        printf("    add rax,rdi # add\n"); 
        break; 
    case ND_SUB: 
        printf("    sub rax,rdi # sub\n"); 
        break; 
    case ND_MUL:
        printf("    imul rax,rdi # mul\n"); 
        break; 
    case ND_DIV:
        printf("    cqo # div\n"); 
        printf("    idiv rdi\n"); 
        break; 
    case ND_EQ:
        printf("    cmp rax,rdi # equal\n"); 
        printf("    sete al\n"); 
        printf("    movzb rax,al\n"); 
        break;
    case ND_NEQ:
        printf("    cmp rax,rdi # neq\n"); 
        printf("    setne al\n"); 
        printf("    movzb rax,al\n"); 
        break;
    case ND_LT:
        printf("    cmp rax,rdi # less than\n"); 
        printf("    setl al\n"); 
        printf("    movzb rax,al\n"); 
        break;
    case ND_LEQ:
        printf("    cmp rax,rdi # less or equal\n"); 
        printf("    setle al\n"); 
        printf("    movzb rax,al\n"); 
        break;
    default:
        errorf("not type of expression");
        break;
    }

    printf("    push rax\n"); 
    align++;
}

// gen_statement generates statement
void gen_statement(Node *node) {
    int cnt;

    switch (node->kind) {
    case ND_IF:
        gen_expression(node->cond);
        printf("    pop rax\n");
        align--;
        printf("    cmp rax,0\n");
        cnt = counter();

        if (node->els) {
            printf("    je .Lelse%d # if\n",cnt);
            gen_statement(node->then);
            printf("    jmp .Lend%d\n",cnt);
            printf(".Lelse%d: # else\n",cnt);
            gen_statement(node->els);
            printf(".Lend%d:\n",cnt);
        } else {
            printf("    je .Lend%d # if \n",cnt);
            gen_statement(node->then);
            printf(".Lend%d:\n",cnt);
        }

        return;
    case ND_RETURN:
        gen_expression(node->lhs);
        printf("    pop rax # return\n");
        printf("    mov rsp,rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        align-=2;
        return;
    case ND_WHILE:
        cnt = counter();
        printf(".Lbegin%d: # while\n",cnt);
        gen_expression(node->cond);
        printf("    pop rax\n");
        align--;
        printf("    cmp rax,0\n");
        printf("    je .Lend%d\n",cnt);
        gen_statement(node->then);
        printf("    jmp .Lbegin%d\n",cnt);
        printf(".Lend%d:\n",cnt);
        return;
    case ND_FOR:
        cnt = counter();
        if (node->ini) {
            gen_expression(node->ini);
            printf("    pop rax\n");
            align--;
        }
        printf(".Lbegin%d: # for\n",cnt);
        if (node->cond) {
            gen_expression(node->cond);
            printf("    pop rax\n");
            align--;
            printf("    cmp rax,0\n");
            printf("    je .Lend%d\n",cnt);
        }
        gen_statement(node->then);
        if (node->step) {
            gen_expression(node->step);
            printf("    pop rax\n");
            align--;
        }
        printf("    jmp .Lbegin%d\n",cnt);
        printf(".Lend%d:\n",cnt);
        return;
    case ND_BLOCK:
        for (Node *now = node->next;now;now = now->next) {
            gen_statement(now);
        }
        return;
    default:
        gen_expression(node);
        printf("    pop rax\n");
        align--;
        return;
    }
}