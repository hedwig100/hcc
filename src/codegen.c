#include "hcc.h"

// for 16byte alignment
int align = 0;

// gen_lvar generates local variable address (expression)
void gen_lvar(Node *node) {
    if (node->kind != ND_LVAR) errorf("node kind is not ND_LVAR.");
    printf("    mov rax,rbp # local variable address\n");
    printf("    sub rax,%d\n", node->offset);
    printf("    push rax # stack%d\n", align++);
}

// gen_gvar generates global variable address (expression)
void gen_gvar(Node *node) {
    if (node->kind != ND_GVAR) errorf("node kind is not ND_GVAR.");
    printf("    lea rax, [%s + rip] # global variable address\n", to_str(node->name, node->len));
    printf("    push rax # stack%d\n", align++);
}

void gen_addr(Node *node) {
    switch (node->kind) {
    case ND_LVAR:
        gen_lvar(node);
        return;
    case ND_GVAR:
        gen_gvar(node);
        return;
    case ND_DEREF:
        gen_expression(node->lhs);
        return;
    default:
        errorf("Left side value of assignment is not variable.");
    }
}

const char *PARAM_REG64[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
const char *PARAM_REG32[6] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
const char *PARAM_REG16[6] = {"di", "si", "dx", "cx", "r8w", "r9w"};
const char *PARAM_REG8[6]  = {"dil", "sil", "dl", "cl", "r8b", "r9b"};

// gen_store stores data in reg in [rax]
void gen_store(Type *typ, const char *reg64, const char *reg32, const char *reg16, const char *reg8) {
    int before_offset = 0;
    switch (typ->kind) {
    case TP_INT:
    case TP_ENUM:
        printf("    mov dword ptr [rax],%s # int \n", reg32);
        return;
    case TP_CHAR:
        printf("    mov byte ptr [rax],%s # char\n", reg8);
        return;
    case TP_ARRAY:
    case TP_PTR:
        printf("    mov qword ptr [rax],%s # ptr,arr\n", reg64);
        return;
    case TP_STRUCT:
        // NOTE:this code assumes reg64 is not rax
        printf("    push %s # stack%d,num\n", reg64, align++);
        for (int i = 0; i < typ->size; i++) {
            printf("    pop %s # stack%d,num\n", reg64, --align);
            printf("    push %s # stack%d,num\n", reg64, align++);
            printf("    mov %s, byte ptr [%s+%d]\n", reg8, reg64, i);
            printf("    mov byte ptr [rax+%d],%s # struct\n", i, reg8);
        }
        printf("    pop %s # stack%d,num\n", reg64, --align);
        return;
    default:
        errorf("typ isn't valid.");
    }
}

// gen_load loads data in [rax] to reg
void gen_load(Type *typ, const char *reg64) {
    switch (typ->kind) {
    case TP_INT:
    case TP_ENUM:
        printf("    movsx %s,dword ptr [rax] # int\n", reg64);
        return;
    case TP_CHAR:
        printf("    movsx %s,byte ptr [rax] # char\n", reg64);
        return;
    case TP_PTR:
        printf("    mov %s,qword ptr [rax] # ptr\n", reg64);
        return;
    case TP_ARRAY:
    case TP_STRUCT:
        return;
    default:
        errorf("typ isn't valid.");
    }
}

int gen_param_set(Node *node) {
    int n_param = 0;
    for (Node *now = node->params; now; now = now->next) {
        gen_expression(now);
        n_param++;
    }
    for (int i = n_param - 1; i >= 0; i--) {
        printf("    pop %s # stack%d\n", PARAM_REG64[i], --align);
    }

    if (align % 2 == 1) {
        printf("    push 0 # stack%d,for 16byte alignment\n", align++); // dummy data
        return 1;
    }
    return 0;
}

void gen_param_get(Node *node) {
    int i = 0;
    for (Node *now = node->params; now; now = now->next) {
        printf("    mov rax,rbp\n");
        printf("    sub rax,%d\n", now->offset);
        gen_store(now->typ, PARAM_REG64[i], PARAM_REG32[i], PARAM_REG16[i], PARAM_REG8[i]);
        i++;
    }
}

// gen_expression generates expression
void gen_expression(Node *node) {
    char ident[101]; // TODO: support function name longer than 100
    int is_pop;
    Node *now;

    switch (node->kind) {
    case ND_NUM:
        printf("    push %d # stack%d,num\n", node->val, align++);
        return;
    case ND_STR:
        printf("    lea rax, [.LC%d + rip] # string literal\n", node->id);
        printf("    push rax # stack%d\n", align++);
        return;
    case ND_STMTEXPR:
        for (now = node->lhs->block; now; now = now->next) {
            if (now->next)
                gen_statement(now);
            else
                gen_expression(now);
        }
        return;
    case ND_LVAR:
        gen_addr(node);
        printf("    pop rax # stack%d, get local variable\n", --align);
        gen_load(node->typ, "rax");
        printf("    push rax # stack%d\n", align++);
        return;
    case ND_GVAR:
        gen_gvar(node);
        printf("    pop rax # stack%d, get local variable\n", --align);
        gen_load(node->typ, "rax");
        printf("    push rax # stack%d\n", align++);
        return;
    case ND_ASSIGN:
        gen_addr(node->lhs);
        gen_expression(node->rhs);
        printf("    pop rdi # stack%d, assign\n", --align);
        printf("    pop rax # stack%d\n", --align);

        if (node->lhs->typ->kind == TP_ARRAY) {
            errorf("cannot assign to array.");
        }
        gen_store(node->lhs->typ, "rdi", "edi", "di", "dil");

        printf("    push rdi # stack%d\n", align++);
        return;
    case ND_CALLFUNC:
        is_pop = gen_param_set(node);
        printf("    call %s\n", to_str(node->name, node->len));
        if (is_pop) {
            printf("    pop rdi # stack%d,for 16byte alignment\n", --align);
        }
        printf("    push rax # stack%d\n", align++);
        return;
    case ND_ADDR:
        // &lvar is ok, &(x + y) is bad
        gen_addr(node->lhs);
        return;
    case ND_DEREF:
        gen_expression(node->lhs);
        printf("    pop rax # stack%d, deref \n", --align);
        gen_load(node->typ, "rax");
        printf("    push rax # stack%d\n", align++);
        return;
    case ND_ARRAY:
        for (Node *now = node->initlist; now; now = now->next) {
            gen_expression(now);
            printf("    pop rax # stack%d\n", --align);
        }
        printf("    push rax # stack%d\n", align++);
        return;
    case ND_INC:
        gen_addr(node->lhs);
        gen_expression(node->rhs);
        printf("    pop rdi # stack%d\n", --align);
        printf("    pop rax # stack%d\n", --align);
        gen_load(node->lhs->typ, "rsi");
        printf("    push rsi # stack%d\n", align++);
        printf("    add rsi,rdi\n");
        gen_store(node->lhs->typ, "rsi", "esi", "si", "sil");
        return;
    case ND_DEC:
        gen_addr(node->lhs);
        gen_expression(node->rhs);
        printf("    pop rdi # stack%d\n", --align);
        printf("    pop rax # stack%d\n", --align);
        gen_load(node->lhs->typ, "rsi");
        printf("    push rsi # stack%d\n", align++);
        printf("    sub rsi,rdi\n");
        gen_store(node->lhs->typ, "rsi", "esi", "si", "sil");
        return;
    case ND_TERNARY:
        gen_expression(node->cond);
        printf("    pop rax # stack%d\n", --align);
        printf("    cmp rax,0\n");
        printf("    je .Lbegin%d\n", node->label);
        gen_expression(node->then);
        printf("    jmp .Lend%d\n", node->label);
        printf(".Lbegin%d:\n", node->label);
        gen_expression(node->els);
        printf(".Lend%d:\n", node->label);
        return;
    }

    gen_expression(node->lhs);
    gen_expression(node->rhs);

    printf("    pop rdi # stack%d\n", --align);
    printf("    pop rax # stack%d\n", --align);

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
    case ND_MOD:
        printf("    cqo # mod\n");
        printf("    idiv rdi\n");
        printf("    mov rax,rdx\n");
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
    case ND_AND:
        printf("    and rax,rdi # and\n");
        break;
    case ND_XOR:
        printf("    xor rax,rdi # xor\n");
        break;
    case ND_OR:
        printf("    or rax,rdi # or \n");
        break;
    case ND_LSHIFT:
        printf("    mov rcx,rdi\n");
        printf("    sal rax,cl # signed left shift\n");
        break;
    case ND_RSHIFT:
        printf("    mov rcx,rdi\n");
        printf("    sar rax,cl # signed right shift\n");
        break;
    default:
        errorf("not type of expression");
        break;
    }

    printf("    push rax # stack%d\n", align++);
}

// gen_statement generates statement
void gen_statement(Node *node) {
    int i;

    switch (node->kind) {
    case ND_IF:
        gen_expression(node->cond);
        printf("    pop rax # stack%d\n", --align);
        printf("    cmp rax,0\n");

        if (node->els) {
            printf("    je .Lelse%d # if\n", node->label);
            gen_statement(node->then);
            printf("    jmp .Lend%d\n", node->label);
            printf(".Lelse%d: # else\n", node->label);
            gen_statement(node->els);
            printf(".Lend%d:\n", node->label);
        } else {
            printf("    je .Lend%d # if \n", node->label);
            gen_statement(node->then);
            printf(".Lend%d:\n", node->label);
        }

        return;
    case ND_SWITCH:
        i                = 0;
        bool has_default = false;
        gen_expression(node->cond);
        for (Node *now = node->block; now; now = now->next) {
            if (now->kind == ND_CASE) {
                // case
                gen_expression(now->cond);
                printf("    pop rdi # stack%d\n", --align);
                printf("    pop rax # stack%d\n", --align);
                printf("    push rax # stack%d\n", align++);
                printf("    cmp rax,rdi\n");
                printf("    setne al\n");
                printf("    movzb rax,al\n");
                printf("    cmp rax,0\n");
                printf("    je .L%dcase%d\n", node->label, i++);
            } else if (now->kind == ND_DEFAULT) {
                // default
                has_default = true;
            } else {
                errorf(token->str, "node kind isn't valid for switch");
            }
        }
        if (has_default) printf("   jmp .L%ddefa\n", node->label);
        printf("    jmp .Lend%d\n", node->label);
        i = 0;
        for (Node *now = node->block; now; now = now->next) {
            if (now->kind == ND_CASE) {
                // case
                printf(".L%dcase%d:\n", node->label, i++);
                gen_statement(now);
            } else {
                // default
                printf(".L%ddefa:\n", node->label);
                gen_statement(now);
            }
        }
        printf(".Lend%d:\n", node->label);
        printf("    pop rax # stack%d\n", --align);
        return;
    case ND_CASE:
    case ND_DEFAULT:
        for (Node *now = node->block; now; now = now->next) {
            gen_statement(now);
        }
        return;
    case ND_RETURN:
        if (node->lhs) {
            gen_expression(node->lhs);
            printf("    pop rax # stack%d,return\n", --align);
        }
        printf("    mov rsp,rbp\n");
        printf("    pop rbp # stack%d \n", --align);
        printf("    ret\n");
        return;
    case ND_WHILE:
        printf(".Lbegin%d: # while\n", node->label);
        printf(".Lcont%d:\n", node->label);
        gen_expression(node->cond);
        printf("    pop rax # stack%d\n", --align);
        printf("    cmp rax,0\n");
        printf("    je .Lend%d\n", node->label);
        gen_statement(node->then);
        printf("    jmp .Lbegin%d\n", node->label);
        printf(".Lend%d:\n", node->label);
        return;
    case ND_FOR:
        if (node->ini) {
            gen_expression(node->ini);
            printf("    pop rax # stack%d\n", --align);
        }
        printf(".Lbegin%d: # for\n", node->label);
        if (node->cond) {
            gen_expression(node->cond);
            printf("    pop rax # stack%d\n", --align);
            printf("    cmp rax,0\n");
            printf("    je .Lend%d\n", node->label);
        }
        gen_statement(node->then);
        printf(".Lcont%d:\n", node->label);
        if (node->step) {
            gen_expression(node->step);
            printf("    pop rax # stack%d\n", --align);
        }
        printf("    jmp .Lbegin%d\n", node->label);
        printf(".Lend%d:\n", node->label);
        return;
    case ND_BREAK:
        printf("    jmp .Lend%d # break \n", node->label);
        return;
    case ND_CONTINUE:
        printf("    jmp .Lcont%d # continue \n", node->label);
        return;
    case ND_BLOCK:
        for (Node *now = node->block; now; now = now->next) {
            gen_statement(now);
        }
        return;
    default:
        gen_expression(node);
        printf("    pop rax # stack%d\n", --align);
        return;
    }
}

void gen_initializer(Node *node, Type *typ) {
    int zero;

    switch (node->kind) {
    case ND_NUM:
        printf("    .long %d\n", node->val);
        return;
    case ND_ADDR:
        printf("    .quad %s\n", to_str(node->lhs->name, node->lhs->len));
        return;
    case ND_GVAR:
        printf("    .quad %s\n", to_str(node->name, node->len));
        return;
    case ND_ADD:
        // lhs must be ND_ADDR or ND_GVAR,rhs must be ND_NUM
        if (node->lhs->kind == ND_ADDR)
            printf("    .quad %s+%d\n", to_str(node->lhs->lhs->name, node->lhs->lhs->len), node->rhs->val);
        else if (node->lhs->kind == ND_GVAR)
            printf("    .quad %s+%d\n", to_str(node->lhs->name, node->lhs->len), node->rhs->val);
        else
            errorf("cannot eval this value");
        return;
    case ND_SUB:
        // lhs must be ND_ADDR or ND_GVAR,rhs must be ND_NUM
        if (node->lhs->kind == ND_ADDR)
            printf("    .quad %s-%d\n", to_str(node->lhs->lhs->name, node->lhs->lhs->len), node->rhs->val);
        else if (node->lhs->kind == ND_GVAR)
            printf("    .quad %s-%d\n", to_str(node->lhs->name, node->lhs->len), node->rhs->val);
        else
            errorf("cannot eval this value");
        return;
    case ND_ARRAY:
        zero = typ->size;
        for (Node *now = node->initlist; now; now = now->next) {
            gen_initializer(now, typ->ptr_to);
            zero -= typ->ptr_to->size;
        }
        if (zero > 0) {
            printf("    .zero %d\n", zero);
        }
        return;
    case ND_STR:
        if (is_typ(typ, TP_PTR)) {
            printf("    .quad .LC%d\n", node->id);
        } else if (is_typ(typ, TP_ARRAY)) {
            printf("    .ascii %s\n", to_str(node->name, node->len));
            if (typ->size > node->len - 2) { // node->len = size("str")
                printf("    .zero %d\n", typ->size - node->len + 2);
            }
        }
        return;
    default:
        errorf("cannot initilize with not number.");
    }
}

void gen_ext_def(Node *node) {
    switch (node->kind) {
    case ND_FUNCDEF:
        align = 0;
        printf("%s:\n", to_str(node->name, node->len));

        // prologue
        printf("    push rbp # stack%d\n", align++);
        printf("    mov rbp,rsp\n");
        printf("    sub rsp,%d\n", node->offset);
        gen_param_get(node);
        gen_statement(node->body);
        return;
    case ND_GVAR:
        printf("%s:\n", to_str(node->name, node->len));
        printf("    .zero %d\n", node->typ->size);
        return;
    case ND_INIT:
        printf("%s:\n", to_str(node->lhs->name, node->lhs->len));
        gen_initializer(node->rhs, node->lhs->typ);
        return;
    default:
        errorf("not external definition");
        return;
    }
}

void gen_program() {
    printf(".intel_syntax noprefix\n");

    // data section
    int start = 1;
    for (int i = 0; code[i]; i++) {
        if (!code[i]) continue;
        if (code[i]->kind == ND_GVAR || code[i]->kind == ND_INIT) {
            if (start) {
                printf(".data\n");
                start = 0;
            }
            gen_ext_def(code[i]);
        }
    }
    for (Str *str = strs; str; str = str->next) {
        printf(".LC%d:\n", str->id);
        printf("    .string %s\n", to_str(str->val, str->len));
    }

    // text section
    start = 1;
    for (int i = 0; code[i]; i++) { // finish if code[i] is NULL
        if (!code[i]) continue;
        if (code[i]->kind == ND_FUNCDEF) {
            if (start) {
                printf(".text\n");
                printf(".globl %s", to_str(code[i]->name, code[i]->len));
                start = 0;
            } else {
                printf(",%s", to_str(code[i]->name, code[i]->len));
            }
        }
    }
    printf("\n");
    for (int i = 0; code[i]; i++) {
        if (!code[i]) continue;
        if (code[i]->kind == ND_FUNCDEF) {
            gen_ext_def(code[i]);
        }
    }
}