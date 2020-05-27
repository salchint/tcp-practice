/*
 *protocol.c
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdint.h>
#include <limits.h>

#include "errorReturn.h"
#include "protocol.h"

/*
 *Search for invalid characters in the arguments.
 */
int control_check_chars(const char* arg) {
    int i = 0;

    for (i = 0; i < strlen(arg); i++) {
        if (arg[i] == '\n' || arg[i] == '\r' || arg[i] == ':') {
            return E_CONTROL_INVALID_INFO;
        }
    }

    return E_CONTROL_OK;
}

/*
 *Validate the given port number
 *Returns an error code according to enum ControlErrorCodes.
 */
int control_check_port(const char* arg) {
    long int port = 0;
    char* end = NULL;
    int success = E_CONTROL_OK;

    port = strtol(arg, &end, 10);

    if ('\0' == *end) {
        if (LONG_MIN == port || LONG_MAX == port || 65535 <= port || 0 >= port) {
            return E_CONTROL_INVALID_PORT;
        }
    } else {
        success = control_check_chars(end);
        if (E_CONTROL_OK != success) {
            return success;
        } else {
            return E_CONTROL_INVALID_PORT2;
        }
    }

    return E_CONTROL_OK;
}

