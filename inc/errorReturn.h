/*
 *errorReturn.h
 */

#pragma once

#ifndef __ERROR_RETURN_H__
#define __ERROR_RETURN_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 *Error codes of control used upon exiting the program.
 */
enum ControlErrorCodes {
    E_CONTROL_OK = 0,
    E_CONTROL_INVALID_ARGS_COUNT = 1,
    E_CONTROL_INVALID_INFO = 2,
    E_CONTROL_INVALID_PORT = 3,
    E_CONTROL_INVALID_PORT2 = 4
};

/*
 *Error codes of roc used upon exiting the program.
 */
enum RocErrorCodes {
    E_ROC_OK = 0,
    E_ROC_INVALID_ARGS_COUNT = 1,
    E_ROC_INVALID_MAPPER_PORT = 2,
    E_ROC_MAPPER_REQUIRED = 3,
    E_ROC_FAILED_TO_CONNECT_MAPPER = 4,
    E_ROC_FAILED_TO_FIND_ENTRY = 5,
    E_ROC_FAILED_TO_CONNECT_CONTROL = 6
};

/*
 *Error messages sent to stdout.
 */
extern const char* controlErrorTexts[];

/*
 *Error messages sent to stdout.
 */
extern const char* rocErrorTexts[];

/*
 *Print an error message to stdout and exit the program.
 */
void error_return_control(enum ControlErrorCodes code);

/*
 *Print an error message to stdout and exit the program.
 */
void error_return_roc(enum RocErrorCodes code);

#endif

