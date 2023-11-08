#include <stdio.h>
#include "file/file_interaction.h"
#include <section/section_header.h>

int main(int argc, char** argv) {

    if (argc > 1) {
        FILE *fp;

        enum open_file_status open_status = open_file(&fp, argv[1], "rwb");
        if (open_status == OPEN_FAILED) {
            printf("file hasn't been opened.. \n");
            return OPEN_FAILED;
        }

        struct File *f = init_file(fp);

    }
    return 0;
}
