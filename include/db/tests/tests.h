#ifndef LLP_TESTS_H
#define LLP_TESTS_H

#include "db/file/file.h"
#include "db/file/file_interaction.h"
#include "db/section/section_interaction.h"
#include "stdlib.h"
#include "db/types/types.h"
#include "db/user/user.h"
#include <time.h>

void add_person_object_test(struct file *f, char *person_name);
void delete_person_sibling_test(struct file *f);
void update_person_object_test(struct file *f);
void find_person_object_test(struct file *f);
void delete_person_property_test(struct file *f);
void test_insert(struct file *f);
void test_read(struct file *f);
void test_update(struct file *f);
void test_delete(struct file *f);
void mega_test(struct file *f);

#endif//LLP_TESTS_H
