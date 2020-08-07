/*
 *errorReturn.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "errorReturn.h"

/**
 * Error messages of control sent to stderr.
 */
const char* controlErrorTexts[] = {
        "",
        "Usage: control2310 id info [mapper]",
        "Invalid char in parameter",
        "Invalid port",
        "Can not connect to map"
        };

/**
 * Error messages of roc sent to stderr.
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

void error_return_control(enum ControlErrorCodes code) {
    fprintf(stderr, "%s\n", controlErrorTexts[code]);
    fflush(stderr);
    exit(code);
}

void error_return_roc(enum RocErrorCodes code) {
    fprintf(stderr, "%s\n", rocErrorTexts[code]);
    fflush(stderr);
    exit(code);
}

