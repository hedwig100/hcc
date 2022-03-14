#include "hcc.h"

char *user_input;
char *filename;
Func *funcs;
Object *globals;
Str *strs;
Scope *scopes;
Node *code[1000];
Node *static_datas;

int main(int argc, char **argv) {
    err_file = fopen("err.txt", "w");
    if (!err_file) printf("error opening err_file.");

    if (argc == 2) {
        filename   = argv[1];
        user_input = read_file(filename);
    } else if (argc == 3) {
        if (strcmp(argv[1], "-")) {
            errorf("compile option %s is not valid.", argv[1]);
        }
        filename   = "cmd";
        user_input = argv[2];
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
    globals              = new_object(OBJ_GVAR);
    scopes               = calloc(1, sizeof(Scope));
    scopes->before       = NULL;
    scopes->offset       = 0;
    scopes->stack_offset = -16;
    scopes->lvar         = new_object(OBJ_LVAR);
    scopes->strct        = calloc(1, sizeof(Struct));
    program();

    // generate code
    infof("try to generate assembly...");
    gen_program();
    infof("DONE!");
    return 0;
}