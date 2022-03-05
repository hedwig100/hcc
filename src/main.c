#include "hcc.h"

#define MAX_INPUT_SIZE 100000 // TODO:support more longer input

void input_from_stdin() {
    char buffer[MAX_INPUT_SIZE];
    char *buf  = buffer;
    size_t len = 0;

    while (1) {
        buf = fgets(buf, MAX_INPUT_SIZE - len, stdin);
        if (!buf) {
            user_input = buffer;
            return;
        }
        len = strlen(buffer);
        buf = buffer + len;
    }
}

int main(int argc, char **argv) {
    if (argc == 1) {
        input_from_stdin();
    } else if (argc == 2) {
        user_input = argv[1];
    } else {
        errorf("number of args is not valid");
        exit(1);
    }

    infof("input:\n%s\n", user_input);

    // tokenize
    infof("try to tokenize...");
    token = tokenize(user_input);

    // construct AST
    infof("try to construct AST...");
    locals = calloc(1, sizeof(LVar));
    program();

    // generate code
    infof("try to generate assembly...");
    printf(".intel_syntax noprefix\n");
    printf(".globl ");
    for (int i = 0; code[i]; i++) {
        if (i == 0)
            printf("%s", to_str(code[i]->name, code[i]->len));
        else
            printf(",%s", to_str(code[i]->name, code[i]->len));
    }
    printf("\n");

    for (int i = 0; code[i]; i++) { // finish if code[i] is NULL
        gen_func_def(code[i]);
    }

    return 0;
}