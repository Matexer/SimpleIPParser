#ifndef UTILS_H
#define UTILS_H


#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <malloc.h>


void signalMemoryError();


char** getCharTable(uint numOfRows, uint rowLength) {
    char** table;
    table = malloc(numOfRows * sizeof(char*));
    table[0] = malloc(numOfRows * rowLength * sizeof(char));

    if (table == NULL || table[0] == NULL) {
        signalMemoryError();
    }

    for (uint i = 1; i < numOfRows; i++) {
        table[i] = table[i - 1] + rowLength;
    }

    return table;
}


char** getCharPointersTable(uint numOfRows) {
    char** table;
    table = malloc(numOfRows * sizeof(char*));

    if (table == NULL) {
        signalMemoryError();
    }

    return table;
}


inline void signalMemoryError() {
    printf("Not enough memory. Operation aborted");
    exit(1);
}


#endif // UTILS_H
