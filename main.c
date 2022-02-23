#include "9cc.h"

int main(int argc,char **argv) {
    if (argc != 2) {
        fprintf(stderr,"number of args is not valid"); 
        return 1; 
    }

    user_input = argv[1];

    // tokenize
    infof("try to tokenize...");
    token = tokenize(user_input);

    // construct AST
    infof("try to construct AST...");
    locals = calloc(1,sizeof(LVar));
    locals->offset = 0;
    program();

    // generate code
    infof("try to generate assembly...");

    printf(".intel_syntax noprefix\n"); 
    printf(".globl main\n"); 
    printf("main:\n");

    // prologue
    printf("    push rbp\n"); 
    printf("    mov rbp,rsp\n");
    printf("    sub rsp,%d\n",locals->offset);
    
    for (int i = 0;code[i];i++) { // finish if code[i] is NULL
        gen_statement(code[i]);
    } 
    
    return 0;
}