#ifndef UTILS_H
#define UTILS_H

#include "structs.h"
#include <sys/types.h>
#include <malloc.h>


char** getCharTable(uint numOfRows, uint rowLength) {
    char** table;
    table = malloc(numOfRows * sizeof(char*));
    table[0] = malloc(numOfRows * BYTES_PER_LINE * sizeof(char));

    for (int i = 1; i < numOfRows; i++) {
        table[i] = table[i - 1] + BYTES_PER_LINE;
    }

    return table;
}


char* getCharPointersTable(uint numOfRows) {
    char* table;
    table = malloc(numOfRows * sizeof(char*));

    return table;
}

#endif // UTILS_H
