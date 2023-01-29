#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>

#define BYTES_PER_LINE 20

typedef char* stringP;


struct FileInfo {
    uint32_t numOfLines;
    uint32_t numOfNetworks;
};


struct Lines {
    char** lines;
    struct FileInfo fileInfo;
};


struct ParsedLines {
    char** linePointers;
    uint32_t numOfLines;
};


struct ConvertOutput {
    struct ParsedLines hosts;
    struct ParsedLines networks;
};


#endif // STRUCTS_H
