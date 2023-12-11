#ifndef LLP_TESTS_H
#define LLP_TESTS_H

#include "file/file.h"
#include "file/file_interaction.h"
#include "section/section_interaction.h"
#include "types/types.h"
#include "user/user.h"

void add_person_object_test(struct file *f, char *person_name);
void delete_person_sibling_test(struct file *f);
void update_person_object_test(struct file *f);
void find_person_object_test(struct file *f);
void delete_person_property_test(struct file *f);

#endif//LLP_TESTS_H
