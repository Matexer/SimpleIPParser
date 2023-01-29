#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H

#include "structs.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <malloc.h>


bool doesFileExists(stringP filePath);
struct Lines loadFile(stringP filePath);
struct FileInfo getFileInfo(stringP filePath);
void saveToFile(stringP filePath, struct ConvertOutput *output);


bool doesFileExists(stringP filePath) {
    if (access(filePath, F_OK) == 0) {
        return true;
    }
    else {
        return false;
    }
}


void saveToFile(stringP filePath, struct ConvertOutput *output) {
    FILE* fp = fopen(filePath, "w");

    printf("Printing %i hosts and %i networks", output->hosts.numOfLines, output->networks.numOfLines);

    for (int i = 0; i < output->networks.numOfLines; i++) {
        fprintf(fp, "network %s,\n", output->networks.linePointers[i]);
    }

    for (int i = 0; i < output->hosts.numOfLines; i++) {
        fprintf(fp, "host %s,\n", output->hosts.linePointers[i]);
    }

    fclose(fp);
}


struct Lines loadFile(stringP filePath) {
    struct FileInfo fileInfo = getFileInfo(filePath);
    char** lines = getCharTable(fileInfo.numOfLines, BYTES_PER_LINE);

    uint lineNumber = 0;
    size_t len = 0;
    ssize_t numOfChars;
    char* line = NULL;
    FILE* fp = fopen(filePath, "r");
    while ((numOfChars = getline(&line, &len, fp)) != -1) {
        if (line[numOfChars-1] == '\n')
            numOfChars--;
        memcpy(lines[lineNumber], line, numOfChars);
        lines[lineNumber][numOfChars] = '\0';
        lineNumber++;
    }

    fclose(fp);

    if (line) {
        free(line);
    }

    struct Lines linesOutput = {lines, fileInfo};

    return linesOutput;
}


struct FileInfo getFileInfo(stringP filePath) {
    FILE* fp = fopen(filePath, "r");
    struct FileInfo fileInfo;

    int ch = 0;
    fileInfo.numOfLines = 0;
    fileInfo.numOfNetworks = 0;

    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '/') {
            fileInfo.numOfNetworks++;
        }
        if (ch == '\n') {
            fileInfo.numOfLines++;
        }
    }

    fclose(fp);

    return fileInfo;
}



#endif // FILEMANAGEMENT_H
