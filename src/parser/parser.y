%code requires {
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "ast/ast.h"

}

%union {
	char *sval;
	float fval;
	int ival;
	bool bval;
	struct ast_node *node;
}

%token <sval> ID
%token <sval> VAL_STRING
%token <ival> VAL_INTEGER
%token <bval> VAL_BOOL
%token <fval> VAL_FLOAT

%token OP_EQUALS
%token OP_GREATER
%token OP_LESS

%token L_BRACE
%token R_BRACE
%token L_BRACKET
%token R_BRACKET
%token SEMICOLON
%token COLON

%token SELECT
%token INSERT
%token DELETE
%token UPDATE

%type <node> select_query
%type <node> insert_query
%type <node> delete_query
%type <node> update_query
%type <node> condition
%type <node> condition_body
%type <node> entity_list
%type <node> entity
%type <node> entity_body
%type <node> field_list
%type <node> field
%type <node> property_list
%type <node> property
%type <node> input
%type <node> repr_body
%type <node> repr_field
%type <node> repr_list

%start input

%%

input:
	SELECT select_query SEMICOLON { set_root_ast_node($2); $$ = $2; }
|	INSERT insert_query SEMICOLON { set_root_ast_node($2); $$ = $2; }
|	UPDATE update_query SEMICOLON { set_root_ast_node($2); $$ = $2; }
|	DELETE delete_query SEMICOLON { set_root_ast_node($2); $$ = $2; }
;

select_query:
	L_BRACE condition_body repr_body R_BRACE	{
		$$ = create_ast_node(AST_NODE_QUERY_SELECT, 2, $2, $3);
		}
;

update_query:
	L_BRACE condition_body entity_body repr_body R_BRACE	{
		$$ = create_ast_node(AST_NODE_QUERY_UPDATE, 3, $2, $3, $4);
                }
;

delete_query:
	L_BRACE condition_body repr_body R_BRACE	{
		$$ = create_ast_node(AST_NODE_QUERY_DELETE, 2, $2, $3);
		}
;

insert_query:
	L_BRACE entity_body repr_body R_BRACE	{
		$$ = create_ast_node(AST_NODE_QUERY_INSERT, 2, $2, $3);
		}
;

repr_body:
	L_BRACE R_BRACE			{ $$ = create_ast_node(AST_NODE_REPR_BODY, 0); }
|	L_BRACE repr_list R_BRACE	{ $$ = create_ast_node(AST_NODE_REPR_BODY, 1, $2); }
;

repr_list:
	repr_field		{
		$$ = create_ast_node(AST_NODE_REPR_LIST, 1, $1);
	}
|	repr_list repr_field	{
		add_ast_node($1, $2);
	}
;

repr_field:
	ID COLON repr_list	{
		$$ = create_ast_node(AST_NODE_REPR_FIELD, 2, create_identifier_ast_node($1), $3);
		}
|	ID			{
		$$ = create_ast_node(AST_NODE_REPR_FIELD, 1, create_identifier_ast_node($1));
		}
;

condition_body:
|	ID L_BRACKET condition R_BRACKET	{
		$$ = create_ast_node(AST_NODE_CONDITION_BODY, 2, create_identifier_ast_node($1), $3);
		}
;

condition:
	L_BRACE property_list R_BRACE	{ $$ = create_ast_node(AST_NODE_CONDITION, 1, $2); }
;

entity_body:
	ID L_BRACKET L_BRACE entity_list R_BRACE R_BRACKET {
		$$ = create_ast_node(AST_NODE_ENTITY_BODY, 2, create_identifier_ast_node($1), $4);
		}
;

property_list:
	property			{ $$ = create_ast_node(AST_NODE_PROPERTY_LIST, 1, $1); }
|	property_list property		{ add_ast_node($1, $2); }
;

entity_list:
	entity			{ $$ = create_ast_node(AST_NODE_ENTITY_LIST, 1, $1); }
|	entity_list entity	{ add_ast_node($1, $2); }
;

property:
	ID OP_EQUALS VAL_STRING		{
		$$ = create_ast_node(AST_NODE_OP_EQ, 2, create_identifier_ast_node($1), create_string_literal_ast_node($3));
		}
|	ID OP_LESS VAL_INTEGER		{
		$$ = create_ast_node(AST_NODE_OP_LESS, 2, create_identifier_ast_node($1), create_int_literal_ast_node($3));
		}
|	ID OP_GREATER VAL_INTEGER		{
		$$ = create_ast_node(AST_NODE_OP_GREATER, 2, create_identifier_ast_node($1), create_int_literal_ast_node($3));
		}
|	ID OP_EQUALS VAL_INTEGER	{
		$$ = create_ast_node(AST_NODE_OP_EQ, 2, create_identifier_ast_node($1), create_int_literal_ast_node($3));
		}
|	ID OP_LESS VAL_FLOAT		{
		$$ = create_ast_node(AST_NODE_OP_LESS, 2, create_identifier_ast_node($1), create_float_literal_ast_node($3));
		}
|	ID OP_GREATER VAL_FLOAT		{
		$$ = create_ast_node(AST_NODE_OP_GREATER, 2, create_identifier_ast_node($1), create_float_literal_ast_node($3));
		}
|	ID OP_EQUALS VAL_FLOAT		{
		$$ = create_ast_node(AST_NODE_OP_EQ, 2, create_identifier_ast_node($1), create_float_literal_ast_node($3));
		}
|	ID OP_LESS VAL_BOOL		{
		$$ = create_ast_node(AST_NODE_OP_LESS, 2, create_identifier_ast_node($1), create_bool_literal_ast_node($3));
		}
|	ID OP_GREATER VAL_BOOL		{
		$$ = create_ast_node(AST_NODE_OP_GREATER, 2, create_identifier_ast_node($1), create_bool_literal_ast_node($3));
		}
|	ID OP_EQUALS VAL_BOOL		{
		$$ = create_ast_node(AST_NODE_OP_EQ, 2, create_identifier_ast_node($1), create_bool_literal_ast_node($3));
		}
;

entity:
	ID COLON L_BRACE field_list R_BRACE	{
		$$ = create_ast_node(AST_NODE_ENTITY, 2, create_identifier_ast_node($1), $4);
		}
|	field_list				{ $$ = create_ast_node(AST_NODE_ENTITY, 1, $1); }
;

field_list:
	field			{ $$ = create_ast_node(AST_NODE_FIELD_LIST, 1, $1); }
|	field_list field	{ add_ast_node($1, $2); }
;

field: ID COLON VAL_BOOL	{
		$$ = create_ast_node(AST_NODE_FIELD, 2, create_identifier_ast_node($1), create_bool_literal_ast_node($3));
		}
|	ID COLON VAL_INTEGER	{
		$$ = create_ast_node(AST_NODE_FIELD, 2, create_identifier_ast_node($1), create_int_literal_ast_node($3));
		}
|	ID COLON VAL_FLOAT 	{
		$$ = create_ast_node(AST_NODE_FIELD, 2, create_identifier_ast_node($1), create_float_literal_ast_node($3));
		}
|	ID COLON VAL_STRING 	{
		$$ = create_ast_node(AST_NODE_FIELD, 2, create_identifier_ast_node($1), create_string_literal_ast_node($3));
		}
;

%%

int main() {
	yyparse();
	print_ast_node(get_root_ast_node());
	free_ast_node(get_root_ast_node());
	return 0;
}


int yyerror(char *s) {
	printf("ERROR: %s\n", s);

	return 0;
}