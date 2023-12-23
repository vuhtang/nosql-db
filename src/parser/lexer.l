%{
    #include <string.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdbool.h>
    #include "parser.tab.h"
%}


id [a-zA-Z]+
float [-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?
integer [-+]?[0-9]+
boolean true|false
string_w_quotes \"[^\"]*\"

%%

"{" { return L_BRACE; }
"}" { return R_BRACE; }
"(" { return L_BRACKET; }
")" { return R_BRACKET; }
";" { return SEMICOLON; }
":" { return COLON; }
"=" { return OP_EQUALS; }
"<" { return OP_LESS; }
">" { return OP_GREATER; }

select { return SELECT; }
insert { return INSERT; }
update { return UPDATE; }
delete { return DELETE; }

{boolean} {
    yylval.bval = (strcmp(yytext, "true") == 0);
    return VAL_BOOL;
}

{id} {
    yylval.sval = strdup(yytext);
    return ID;
}

{integer} {
    yylval.ival = atoi(yytext);
    return VAL_INTEGER;
}

{float} {
    yylval.fval = atof(yytext);
    return VAL_FLOAT;
}

{string_w_quotes} {
    yylval.sval = strdup(yytext);
    return VAL_STRING;
}

[ \t\n] ;
. ;

%%

int yywrap() { return 1; }
