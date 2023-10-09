//
// Created by Иван Варюхин on 26.09.2023.
//

#ifndef LLP_FILE_INTERACTION_H
#define LLP_FILE_INTERACTION_H

#include <stdio.h>

enum open_file_status {
    OPEN_SUCCESS = 0,
    OPEN_FAILED
};

enum close_file_status {
    CLOSE_SUCCESS = 0,
    CLOSE_FAILED
};

enum open_file_status open_file(FILE **pFile, const char *filename, const char *mode);
enum close_file_status close_file(FILE *pFile);



#endif//LLP_FILE_INTERACTION_H
