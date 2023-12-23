#include "db/tests/tests.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

    int fd = open("file.bin", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        printf("couldn't get file size\n");
        return -1;
    }

    struct file *f;

    if (sb.st_size == 0) {
        f = init_file(fd);
        printf("file created\n");
    } else {
        f = read_file(fd);
        printf("file was read\n");
    }

    test_read(f);

    del_file(f);
    return 0;
}