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
    globals = calloc(1, sizeof(GVar));
    funcs   = NULL;
    program();

    // generate code
    infof("try to generate assembly...");

    printf(".intel_syntax noprefix\n");

    // data section
    int start = 1;
    for (int i = 0; code[i]; i++) {
        if (code[i]->kind == ND_GVAR) {
            if (start) {
                printf(".data\n");
                start = 0;
            }
            gen_ext_def(code[i]);
        }
    }

    // text section
    start = 1;
    for (int i = 0; code[i]; i++) { // finish if code[i] is NULL
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
        if (code[i]->kind == ND_FUNCDEF) {
            gen_ext_def(code[i]);
        }
    }

    return 0;
}