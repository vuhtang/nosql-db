//
// Created by Иван Варюхин on 26.09.2023.
//
#include "file/file_interaction.h"

enum open_file_status open_file(FILE **pFile, const char *filename, const char *mode) {
    *pFile = fopen(filename, mode);
    return pFile ? OPEN_SUCCESS : OPEN_FAILED;
}