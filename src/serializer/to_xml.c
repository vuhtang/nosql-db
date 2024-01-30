#include "serializer/to_xml.h"

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

char *object_type_to_char(enum object_type type) {
    switch (type) {
        case OBJ_STRING:
            return "string";
        case OBJ_FLOAT:
            return "float";
        case OBJ_INT32:
            return "int32";
        case OBJ_BOOL:
            return "boolean";
        case OBJ_OBJ:
            return "object";
    }
}

char *raw_value_to_char(union raw_value value, enum object_type type) {
    switch (type) {
        case OBJ_OBJ:
            return "object";
        case OBJ_BOOL:
            return value.aBool == true ? "true" : "false";
        case OBJ_INT32: {
            char *buf;
            sprintf(buf, "%d", value.int32);
            return buf;
        }
        case OBJ_FLOAT: {
            char *buf;
            sprintf(buf, "%f", value.aFloat);
            return buf;
        }
        case OBJ_STRING:
            return value.string.ptr;
    }
}

void to_xml(struct file *file, char *buffer, struct query_repr_list *repr_list, struct string *path_to_object) {
    xmlDocPtr doc = create_base_XML_document();
    xmlNodePtr root_node = xmlDocGetRootElement(doc);

    xmlNewChild(root_node, NULL, BAD_CAST "object_id", BAD_CAST path_to_object->ptr);
    xmlNodePtr object_properties = xmlNewChild(root_node, NULL, BAD_CAST "object_properties", NULL);

    struct query_repr_list *cur = repr_list;
    while (cur) {
        struct string *cur_property = cur->property;
        struct string *path_to_property = string_concat_with_dot(path_to_object, cur_property);

        struct query_result res1 = {};
        user_read_object_property(file, path_to_property, &res1);
        if (!res1.first_item) {
            cur = cur->next;
            continue;
        }
        struct object *o = res1.first_item->object;

        xmlNodePtr object_property = xmlNewChild(object_properties, NULL, BAD_CAST "object_property", NULL);
        object_property->type = 0;

        xmlNewChild(object_property, NULL, BAD_CAST "id", BAD_CAST o->key->ptr);
        xmlNewChild(object_property, NULL, BAD_CAST "type", BAD_CAST object_type_to_char(o->type));
        xmlNewChild(object_property, NULL, BAD_CAST "value", BAD_CAST raw_value_to_char(o->raw_value, o->type));

        cur = cur->next;
    }


    char *out = xml_to_str(doc);
    memset(buffer, 0, MAX_BUFFER_SIZE);
    strcpy(buffer, out);

    xmlFreeDoc(doc);
}