#ifndef LLP_FROM_XML_H
#define LLP_FROM_XML_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include <string.h>
#include "db/file/file.h"
#include "db/query/query.h"
#include "db/user/user.h"
#include "serializer/to_xml.h"

#define QUERY_FILE "query.xml"

void handle_xml_and_return_response_at_buffer(char *buffer, struct file *f);

#endif//LLP_FROM_XML_H
