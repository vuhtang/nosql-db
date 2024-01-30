#include "serializer/xml.h"

xmlDocPtr create_base_XML_document() {
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr rootNode = xmlNewNode(NULL, BAD_CAST "Root");

    xmlDocSetRootElement(doc, rootNode);

    return doc;
}

char *xml_to_str(xmlDocPtr doc) {
    xmlChar *xmlbuff;
    int buffersize;
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    char *result = (char *) malloc(buffersize + 1);
    snprintf(result, buffersize + 1, "%s", xmlbuff);
    xmlFree(xmlbuff);
    return result;
}

void fill_condition_body(xmlNodePtr condition_body, struct ast_node *condition_ast) {
    char* id = condition_ast->children[0]->value.string_value;
    xmlNodePtr condition_id = xmlNewNode(NULL, BAD_CAST "condition_id");
    xmlNodeSetContent(condition_id, BAD_CAST id);
    xmlAddChild(condition_body, condition_id);

    if (condition_ast->children_count == 1) return ;
    xmlNodePtr condition_elements = xmlNewNode(NULL, BAD_CAST "condition_elements");
    xmlAddChild(condition_body, condition_elements);

    struct ast_node *conditions = condition_ast->children[1]->children[0];
    if (!conditions) return ;
    size_t conditions_count = conditions->children_count;
    for (size_t i = 0; i < conditions_count; ++i) {
        xmlNodePtr condition_element = xmlNewNode(NULL, BAD_CAST "condition_element");
        struct ast_node *element = conditions->children[i];
        xmlAddChild(condition_elements, condition_element);

        char *cond_id = element->children[0]->value.string_value;
        char *op = get_op_from_ast_node(element);
        char *value = get_char_value_from_ast_node(element->children[1]);
        xmlNewChild(condition_element, NULL, BAD_CAST "id", BAD_CAST cond_id);
        xmlNewChild(condition_element, NULL, BAD_CAST "operation", BAD_CAST op);
        xmlNewChild(condition_element, NULL, BAD_CAST "value", BAD_CAST value);
    }

}

void fill_represent_body(xmlNodePtr represent_body, struct ast_node *represent_ast) {
    struct ast_node *represents = represent_ast->children[0];
    if (!represents) return ;
    size_t represents_count = represents->children_count;

    for (size_t i = 0; i < represents_count; ++i) {
        struct ast_node *element = represents->children[i]->children[0];
        xmlNodePtr represent_element = xmlNewNode(NULL, BAD_CAST "represent_element");
        xmlNodePtr id = xmlNewNode(NULL, BAD_CAST "id");
        xmlAddChild(represent_element, id);
        xmlNodeSetContent(id, BAD_CAST element->value.string_value);
        xmlAddChild(represent_body, represent_element);
    }
}

void fill_entity_body(xmlNodePtr entity_body, struct ast_node *entity_ast) {
    char* id = entity_ast->children[0]->value.string_value;
    xmlNodePtr entity_id = xmlNewNode(NULL, BAD_CAST "entity_id");
    xmlNodeSetContent(entity_id, BAD_CAST id);
    xmlAddChild(entity_body, entity_id);

    if (entity_ast->children_count == 1) return ;
    xmlNodePtr entity_elements = xmlNewNode(NULL, BAD_CAST "entity_elements");
    xmlAddChild(entity_body, entity_elements);

    struct ast_node *entity_fields = entity_ast->children[1]->children[0]->children[0];
    size_t fields_count = entity_fields->children_count;
    for (size_t i = 0; i < fields_count; ++i) {
        xmlNodePtr entity_element = xmlNewNode(NULL, BAD_CAST "entity_element");
        struct ast_node *element = entity_fields->children[i];
        xmlAddChild(entity_elements, entity_element);

        char *field_id = element->children[0]->value.string_value;
        char *value = get_char_value_from_ast_node(element->children[1]);
        xmlNewChild(entity_element, NULL, BAD_CAST "id", BAD_CAST field_id);
        xmlNewChild(entity_element, NULL, BAD_CAST "value", BAD_CAST value);
    }
}

char *update_query_to_xml(xmlDocPtr doc) {
    xmlNodePtr root = xmlDocGetRootElement(doc);
    xmlNewChild(root, NULL, BAD_CAST "query_type", BAD_CAST "update");

    xmlNodePtr condition_body =  xmlNewNode(NULL, BAD_CAST "condition_body");
    xmlNodePtr entity_body =  xmlNewNode(NULL, BAD_CAST "entity_body");
    xmlNodePtr represent_body =  xmlNewNode(NULL, BAD_CAST "represent_body");
    xmlAddChild(root, condition_body);
    xmlAddChild(root, entity_body);
    xmlAddChild(root, represent_body);

    struct ast_node *root_node = get_root_ast_node();
    struct ast_node *condition_body_ast = get_condition_body_node(root_node);
    struct ast_node *entity_body_ast = get_entity_body_node(root_node);
    struct ast_node *represent_body_ast = get_represent_body_node(root_node);

    fill_condition_body(condition_body, condition_body_ast);
    fill_entity_body(entity_body, entity_body_ast);
    fill_represent_body(represent_body, represent_body_ast);

    return xml_to_str(doc);
}

char *select_query_to_xml(xmlDocPtr doc) {
    xmlNodePtr root = xmlDocGetRootElement(doc);
    xmlNewChild(root, NULL, BAD_CAST "query_type", BAD_CAST "select");

    xmlNodePtr condition_body =  xmlNewNode(NULL, BAD_CAST "condition_body");
    xmlNodePtr represent_body =  xmlNewNode(NULL, BAD_CAST "represent_body");
    xmlAddChild(root, condition_body);
    xmlAddChild(root, represent_body);

    struct ast_node *root_node = get_root_ast_node();
    struct ast_node *condition_body_ast = get_condition_body_node(root_node);
    struct ast_node *represent_body_ast = get_represent_body_node(root_node);

    fill_condition_body(condition_body, condition_body_ast);
    fill_represent_body(represent_body, represent_body_ast);

    return xml_to_str(doc);
}

char *insert_query_to_xml(xmlDocPtr doc) {
    xmlNodePtr root = xmlDocGetRootElement(doc);
    xmlNewChild(root, NULL, BAD_CAST "query_type", BAD_CAST "insert");

    xmlNodePtr entity_body =  xmlNewNode(NULL, BAD_CAST "entity_body");
    xmlNodePtr represent_body =  xmlNewNode(NULL, BAD_CAST "represent_body");
    xmlAddChild(root, entity_body);
    xmlAddChild(root, represent_body);

    struct ast_node *root_node = get_root_ast_node();
    struct ast_node *entity_body_ast = get_entity_body_node(root_node);
    struct ast_node *represent_body_ast = get_represent_body_node(root_node);

    fill_entity_body(entity_body, entity_body_ast);
    fill_represent_body(represent_body, represent_body_ast);
    return xml_to_str(doc);
}

char *delete_query_to_xml(xmlDocPtr doc) {
    xmlNodePtr root = xmlDocGetRootElement(doc);
    xmlNewChild(root, NULL, BAD_CAST "query_type", BAD_CAST "delete");

    xmlNodePtr condition_body =  xmlNewNode(NULL, BAD_CAST "condition_body");
    xmlNodePtr represent_body =  xmlNewNode(NULL, BAD_CAST "represent_body");
    xmlAddChild(root, condition_body);
    xmlAddChild(root, represent_body);

    struct ast_node *root_node = get_root_ast_node();
    struct ast_node *condition_body_ast = get_condition_body_node(root_node);
    struct ast_node *represent_body_ast = get_represent_body_node(root_node);

    fill_condition_body(condition_body, condition_body_ast);
    fill_represent_body(represent_body, represent_body_ast);

    return xml_to_str(doc);
}

char *to_xml() {
    xmlDocPtr doc = create_base_XML_document();
    char *out;

    switch (get_root_ast_node()->type) {
        case AST_NODE_QUERY_UPDATE:
            out = update_query_to_xml(doc);
            break ;
        case AST_NODE_QUERY_INSERT:
            out = insert_query_to_xml(doc);
            break ;
        case AST_NODE_QUERY_SELECT:
            out = select_query_to_xml(doc);
            break ;
        case AST_NODE_QUERY_DELETE:
            out = delete_query_to_xml(doc);
            break ;
        default:
            out = "There is no such query type, please try again\n";
    }
    xmlFreeDoc(doc);
    return out;
}