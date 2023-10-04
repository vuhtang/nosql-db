#include <stdio.h>
#include "file/file_interaction.h"

int main(int argc, char** argv) {

    if (argc > 1) {
        FILE *fp;
        enum open_file_status status = open_file(&fp, argv[1], "rw");
        if (status == OPEN_FAILED)
            printf("file hasn't been opened..");

    }
    return 0;
}
