#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H


#include "consts.h"
#include "structs.h"
#include "memoryUtils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <malloc.h>


struct FileInfo getFileInfo(const char* filePath);


bool doesFileExists(const char* filePath) {
    return (access(filePath, F_OK) == 0);
}


void saveToFile(const char* filePath, const struct ParseOutput *output) {
    FILE* fp = fopen(filePath, "w");

    for (int i = 0; i < output->subnets.numOfLines; i++) {
        fprintf(fp, "network %s,\n", output->subnets.lines[i]);
    }

    for (int i = 0; i < output->hosts.numOfLines; i++) {
        fprintf(fp, "host %s,\n", output->hosts.lines[i]);
    }

    fclose(fp);
}


struct RawLines getDataFromFile(const char* filePath) {
    struct FileInfo fileInfo = getFileInfo(filePath);

    char** lines = getCharTable(fileInfo.numOfLines, CHARS_PER_LINE);
    char* lineBuffer = malloc(CHARS_PER_LINE + 1); //+1 for \n

    ssize_t lineLength;
    size_t lineSize = CHARS_PER_LINE + 1; //+1 for \n
    uint lineNumber = 0;

    FILE* fp = fopen(filePath, "r");

    while ((lineLength = getline(&lineBuffer, &lineSize, fp)) != -1) {
        //Too many chars for valid ip/network
        if (lineLength > CHARS_PER_LINE) {
            fileInfo.numOfLines--;
            printf("Skipping invalid line: %s", lineBuffer);
            continue;
        }

        if (lineBuffer[lineLength-1] == '\n') {
            lineLength--;
        }

        //Blank line. Silent skip.
        if (lineLength < 1) {
            fileInfo.numOfLines--;
            continue;
        }

        memcpy(lines[lineNumber], lineBuffer, lineLength);
        lines[lineNumber][lineLength] = '\0';
        lineNumber++;
    }

    free(lineBuffer);
    lineBuffer = NULL;

    fclose(fp);

    struct RawLines rawLines = {lines, fileInfo};

    return rawLines;
}


struct FileInfo getFileInfo(const char* filePath) {
    FILE* fp = fopen(filePath, "r");
    struct FileInfo fileInfo;

    char ch = 0;
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
