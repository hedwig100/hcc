#include "9cc.h"

int main(int argc,char **argv) {
    if (argc != 2) {
        fprintf(stderr,"number of args is not valid"); 
        return 1; 
    }

    user_input = argv[1];
    fprintf(stderr,"try to tokenize...\n");
    token = tokenize(user_input);
    fprintf(stderr,"try to construct AST...\n");
    locals = calloc(1,sizeof(LVar));
    locals->offset = 0;
    program();
    fprintf(stderr,"try to generate assembly...\n");

    printf(".intel_syntax noprefix\n"); 
    printf(".globl main\n"); 
    printf("main:\n");

    // prologue, allocate space for 26 variables
    printf("    push rbp\n"); 
    printf("    mov rbp, rsp\n"); 
    printf("    sub rsp,208\n");

    
    for (int i = 0;code[i];i++) { // finish if code[i] is NULL
        gen(code[i]);
        printf("    pop rax\n");
    } 

    // epilogue
    printf("    mov rsp,rbp\n"); 
    printf("    pop rbp\n"); 
    printf("    ret\n");
    return 0;
}