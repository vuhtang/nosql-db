#include "tests/tests.h"

//"person": {
//    "age": 20;
//    "isMale": true;
//    "location": {
//        "name": "Nizniy Novgorod";
//    }
//}

void add_person_object_test(struct file *f, char *person_name) {

    struct object obj0;
    struct object obj3;

    char *loc_name = "name";
    struct string key4 = {.ptr = loc_name, .len = strlen(loc_name)};
    char *nn = "Nizhniy Novgorod";
    struct string val4 = {.ptr = nn, .len = strlen(nn)};
    struct object obj4 = {
            .key = &key4,
            .type = OBJ_STRING,
            .raw_value = {.string = val4},
            .ancestor = &obj3,
            .sibling = 0,
            .child = 0};

    char *name3 = "location";
    struct string key3 = {.ptr = name3, .len = 8};
    obj3.key = &key3;
    obj3.type = OBJ_OBJ;
    obj3.ancestor = &obj0;
    obj3.child = &obj4;
    obj3.sibling = 0;


    char *name1 = "isMale";
    bool sex = true;
    struct string key2 = {.ptr = name1, .len = 6};
    union raw_value raw_val2 = {.aBool = sex};
    struct object obj1 = {
            .key = &key2,
            .type = OBJ_BOOL,
            .raw_value = raw_val2,
            .ancestor = &obj0,
            .sibling = &obj3,
            .child = 0};

    char *name = "age";
    int32_t age = 20;
    struct string key1 = {.ptr = name, .len = 3};
    union raw_value raw_val1 = {.int32 = age};
    struct object obj = {
            .key = &key1,
            .type = OBJ_INT32,
            .raw_value = raw_val1,
            .sibling = &obj1,
            .ancestor = &obj0,
            .child = 0};

    char *person = person_name;
    struct string key0 = {.ptr = person, .len = strlen(person_name)};
    obj0.key = &key0;
    obj0.type = OBJ_OBJ;
    obj0.child = &obj;
    obj0.ancestor = 0;
    obj0.sibling = 0;

    //    char *nn = "Nizhniy Novgorod";
    //    struct string val4 = {.ptr = nn, .len = strlen(nn)};
    //    struct object objjj = {
    //            .key = &key0,
    //            .type = OBJ_string,
    //            .raw_value = {.string = val4},
    //            .sibling = NULL,
    //            .ancestor = NULL,
    //            .child = NULL
    //    };
    //    enum status add_res = user_add_object(f, &objjj);
    enum status add_res = user_add_object(f, &obj0);
    print_status(add_res);
}

void find_person_object_test(struct file *f) {
    add_person_object_test(f, "person");
    add_person_object_test(f, "person_sib");
    add_person_object_test(f, "person_sib2");

    char *path = "person_sib2";
    struct string path_str = {.ptr = path, .len = strlen(path)};
    struct query_result res = {};
    user_read_object_property(f, &path_str, &res);
}

void delete_person_sibling_test(struct file *f) {
    add_person_object_test(f, "person");
    add_person_object_test(f, "person_sib");
    add_person_object_test(f, "person_sib2");

    char *path = "person_sib";
    struct string path_str = {.ptr = path, .len = strlen(path)};
    user_delete_object(f, &path_str);

    char *path2 = "person_sib2";
    struct string path_str2 = {.ptr = path2, .len = strlen(path2)};

    struct query_result res = {};
    user_read_object_property(f, &path_str2, &res);
}

void delete_person_property_test(struct file *f) {
    add_person_object_test(f, "person");

    char *path = "person.location.name";
    struct string path_str = {.ptr = path, .len = strlen(path)};
    user_delete_object(f, &path_str);

    char *path2 = "person.location.name";
    struct string path_str2 = {.ptr = path2, .len = strlen(path2)};

    struct query_result res = {};
    user_read_object_property(f, &path_str2, &res);
}

void update_person_object_test(struct file *f) {
    add_person_object_test(f, "person");

    // modify "isMale" to false
    char *path = "person.isMale";
    struct string path_str = {.ptr = path, .len = strlen(path)};
    union raw_value val = {.aBool = true};

    user_modify_object_property(f, &path_str, val, Q_VAL_BOOL);

    char *path2 = "person.isMale";
    struct string path_str2 = {.ptr = path2, .len = strlen(path2)};
    struct query_result res = {};
    user_read_object_property(f, &path_str2, &res);

    path = "person.age";
    path_str.ptr = path;
    path_str.len = strlen(path);
    val.int32 = 21;
    user_modify_object_property(f, &path_str, val, Q_VAL_INT32);

    path2 = "person.age";
    path_str2.ptr = path2;
    path_str2.len = strlen(path2);
    struct query_result res1 = {};
    user_read_object_property(f, &path_str2, &res1);

    path = "person.location.name";
    path_str.ptr = path;
    path_str.len = strlen(path);
    char *new_name = "Moscow";
    struct string new_name_string = {.ptr = new_name, .len = strlen(new_name)};
    val.string = new_name_string;
    user_modify_object_property(f, &path_str, val, Q_VAL_STRING);

    path2 = path;
    path_str2.ptr = path2;
    path_str2.len = strlen(path2);
    struct query_result res2 = {};
    user_read_object_property(f, &path_str2, &res2);
}
