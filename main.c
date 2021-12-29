#include "9cc.h"

int main(int argc,char **argv) {
    if (argc != 2) {
        fprintf(stderr,"number of args is not valid"); 
        return 1; 
    }

    user_input = argv[1];
    token = tokenize(user_input);
    program();

    printf(".intel_syntax noprefix\n"); 
    printf(".globl main\n"); 
    printf("main:\n");

    // プロローグ, 変数26個分の領域を確保
    printf("    push rbp\n"); 
    printf("    mov rbp, rsp\n"); 
    printf("    sub rsp,208\n");

    
    for (int i = 0;code[i];i++) { // code[i]=NULLになると終了
        gen(code[i]);
        printf("    pop rax\n"); // スタックが溢れることを防ぐためにいらないものは消す。最後のみraxを出力として利用する
    } 

    // エピローグ
    printf("    mov rsp,rbp\n"); 
    printf("    pop rbp\n"); 
    printf("    ret\n");
    return 0;
}