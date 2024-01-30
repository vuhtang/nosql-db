#include "client/ast.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AST_NODE_STRING_MAX_LENGTH 1024


struct ast_node *create_bool_literal_ast_node(bool value) {
    struct ast_node *node = malloc(sizeof(struct ast_node));
    node->type = AST_NODE_VAL_BOOL;
    node->value.bool_value = value;
    node->children_count = 0;
    node->children = NULL;
    return node;
}

struct ast_node *create_int_literal_ast_node(int value) {
    struct ast_node *node = malloc(sizeof(struct ast_node));
    node->type = AST_NODE_VAL_INT;
    node->value.int_value = value;
    node->children_count = 0;
    node->children = NULL;
    return node;
}

struct ast_node *create_float_literal_ast_node(float value) {
    struct ast_node *node = malloc(sizeof(struct ast_node));
    node->type = AST_NODE_VAL_FLOAT;
    node->value.float_value = value;
    node->children_count = 0;
    node->children = NULL;
    return node;
}

struct ast_node *create_string_literal_ast_node(char *value) {
    struct ast_node *node = malloc(sizeof(struct ast_node));
    node->type = AST_NODE_VAL_STRING;
    node->value.string_value = value;
    node->children_count = 0;
    node->children = NULL;
    return node;
}

struct ast_node *create_identifier_ast_node(char *value) {
    struct ast_node *node = malloc(sizeof(struct ast_node));
    node->type = AST_NODE_ID;
    node->value.string_value = value;
    node->children_count = 0;
    node->children = NULL;
    return node;
}

char *get_op_from_ast_node(struct ast_node *node) {
    switch (node->type) {
        case AST_NODE_OP_EQ:
            return "eq";
        case AST_NODE_OP_LESS:
            return "less";
        case AST_NODE_OP_GREATER:
            return "greater";
        default:
            return "wrong operation";
    }
}

char *get_char_value_from_ast_node(struct ast_node *node) {
    switch (node->type) {
        case AST_NODE_VAL_BOOL:
            return (node->value.bool_value ? "true" : "false");
        case AST_NODE_VAL_FLOAT: {
            char *out;
            sprintf(out, "%f", node->value.float_value);
            return out;
        }
        case AST_NODE_VAL_INT: {
            char *out;
            sprintf(out, "%d", node->value.int_value);
            return out;
        }
        case AST_NODE_VAL_STRING:
            return node->value.string_value;
        default:
            return "error value";
    }
}

struct ast_node *create_ast_node(enum ast_node_type type, size_t children_count, ...) {
    struct ast_node *node = malloc(sizeof(struct ast_node));
    node->type = type;
    node->children_count = children_count;
    node->children = malloc(sizeof(struct ast_node *) * children_count);

    va_list args;
    va_start(args, children_count);
    for (size_t i = 0; i < children_count; i++) {
        node->children[i] = va_arg(args, struct ast_node *);
    }
    va_end(args);

    return node;
}

void add_ast_node(struct ast_node *node, struct ast_node *child) {
    struct ast_node **tmp = malloc(sizeof(struct ast_node *) * (node->children_count + 1));
    memcpy(tmp, node->children, sizeof(struct ast_node *) * node->children_count);
    free(node->children);
    node->children = tmp;
    node->children_count += 1;
    node->children[node->children_count - 1] = child;
}

struct ast_node *root = NULL;

void set_root_ast_node(struct ast_node *root_to_set) {
    root = root_to_set;
}

void print_ast_node_internal(struct ast_node *node, int level) {
    if (node == NULL) {
        printf("NULL\n");
        return;
    }

    char *indent = malloc(sizeof(char) * (2 * level + 1));
    for (int i = 0; i < level; i++) {
        indent[i * 2] = '|';
        indent[i * 2 + 1] = '\t';
    }
    indent[2 * level] = '\0';

    char *value = malloc(sizeof(char) * AST_NODE_STRING_MAX_LENGTH);
    switch (node->type) {
        case AST_NODE_VAL_BOOL:
            sprintf(value, "%s", node->value.bool_value ? "true" : "false");
            break;
        case AST_NODE_VAL_INT:
            sprintf(value, "%d", node->value.int_value);
            break;
        case AST_NODE_VAL_FLOAT:
            sprintf(value, "%f", node->value.float_value);
            break;
        case AST_NODE_VAL_STRING:
            sprintf(value, "\'%s\'", node->value.string_value);
            break;
        case AST_NODE_ID:
            sprintf(value, "%s", node->value.string_value);
            break;
        default:
            sprintf(value, "%s", ast_node_type_to_string[node->type]);
            break;
    }

    printf("%s%s\n", indent, value);

    for (size_t i = 0; i < node->children_count; i++) {
        print_ast_node_internal(node->children[i], level + 1);
    }

    free(indent);
    free(value);
}

void print_ast_node(struct ast_node *node) {
    print_ast_node_internal(node, 0);
}

struct ast_node *get_root_ast_node() {
    return root;
}

struct ast_node *get_entity_body_node(struct ast_node *root_node) {
    struct ast_node *out = root_node->type == AST_NODE_QUERY_INSERT ? root_node->children[0] : root_node->children[1];
    return out;
}

struct ast_node *get_condition_body_node(struct ast_node *root_node) {
    struct ast_node *out = root_node->type != AST_NODE_QUERY_INSERT ? root_node->children[0] : NULL;
    return out;
}

struct ast_node *get_represent_body_node(struct ast_node *root_node) {
    struct ast_node *out = root_node->type == AST_NODE_QUERY_UPDATE ? root_node->children[2] : root_node->children[1];
    return out;
}

void free_ast_node(struct ast_node *node) {
    if (node == NULL) {
        return;
    }

    if (node->children_count > 0) {
        for (size_t i = 0; i < node->children_count; i++) {
            free_ast_node(node->children[i]);
        }
        free(node->children);
    }
    free(node);
}