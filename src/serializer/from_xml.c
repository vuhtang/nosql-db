#include "serializer/from_xml.h"
void print_element_names(xmlNode *a_node) {
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
            if (xmlStrEqual(cur_node->name, BAD_CAST "id"))
                printf("%s", cur_node->children->content);
        }

        print_element_names(cur_node->children);
    }
}

void find_xml_node(char *node_name, xmlNodePtr ptr, xmlNodePtr *ans) {

    xmlNodePtr cur_node = NULL;

    for (cur_node = ptr; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE && xmlStrEqual(cur_node->name, BAD_CAST node_name)) {
            if (!*ans)
                *ans = cur_node;
        }

        find_xml_node(node_name, cur_node->children, ans);
    }
}

enum query_value_cmp_op get_operation_from_node(xmlNodePtr operation_node) {
    if (xmlStrEqual(operation_node->children->content, BAD_CAST "eq"))
        return EQUALS;
    if (xmlStrEqual(operation_node->children->content, BAD_CAST "less"))
        return LESS;
    if (xmlStrEqual(operation_node->children->content, BAD_CAST "greater"))
        return BIGGER;
    return SET_VAL;
}

enum query_value_type get_value_type_from_node(xmlNodePtr value_node) {
    if (xmlStrEqual(value_node->children->content, BAD_CAST "true") ||
        xmlStrEqual(value_node->children->content, BAD_CAST "false"))
        return Q_VAL_BOOL;

    int32_t i = atoi((char *) value_node->children->content);
    if (i != 0)
        return Q_VAL_INT32;

    float f = atof((char *) value_node->children->content);
    if (f != 0)
        return Q_VAL_FLOAT;

    return Q_VAL_STRING;
}

union raw_value get_raw_value_from_node(xmlNodePtr value_node, enum query_value_type value_type) {
    switch (value_type) {
        case Q_VAL_STRING: {
            char *s = (char *) value_node->children->content;
            struct string *s_s = init_string(s, 0);
            union raw_value v = {.string = *s_s};
            return v;
        }
        case Q_VAL_INT32: {
            union raw_value v = {.int32 = atoi((char *) value_node->children->content)};
            return v;
        }
        case Q_VAL_FLOAT: {
            union raw_value v = {.aFloat = atof((char *) value_node->children->content)};
            return v;
        }
        case Q_VAL_BOOL: {
            union raw_value v = {.aBool = xmlStrEqual(value_node->children->content, BAD_CAST "true") ? true : false};
            return v;
        }
    }
}

void add_elem_to_list(struct query_condition *list, struct query_condition_element *element) {
    if (!list->condition) {
        list->condition = element;
        return;
    }
    struct query_condition *cur = list;
    while (cur->next)
        cur = cur->next;
    cur->next = malloc(sizeof(struct query_condition));
    cur->next->condition = element;
}

struct query_condition *get_query_condition_from_node(xmlNodePtr condition_elements) {
    struct query_condition *query_condition = malloc(sizeof(struct query_condition));

    xmlNodePtr node = condition_elements;
    xmlNodePtr condition_element = NULL;
    find_xml_node("condition_element", node, &condition_element);

    while (condition_element) {
        // find id node
        xmlNodePtr id_node = NULL;
        find_xml_node("id", condition_element, &id_node);
        char *id = (char *) id_node->children->content;
        struct string *id_s = init_string(id, 0);
        // find operation node
        xmlNodePtr operation_node = NULL;
        find_xml_node("operation", condition_element, &operation_node);
        enum query_value_cmp_op cmp_op = get_operation_from_node(operation_node);
        // find value node
        xmlNodePtr value_node = NULL;
        find_xml_node("value", condition_element, &value_node);
        enum query_value_type value_type = get_value_type_from_node(value_node);
        union raw_value value = get_raw_value_from_node(value_node, value_type);

        struct query_condition_element *element = malloc(sizeof(struct query_condition_element));
        element->value = value;
        element->value_type = value_type;
        element->key = id_s;
        element->op = cmp_op;

        add_elem_to_list(query_condition, element);

        condition_element = NULL;
        find_xml_node("condition_element", value_node, &condition_element);
    }

    return query_condition;
}

void add_property_to_list(struct query_repr_list *list, struct query_repr_list *property) {
    struct query_repr_list *cur = list;
    while (cur->next) cur = cur->next;
    cur->next = property;
}

struct query_repr_list *get_query_repr_list(xmlDocPtr doc) {
    struct query_repr_list *list = NULL;

    xmlNodePtr represent_body = NULL;
    find_xml_node("represent_body", xmlDocGetRootElement(doc), &represent_body);
    if (!represent_body)
        return list;


    struct query_repr_list *cur = list;
    xmlNodePtr represent_element = NULL;
    find_xml_node("represent_element", represent_body, &represent_element);
    while (represent_element) {
        cur = malloc(sizeof(struct query_repr_list));
        // find id node
        xmlNodePtr id_node = NULL;
        find_xml_node("id", represent_element, &id_node);
        char *id = (char *) id_node->children->content;
        struct string *id_s = init_string(id, 0);

        cur->property = id_s;
        if (!list)
            list = cur;
        else
            add_property_to_list(list, cur);


        xmlNodePtr tmp = represent_element;
        represent_element = NULL;
        find_xml_node("represent_element", tmp->next, &represent_element);
    }

    return list;
}

void handle_select(char *buffer, xmlDocPtr doc, struct file *file) {
    xmlNodePtr condition_id = NULL;
    find_xml_node("condition_id", xmlDocGetRootElement(doc), &condition_id);
    char *condition_elem_path = (char *) condition_id->children->content;
    struct string *condition_elem_path_s = init_string(condition_elem_path, 0);
    xmlNodePtr condition_elements = NULL;
    find_xml_node("condition_elements", xmlDocGetRootElement(doc), &condition_elements);

    struct query_condition *condition = NULL;
    if (condition_elements->children)
        condition = get_query_condition_from_node(condition_elements);

    struct query_result res = {};
    enum status status = user_read_object_property_with_condition(file, condition_elem_path_s, condition, &res);

    struct query_repr_list *repr_list = get_query_repr_list(doc);

    if (status == ERROR)
        to_xml(file, buffer, NULL, condition_elem_path_s);
    else
        to_xml(file, buffer, repr_list, condition_elem_path_s);

    free(condition);
}

enum object_type object_type_from_query_type(enum query_value_type type) {
    switch (type) {
        case Q_VAL_STRING:
            return OBJ_STRING;
        case Q_VAL_INT32:
            return OBJ_INT32;
        case Q_VAL_FLOAT:
            return OBJ_FLOAT;
        case Q_VAL_BOOL:
            return OBJ_BOOL;
        default:
            return OBJ_STRING;
    }
}

void insert_sibling(struct object *o1, struct object *o2) {
    struct object *cur = o1->child;
    while (cur->sibling) cur = cur->sibling;
    cur->sibling = o2;
}

void handle_insert(char *buffer, xmlDocPtr doc, struct file *file) {
    xmlNodePtr entity_body = NULL;
    find_xml_node("entity_body", xmlDocGetRootElement(doc), &entity_body);

    xmlNodePtr entity_id_node = NULL;
    find_xml_node("entity_id", xmlDocGetRootElement(doc), &entity_id_node);
    char *entity_id = (char *) entity_id_node->children->content;
    struct string *entity_id_s = init_string(entity_id, 0);

    struct object o = {};

    xmlNodePtr entity_elements = NULL;
    find_xml_node("entity_elements", xmlDocGetRootElement(doc), &entity_elements);
    xmlNodePtr entity_element = NULL;
    find_xml_node("entity_element", xmlDocGetRootElement(doc), &entity_element);
    while (entity_element) {

        xmlNodePtr id_node = NULL;
        find_xml_node("id", entity_element, &id_node);
        char *id = (char *) id_node->children->content;
        struct string *id_s = init_string(id, 0);

        xmlNodePtr value_node = NULL;
        find_xml_node("value", entity_element, &value_node);
        enum query_value_type value_type = get_value_type_from_node(value_node);
        union raw_value value = get_raw_value_from_node(value_node, value_type);

        struct object *o1 = malloc(sizeof(struct object));
        o1->key = id_s;
        o1->type = object_type_from_query_type(value_type);
        o1->raw_value = value;
        o1->ancestor = &o;
        if (!o.child)
            o.child = o1;
        else
            insert_sibling(&o, o1);


        xmlNodePtr tmp = entity_element;
        entity_element = NULL;
        find_xml_node("entity_element", tmp->next, &entity_element);
    }

    o.type = OBJ_OBJ;
    o.ancestor = 0;
    o.sibling = 0;

    enum status status = user_insert_object_with_path(file, &o, entity_id_s);

    struct query_repr_list *repr_list = get_query_repr_list(doc);

    if (status == ERROR)
        to_xml(file, buffer, NULL, entity_id_s);
    else
        to_xml(file, buffer, repr_list, entity_id_s);
}

void handle_update(char *buffer, xmlDocPtr doc, struct file *file) {
    xmlNodePtr condition_id = NULL;
    find_xml_node("condition_id", xmlDocGetRootElement(doc), &condition_id);
    char *condition_elem_path = (char *) condition_id->children->content;
    struct string *condition_elem_path_s = init_string(condition_elem_path, 0);
    xmlNodePtr condition_elements = NULL;
    find_xml_node("condition_elements", xmlDocGetRootElement(doc), &condition_elements);

    struct query_condition *condition = NULL;
    if (condition_elements->children)
        condition = get_query_condition_from_node(condition_elements);

    struct query_result res = {};
    enum status status = user_read_object_property_with_condition(file, condition_elem_path_s, condition, &res);

    if (status == OK && res.first_item) {
        xmlNodePtr entity_elements = NULL;
        find_xml_node("entity_elements", xmlDocGetRootElement(doc), &entity_elements);
        xmlNodePtr entity_element = NULL;
        find_xml_node("entity_element", xmlDocGetRootElement(doc), &entity_element);
        while (entity_element) {

            xmlNodePtr id_node = NULL;
            find_xml_node("id", entity_element, &id_node);
            char *id = (char *) id_node->children->content;
            struct string *id_s = init_string(id, 0);

            xmlNodePtr value_node = NULL;
            find_xml_node("value", entity_element, &value_node);
            enum query_value_type value_type = get_value_type_from_node(value_node);
            union raw_value value = get_raw_value_from_node(value_node, value_type);

            struct string *path_to_property = string_concat_with_dot(condition_elem_path_s, id_s);
            user_modify_object_property(file, path_to_property, value, value_type);

            xmlNodePtr tmp = entity_element;
            entity_element = NULL;
            find_xml_node("entity_element", tmp->next, &entity_element);
        }
    }

    struct query_repr_list *repr_list = get_query_repr_list(doc);

    if (status == ERROR)
        to_xml(file, buffer, NULL, condition_elem_path_s);
    else
        to_xml(file, buffer, repr_list, condition_elem_path_s);

    free(condition);
}

void handle_delete(char *buffer, xmlDocPtr doc, struct file *file) {
    xmlNodePtr condition_id = NULL;
    find_xml_node("condition_id", xmlDocGetRootElement(doc), &condition_id);
    char *condition_elem_path = (char *) condition_id->children->content;
    struct string *condition_elem_path_s = init_string(condition_elem_path, 0);
    xmlNodePtr condition_elements = NULL;
    find_xml_node("condition_elements", xmlDocGetRootElement(doc), &condition_elements);

    struct query_condition *condition = NULL;
    if (condition_elements->children)
        condition = get_query_condition_from_node(condition_elements);

    struct query_result res = {};
    enum status status = user_read_object_property_with_condition(file, condition_elem_path_s, condition, &res);

    if (status == OK && res.first_item) {
        user_delete_object(file, condition_elem_path_s);
    }

    to_xml(file, buffer, NULL, condition_elem_path_s);
    free(condition);
}

void handle_xml_and_return_response_at_buffer(char *buffer, struct file *f) {
    FILE *file = fopen(QUERY_FILE, "w");
    fprintf(file, "%s", buffer);
    fflush(file);

    xmlDocPtr doc = xmlReadFile(QUERY_FILE, NULL, 0);
    xmlNodePtr root = xmlDocGetRootElement(doc);

    xmlNodePtr node = NULL;
    find_xml_node("query_type", root, &node);

    if (xmlStrEqual(node->children->content, BAD_CAST "select"))
        handle_select(buffer, doc, f);
    if (xmlStrEqual(node->children->content, BAD_CAST "insert"))
        handle_insert(buffer, doc, f);
    if (xmlStrEqual(node->children->content, BAD_CAST "update"))
        handle_update(buffer, doc, f);
    if (xmlStrEqual(node->children->content, BAD_CAST "delete"))
        handle_delete(buffer, doc, f);


    xmlFreeDoc(doc);
}