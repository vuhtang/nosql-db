#include "tests/tests.h"

struct object *create_object() {
    return malloc(sizeof(struct object));
}

void init_object(struct object *obj, struct string *key, enum object_type type, union raw_value value,
                 struct object *ancestor, struct object *child, struct object *sibling) {
    obj->key = key;
    obj->type = type;
    obj->raw_value = value;
    obj->ancestor = ancestor;
    obj->child = child;
    obj->sibling = sibling;
}

struct object *create_and_init_object(struct string *key, enum object_type type, union raw_value value,
                                      struct object *ancestor, struct object *child, struct object *sibling) {
    struct object *o = create_object();
    init_object(o, key, type, value, ancestor, child, sibling);
    return o;
}

//obj0 "person": {
//    obj1 "age": 20;
//    obj2 "isMale": true;
//    obj3 "location": {
//        obj4 "name": "Nizniy Novgorod";
//        obj5 "lat": 50.33;
//    }
//}

struct object *create_person_object(char *person_name) {
    struct object *obj0 = create_object();
    struct object *obj1 = create_object();
    struct object *obj2 = create_object();
    struct object *obj3 = create_object();
    struct object *obj4 = create_object();
    struct object *obj5 = create_object();

    char *lat = "lat";
    struct string *key5 = init_string(lat, 0);
    union raw_value val5 = {.aFloat = 50.33};
    init_object(obj5, key5, OBJ_FLOAT, val5, obj3, 0, 0);

    char *loc_name = "name";
    struct string *key4 = init_string(loc_name, 0);
    char *nn = "Nizhniy Novgorod";
    struct string *val4 = init_string(nn, 0);
    init_object(obj4, key4, OBJ_STRING, (union raw_value) * val4, obj3, 0, obj5);

    char *name3 = "location";
    struct string *key3 = init_string(name3, 0);
    union raw_value val3 = {};
    init_object(obj3, key3, OBJ_OBJ, val3, obj0, obj4, 0);

    char *name2 = "isMale";
    bool sex = true;
    struct string *key2 = init_string(name2, 0);
    union raw_value val2 = {.aBool = sex};
    init_object(obj2, key2, OBJ_BOOL, val2, obj0, 0, obj3);

    char *name1 = "age";
    int32_t age = 20;
    struct string *key1 = init_string(name1, 0);
    union raw_value val1 = {.int32 = age};
    init_object(obj1, key1, OBJ_INT32, val1, obj0, 0, obj2);

    char *person = person_name;
    struct string *key0 = init_string(person, 0);
    union raw_value val0 = {};
    init_object(obj0, key0, OBJ_OBJ, val0, 0, obj1, 0);

    return obj0;
}

void delete_object(struct object *o) {
    if (o->sibling)
        delete_object(o->sibling);

    if (o->child)
        delete_object(o->child);

    free_string(o->key);
    free(o);
}

void add_person_object_test(struct file *f, char *person_name) {
    //    struct object *obj0 = create_person_object(person_name);

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

    enum status add_res = user_insert_object(f, &obj0);
}
void add_person_object_test_with_anc(struct file *f, char *person_name, struct object *ancestor) {
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
    obj0.ancestor = ancestor;
    obj0.sibling = 0;

    enum status add_res = user_insert_object(f, &obj0);
}

void find_person_object_test(struct file *f) {
    add_person_object_test(f, "person");
    add_person_object_test(f, "person_sib");
    add_person_object_test(f, "person_sib2");

    char *path = "person_sib2.location.name";
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


void test_insert(struct file *f) {

    int d = 10000;
    int max = 100000;
    clock_t start, end;
    double t;
    char *p_name = malloc(sizeof(char) * 20);
    start = clock();
    for (int num_of_iter = 0; num_of_iter < max; num_of_iter += d) {
        printf("iterations: %d, ", num_of_iter + d);
        for (int i = 0; i < d; ++i) {
            sprintf(p_name, "person %d", i);
            add_person_object_test(f, p_name);
        }
        end = clock();
        t = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("time: %f", t);
        printf("\n");
    }
    free(p_name);
}

void test_read(struct file *f) {

    int c = 100000;
    clock_t start, end;
    double t;
    char *p_name = malloc(sizeof(char) * 20);

    for (int i = 0; i < c; ++i) {
        sprintf(p_name, "person %d", i);
        add_person_object_test(f, p_name);
    }

    struct string path = {};
    path.ptr = p_name;

    start = clock();

    sprintf(p_name, "person %d", c - 1);
    path.len = strlen(path.ptr);
    struct query_result res = {};
    user_read_object_property(f, &path, &res);

    end = clock();
    t = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("time: %f", t);

    free(p_name);
}

void test_update(struct file *f) {

    int c = 100000;
    clock_t start, end;
    double t;
    char *p_name = malloc(sizeof(char) * 20);

    for (int i = 0; i < c; ++i) {
        sprintf(p_name, "person %d", i);
        add_person_object_test(f, p_name);
    }


    int d = 10000;
    for (int i = d; i <= c; i += d) {

        start = clock();
        struct string path = {};
        union raw_value value = {.int32 = 21};
        path.ptr = p_name;

        sprintf(p_name, "person %d.age", i - 1);
        path.len = strlen(path.ptr);
        user_modify_object_property(f, &path, value, Q_VAL_INT32);

        end = clock();
        t = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("time: %f \n", t);
    }


    free(p_name);
}

void test_delete(struct file *f) {

    int c = 100000;
    clock_t start, end;
    double t;
    char *p_name = malloc(sizeof(char) * 20);

    for (int i = 0; i < c; ++i) {
        sprintf(p_name, "person %d", i);
        add_person_object_test(f, p_name);
    }


    int d = 10000;
    for (int i = d; i <= c; i += d) {

        start = clock();
        struct string path = {};
        path.ptr = p_name;

        sprintf(p_name, "person %d.age", i - 1);
        path.len = strlen(path.ptr);
        user_delete_object(f, &path);

        end = clock();
        t = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("time: %f \n", t);
    }


    free(p_name);
}

void mega_test(struct file *f) {
    int c = 1000;
    char *p_name = malloc(sizeof(char) * 30);
    for (int i = 0; i < c; ++i) {
        sprintf(p_name, "person %d", i);
        add_person_object_test(f, p_name);
    }

    FILE *out = fopen("mega_test.txt", "w");

    clock_t start, end;
    double t;
    struct string path = {};
    path.ptr = p_name;

    while (true) {
        int i500 = 500;
        int i400 = 400;
        int a;

        char **tree = malloc(sizeof(char *) * (RAND_MAX % 1000 + 1));
        for (int i = 0; i < RAND_MAX % 1000; ++i) {
            sprintf(p_name, "person %d", i);
            //            char *ptr = tree[i];
            tree[i] = malloc(sizeof(char) * strlen(p_name));
            strcpy(tree[i], p_name);
        }

        char *ppp = tree[10];

        start = clock();
        for (int i = 1; i <= i500; ++i) {
            a = rand() % 10;
            sprintf(p_name, "person %d", a);
            char *ptr = tree[a];
            if (strlen(ptr) > 0 && ptr[0] == 'p') {
                path.ptr = ptr;
                path.len = strlen(path.ptr);
                struct query_result res = {};
                user_read_object_property(f, &path, &res);
                if (res.first_item) {
                    struct object *anc = res.first_item->object;
                    add_person_object_test_with_anc(f, p_name, anc);
                    ptr = realloc(ptr, path.len + strlen(p_name) + 1);
                    strcat(ptr, ".");
                    strcat(ptr, p_name);
                }
            } else {
                ptr = malloc(strlen(p_name));
                strcpy(ptr, p_name);
                add_person_object_test(f, p_name);
            }

            if (i % 100 == 0) {
                end = clock();
                t = ((double) (end - start)) / CLOCKS_PER_SEC;
                fprintf(out, "%f\n", t);
                fflush(out);
                start = clock();
            }
        }

        start = clock();
        for (int i = 1; i <= i400; ++i) {
            a = rand() % 10;
            char *ptr = tree + sizeof(char) * a;
            path.ptr = ptr;
            path.len = strlen(path.ptr);
            user_delete_object(f, &path);
            ptr[0] = '\0';
            if (i % 100 == 0) {
                end = clock();
                t = ((double) (end - start)) / CLOCKS_PER_SEC;
                fprintf(out, "%f\n", t);
                fflush(out);
                start = clock();
            }
        }
    }
}
