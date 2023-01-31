#include <stdio.h>
#include "fileManagement.h"
#include "structs.h"
#include "parsing.h"


bool areArgumentsSufficient(int argc, char *argv[]);


int main(int argc, char *argv[])
{
    const char* inputFilePath = argv[1];
    const char* outputFilePath = argv[2];

    if(!areArgumentsSufficient(argc, argv)) {
        return 1;
    }

    struct RawLines inputLines = getDataFromFile(inputFilePath);
    struct ParseOutput parseOutput = parse(&inputLines);

    saveToFile(outputFilePath, &parseOutput);

    free(parseOutput.hosts.lines);
    free(parseOutput.subnets.lines);
    free(inputLines.lines);

    return 0;
}


bool areArgumentsSufficient(int argc, char *argv[]) {
    const char* programName = argv[0];
    const char* inputFilePath = argv[1];
    const char* outputFilePath = argv[2];

    if (argc != 3) {
        if (argc > 3) {
            printf("Too many input arguments.\n");
        }
        else if (argc < 3) {
            printf("Too little input arguments.\n");
        }

        printf("A proper call looks like: '%s <inputFilePath> <outputFilePath>'\n"
               "Operation aborted.\n", programName);

        return false;
    }

    if(!doesFileExists(inputFilePath)) {
        printf("Input file does not exist. Operation aborted.\n");
        return false;
    }

    if(doesFileExists(outputFilePath)) {
        printf("Output file already exists. Operation aborted.\n");
        return false;
    }

    return true;
}


