#ifndef CONVERTING_H
#define CONVERTING_H


#include <errno.h>
#include <printf.h>
#include <stdbool.h>
#include <stdint.h>
#include "structs.h"
#include "memoryUtils.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct ParseOutput parse(const struct RawLines* inputLines);
bool isSingleIP(const char* line);
bool isValidIP(const char* line);
bool isValidSubnet(const char* line);
bool hasValidMask(const char* line, uint index);
void parseLine(char* line, struct ParseOutput* output);
void normalizeSubnet(char* line);


struct ParseOutput parse(const struct RawLines* inputLines) {
    struct ParseOutput output;
    uint numOfNetworks = inputLines->fileInfo.numOfNetworks;
    uint numOfHosts = inputLines->fileInfo.numOfLines - numOfNetworks;

    output.hosts.lines = getCharPointersTable(numOfHosts);
    output.hosts.numOfLines = 0;
    output.subnets.lines = getCharPointersTable(numOfNetworks);
    output.subnets.numOfLines = 0;

    for (uint i = 0; i < inputLines->fileInfo.numOfLines; i++) {
        parseLine(inputLines->lines[i], &output);
    }

    return output;
}


inline void parseLine(char* line, struct ParseOutput* output) {
    uint index;
    if (isSingleIP(line)) {
        if (!isValidIP(line)) {
            printf("Skipping invalid IP: %s\n", line);

            return;
        }

        index = output->hosts.numOfLines;
        output->hosts.lines[index] = line;
        output->hosts.numOfLines++;
    }
    else if (isValidSubnet(line)) {
        normalizeSubnet(line);
        index = output->subnets.numOfLines;
        output->subnets.lines[index] = line;
        output->subnets.numOfLines++;
    }
}


inline bool isSingleIP(const char* line) {
    char* slash = strrchr(line, '/');
    return (slash == NULL);
}


inline bool isValidIP(const char* line) {
    struct sockaddr_in tmp_sockaddr;
    return (inet_pton(AF_INET, line, &(tmp_sockaddr.sin_addr)) == 1);
}


inline bool isValidSubnet(const char* line) {
    struct sockaddr_in tmp_sockaddr;
    char* fromSlashPart = strrchr(line, '/');

    if (fromSlashPart == NULL) {
        printf("Skipping invalid line: %s\n", line);
        return false;
    }

    uint slashIndex = fromSlashPart - line;

    char ipPart[INET_ADDRSTRLEN];
    strncpy(ipPart, line, slashIndex);
    ipPart[slashIndex] = '\0';

    if (!isValidIP(ipPart)) {
        printf("Skipping subnet. Inproper IP part: %s\n", line);
        return false;
    }

    if (!hasValidMask(line, slashIndex + 1)) {
        return false;
    }

    return true;
}


inline bool hasValidMask(const char* line, uint slashIndex) {
    uint maskLength = strlen(line) - slashIndex;

    //Mask too long to be proper
    if (maskLength > 2 || maskLength < 1) {
        goto invalidMask;
    }

    char maskPart[3];
    char *maskEnd;

    strcpy(maskPart, line + slashIndex);
    maskPart[maskLength] = '\0';

    long parsedInt = strtol(maskPart, &maskEnd, 10);

    //Successfully prased to int but not equal to parsed string
    if (maskEnd == maskPart || *maskEnd != '\0'){
        goto invalidMask;
    }

    //Cannot parse to int
    if (parsedInt == 0 && errno == EINVAL) {
        goto invalidMask;
    }

    //Inproper mask value
    if (parsedInt > 32 || parsedInt < 0) {
        goto invalidMask;
    }

    return true;

    invalidMask:
    printf("Skipping subnet. Inproper mask: %s\n", line);
    return false;
}


void normalizeSubnet(char* line) {
    char* fromLastDotPart = strrchr(line, '.');

    //Already in normalized form (fully mask validity checked before)
    if (fromLastDotPart[1] == '0') {
        return;
    }

    fromLastDotPart[1] = '0';
    char* fromSlashPart = strrchr(line, '/');

    //No further modification required
    if ((fromSlashPart - fromLastDotPart) == 2) {
        return;
    }

    uint slashPartLength = strlen(line) - (fromSlashPart - line) + 1; //+1 for \0

    char tmpSlashPart[4];
    memcpy(tmpSlashPart, fromSlashPart, slashPartLength);
    memcpy(fromLastDotPart + 2, tmpSlashPart, slashPartLength);
}


#endif // CONVERTING_H
