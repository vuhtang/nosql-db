#ifndef LLP_AST_H
#define LLP_AST_H

#include <stdbool.h>
#include <stddef.h>

#include "ast_node_type.h"

struct ast_node {
    union {
        char *string_value;
        int int_value;
        float float_value;
        bool bool_value;
    } value;
    enum ast_node_type type;
    size_t children_count;
    struct ast_node **children;
};

struct ast_node *create_bool_literal_ast_node(bool value);
struct ast_node *create_int_literal_ast_node(int value);
struct ast_node *create_float_literal_ast_node(float value);
struct ast_node *create_string_literal_ast_node(char *value);
struct ast_node *create_identifier_ast_node(char *value);
struct ast_node *create_ast_node(enum ast_node_type type, size_t children_count, ...);
void add_ast_node(struct ast_node *node, struct ast_node *child);
void set_root_ast_node(struct ast_node *root_to_set);
struct ast_node *get_root_ast_node();
void free_ast_node(struct ast_node *node);
void print_ast_node(struct ast_node *node);


#endif
