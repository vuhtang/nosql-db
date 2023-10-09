#include <stdio.h>
#include "file/file_interaction.h"
#include "section/section_header.h"

int main(int argc, char** argv) {

    if (argc > 1) {
        FILE *fp;

        enum open_file_status open_status = open_file(&fp, argv[1], "rw");
        if (open_status == OPEN_FAILED) {
            printf("file hasn't been opened..");
            return OPEN_FAILED;
        }

        fseek(fp, 0L, SEEK_END);
        size_t size = ftell(fp);
        int fd = fileno(fp);

        char *p = mmap(NULL, 8096,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0);
        for (size_t i = 0; i < size; ++i) {
            printf("%c", p[i]);
        }
//        void *addr = init_section_header(fp);
//        int res = delete_section_header(addr);
//
//        enum close_file_status close_status = close_file(fp);
//        if (close_status == CLOSE_FAILED) {
//            printf("file hasn't been closed successfully");
//            return CLOSE_FAILED;
//        }
//        printf("cool");
    }
    return 0;
}
