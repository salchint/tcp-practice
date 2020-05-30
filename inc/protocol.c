/*
 *protocol.c
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "errorReturn.h"
#include "protocol.h"

/*
 *Allocate a map as a contignuous chunk.
 */
char** alloc_log(int rows, int columns) {
    int bodySize = 0;
    int headerSize = 0;
    char** row = NULL;
    char* buf = NULL;
    int i = 0;

    headerSize = rows * sizeof(char*);
    bodySize = rows * columns * sizeof(char);

    row = (char**)malloc(headerSize + bodySize);
    memset(row, 0, headerSize + bodySize);

    buf = (char*)(row + rows);
    row[0] = buf;
    for(i = 1; i < rows; i++) {
        row[i] = row[i - 1] + columns;
    }

    return row;
}

/*
 *Allocate an array of airport info strings.
 */
char** roc_alloc_log(int rows, int columns) {
    return alloc_log(rows, columns);
}

/*
 *Allocate an array of airplane IDs.
 */
char** control_alloc_log(int rows, int columns) {
    return alloc_log(rows, columns);
}

/*
 *Create a server-like socket and wait for incoming connections.
 *Returns the socket's file descriptor.
 */
int control_open_incoming_conn(int* port) {
    int acceptSocket = 0;
    int enable = 1;
    struct sockaddr_in acceptAddress;
    socklen_t addressSize = sizeof(struct sockaddr_in);

    memset(&acceptAddress, 0, sizeof(struct sockaddr_in));

    acceptSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > acceptSocket) {
        error_return_control(E_CONTROL_FAILED_TO_CONNECT);
    }
    setsockopt(acceptSocket, SOL_SOCKET, SO_REUSEADDR, (void*)&enable,
            sizeof(enable));

    acceptAddress.sin_family = AF_INET;
    acceptAddress.sin_addr.s_addr = INADDR_ANY;
    if (0 != bind(acceptSocket, (struct sockaddr*)(&acceptAddress),
                addressSize)) {
        error_return_control(E_CONTROL_FAILED_TO_CONNECT);
    }
    if (0 != getsockname(acceptSocket, (struct sockaddr*)(&acceptAddress),
                &addressSize)) {
        error_return_control(E_CONTROL_FAILED_TO_CONNECT);
    }
    *port = ntohs(acceptAddress.sin_port);

    return acceptSocket;
}

/*
 *Close the given socket.
 */
void control_close_conn(int closeSocket) {
    close(closeSocket);
}

/*
 *Search for invalid characters in the arguments.
 */
int check_chars(const char* arg, int isControl) {
    int i = 0;

    if (isControl) {
        if (CONTROL_MAX_ID_SIZE <= strlen(arg)) {
            return E_CONTROL_INVALID_INFO;
        }
    } else {
        if (ROC_MAX_INFO_SIZE <= strlen(arg)) {
            return E_ROC_INVALID_ARGS_COUNT;
        }
    }

    for (i = 0; i < strlen(arg); i++) {
        if (arg[i] == '\n' || arg[i] == '\r' || arg[i] == ':') {
            if (isControl) {
                return E_CONTROL_INVALID_INFO;
            }
            return E_ROC_INVALID_ARGS_COUNT;
        }
    }

    if (isControl) {
        return E_CONTROL_OK;
    }
    return E_ROC_OK;
}

/*
 *Search for invalid characters in the arguments.
 *Returns an error code according to enum ControlErrorCodes.
 */
int control_check_chars(const char* arg) {
    return check_chars(arg, 1);
}

/*
 *Search for invalid characters in the arguments.
 *Returns an error code according to enum RocErrorCodes.
 */
int roc_check_chars(const char* arg) {
    return check_chars(arg, 0);
}

/*
 *Validate the given port number
 */
int check_port(const char* arg, int isControl, int isDestination) {
    long int port = 0;
    char* end = NULL;
    int success = E_CONTROL_OK;

    if (!isControl) {
        if (!isDestination) {
            if ('-' == arg[0] && '\0' == arg[1]) {
                return E_ROC_OK;
            }
        }

        if (ROC_MAX_INFO_SIZE <= strlen(arg)) {
            return E_ROC_INVALID_MAPPER_PORT;
        }
    }

    port = strtol(arg, &end, 10);

    if ('\0' == *end) {
        if (LONG_MIN == port || LONG_MAX == port || 65536 <= port || 0 >= port) {
            if (isControl) {
                return E_CONTROL_INVALID_PORT;
            }
            return E_ROC_INVALID_MAPPER_PORT;
        }
    } else {
        if (isControl) {
            success = control_check_chars(end);
            if (E_CONTROL_OK != success) {
                return success;
            } else {
                return E_CONTROL_INVALID_PORT;
            }
        } else {
            success = roc_check_chars(end);
            if (E_ROC_OK != success) {
                return success;
            } else {
                return E_ROC_INVALID_MAPPER_PORT;
            }
        }
    }

    if (isControl) {
        return E_CONTROL_OK;
    }
    return E_ROC_OK;
}

/*
 *Validate the given port number
 *Returns an error code according to enum ControlErrorCodes.
 */
int control_check_port(const char* arg) {
    return check_port(arg, 1, 0);
}

/*
 *Validate the given port number
 *Returns an error code according to enum RocErrorCodes.
 */
int roc_check_port(const char* arg) {
    return check_port(arg, 0, 0);
}

/*
 *Validate the given port number
 *Returns an error code according to enum RocErrorCodes.
 */
int roc_check_destination_port(const char* arg) {
    return check_port(arg, 0, 1);
}

/*
 *Comparer function used while ordering the flight information.
 */
static int string_comparator(const void* arg0, const void* arg1) {
    return strcmp(*(char* const*)arg0, *(char* const*)arg1);
}

/*
 *Sort the collected airplane identifiers in lexicographic order.
 */
void control_sort_plane_log(char** planesLog, int loggedPlanes) {
    qsort(planesLog, loggedPlanes, sizeof(char*), string_comparator);
}

/*
 *Look up the given destination airport if needed.
 *Returns the port number of the given airport on success, 0 else.
 */
int roc_resolve_control(int mapperPort, const char* destination) {
    /*
     *TODO implement
     */

    return 0;
}

