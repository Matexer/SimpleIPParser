#ifndef CONVERTING_H
#define CONVERTING_H


#include <errno.h>
#include <printf.h>
#include <stdbool.h>
#include <stdint.h>
#include "structs.h"
#include "utils.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


struct ConvertOutput parse(struct Lines* inputLines);
bool isSingleIP(stringP line);
bool isValidIP(stringP line);
bool isValidNetwork(stringP line);
bool hasValidSubnet(stringP line, uint index);
void parseLine(stringP line, struct ConvertOutput* output);
void normalizeNetwork(stringP line);


struct ConvertOutput parse(struct Lines* inputLines) {
    struct ConvertOutput output;
    uint numOfNetworks = inputLines->fileInfo.numOfNetworks;
    uint numOfHosts = inputLines->fileInfo.numOfLines - numOfNetworks;

    output.hosts.linePointers = getCharPointersTable(numOfHosts);
    output.hosts.numOfLines = 0;
    output.networks.linePointers = getCharPointersTable(numOfNetworks);
    output.networks.numOfLines = 0;

    char** rawLines = inputLines->lines;
    for (int i = 0; i < inputLines->fileInfo.numOfLines; i++) {
        parseLine(rawLines[i], &output);
    }

    return output;
}


inline void parseLine(stringP line, struct ConvertOutput* output) {
    uint index;
    if (isSingleIP(line)) {
        if (!isValidIP(line)) {
            printf("Skipping invalid IP: %s\n", line);
            return;
        }

        index = output->hosts.numOfLines;
        output->hosts.linePointers[index] = line;
        output->hosts.numOfLines++;
        return;
    }
    else if (isValidNetwork(line)) {
        normalizeNetwork(line);
        index = output->networks.numOfLines;
        output->networks.linePointers[index] = line;
        output->networks.numOfLines++;
        return;
    }
}


inline bool isSingleIP(stringP line) {
    char* position = strchr(line, '/');
    if (position == NULL) {
        return true;
    }
    position = NULL;

    return false;
}


inline bool isValidIP(stringP line) {
    struct sockaddr_in tmp_sockaddr;
    return (inet_pton(AF_INET, line, &(tmp_sockaddr.sin_addr)) == 1);
}


inline bool isValidNetwork(stringP line) {
    struct sockaddr_in tmp_sockaddr;
    char* position = strchr(line, '/');

    if (position == NULL) {
        printf("Skipping invalid line: %s\n", line);
        return false;
    }

    uint index = position - line;

    char ipPart[INET_ADDRSTRLEN];
    strncpy(ipPart, line, index);
    ipPart[index] = '\0';

    if (!isValidIP(ipPart)) {
        printf("Skipping invalid IP: %s\n", line);
        return false;
    }

    if (!hasValidSubnet(line, index + 1)) {

        return false;
    }

    return true;
}


inline bool hasValidSubnet(stringP line, uint index) {
    char subnetPart[3];
    char *subnetEnd;
    uint subnetLength = strlen(line) - index;

    if (subnetLength > 2 || subnetLength < 1) {
        printf("Skipping inproper subnet: %s\n", line);
        return false;
    }

    strncpy(subnetPart, line + index, subnetLength);
    subnetPart[subnetLength] = '\0';
    long parsedInt = strtol(subnetPart, &subnetEnd, 10);

    if (subnetEnd == subnetPart || *subnetEnd != '\0'){
        printf("Skipping invalid subnet: %s\n", line);
        return false;
    }

    if (parsedInt == 0 && errno == EINVAL) {
        printf("Skipping subnet which is not an integer: %s\n", line);
        return false;
    }

    if (parsedInt > 32 || parsedInt < 0) {
        printf("Skipping subnet with inproper value: %s\n", line);
        return false;
    }

    return true;
}


void normalizeNetwork(stringP line) {
    char* slash = strchr(line, '/');
    char* lastDot = strrchr(line, '.');

    uint lastDotPosition = lastDot - line;
    uint slashPosition = slash - line;

    char* newLine = malloc(strlen(line) * sizeof(char) + 100);
    strncpy(newLine, line, lastDotPosition + 1);
    strcat(newLine, "0");
    strcat(newLine, slash);

    memcpy(line, newLine, strlen(line));
    free(newLine);
}


#endif // CONVERTING_H
