#ifndef LLP_TO_XML_H
#define LLP_TO_XML_H

#include "db/query/query.h"
#include "libxml/parser.h"
#include "transfer/socket.h"
#include "db/user/user.h"

void to_xml(struct file *file, char *buffer, struct query_repr_list *repr_list, struct string *path_to_object);

#endif//LLP_TO_XML_H
