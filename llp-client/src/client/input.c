#include "client/input.h"

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}


int input() {
    yyparse();
    return 0;
}