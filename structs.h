#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>
#include <sys/types.h>


struct FileInfo {
    uint numOfLines;
    uint numOfNetworks;
};


struct RawLines {
    char** lines;
    struct FileInfo fileInfo;
};


struct ParsedLines {
    char** lines;
    uint numOfLines;
};


struct ParseOutput {
    struct ParsedLines hosts;
    struct ParsedLines subnets;
};


#endif // STRUCTS_H
