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

enum open_file_status open_file(FILE **pFile, const char *filename, const char *mode);

#endif//LLP_FILE_INTERACTION_H
