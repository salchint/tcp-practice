/*
 *errorReturn.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "errorReturn.h"

/*
 *Error messages sent to stdout.
 */
const char* controlErrorTexts[] = {
    "",
    "Usage: control2310 id info [mapper]",
    "Invalid char in parameter",
    "Invalid port",
    "Can not connect to map"
};

/*
 *Error messages sent to stdout.
 */
const char* rocErrorTexts[] = {
    "",
    "Usage: roc2310 id mapper {airports}",
    "Invalid mapper port",
    "Mapper required",
    "Failed to connect to mapper",
    "No map entry for destination",
    "Failed to connect to at least one destination"
};

/*
 *Print an error message to stdout and exit the program.
 */
void error_return_control(enum ControlErrorCodes code) {
    fprintf(stdout, "%s\n", controlErrorTexts[code]);
    exit(code);
}

/*
 *Print an error message to stdout and exit the program.
 */
void error_return_roc(enum RocErrorCodes code) {
    fprintf(stdout, "%s\n", rocErrorTexts[code]);
    exit(code);
}

