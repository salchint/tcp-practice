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

/**
 * Allocate a 2-dimensional array of chars as a contignuous chunk.
 *
 * The allocated buffer contains a header containing pointers to all rows.
 * All rows have the same length. This returns a pointer to the start of rows,
 * which in turn are pointers to columns each. You can free this "array of
 * arrays" via one single free() invocation passing on the returned
 * pointers-pointer.
 *
 * @param rows The number of rows (1st demension).
 *
 * @param columns The number of columns in the rows (2nd dimension).
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
    for (i = 1; i < rows; i++) {
        row[i] = row[i - 1] + columns;
    }

    return row;
}

char** mapper_alloc_map(int rows, int columns) {
    return alloc_log(rows, columns);
}

char** roc_alloc_log(int rows, int columns) {
    return alloc_log(rows, columns);
}

char** control_alloc_log(int rows, int columns) {
    return alloc_log(rows, columns);
}

/**
 * Create a server-like socket and wait for incoming connections.
 *
 * Returns the socket's file descriptor, which is created as a TCP server-side
 * end-point and bound to the localhost.  In case of error, this function does
 * not return. Instead the program exits and a specific error code is issued.
 *
 * @param port  Output parameter, which holds the port number the new socket is
 *              bound to.
 */
int open_incoming_conn(int* port) {
    int acceptSocket = 0;
    int enable = 1;
    struct sockaddr_in acceptAddress;
    socklen_t addressSize = sizeof(struct sockaddr_in);

    memset(&acceptAddress, 0, addressSize);

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

int control_open_incoming_conn(int* port) {
    return open_incoming_conn(port);
}

int mapper_open_incoming_conn(int* port) {
    return open_incoming_conn(port);
}

/**
 * Open a connection to the given server.
 *
 * Returns a TCP client socket, which is created and connected to the server at
 * the given port. In case of an error, -1 is returned.
 *
 * @param port  The port number at which the TCP server is listening.
 */
int open_client_conn(int port) {
    int clientSocket = 0;
    struct sockaddr_in clientAddress;
    socklen_t addressSize = sizeof(struct sockaddr_in);

    memset(&clientAddress, 0, addressSize);

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > clientSocket) {
        return -1;
    }

    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr = INADDR_ANY;
    clientAddress.sin_port = htons(port);

    if (0 > connect(clientSocket, (struct sockaddr*)&clientAddress,
            addressSize)) {
        return -1;
    }

    return clientSocket;
}

int roc_open_destination_conn(int port) {
    return open_client_conn(port);
}

int control_open_mapper_conn(int port) {
    return open_client_conn(port);
}

void control_close_conn(int closeSocket) {
    close(closeSocket);
}

void roc_close_conn(int closeSocket) {
    close(closeSocket);
}

void mapper_close_conn(int closeSocket) {
    close(closeSocket);
}

/**
 * Search for invalid characters in the arguments.
 *
 * Returns the caller specific error code if the given arg exceeds the maximum
 * allowed length or contains either of LF, CR or ':', which are regarded
 * invalid.
 *
 * @param arg The argument, which is to be verified.
 *
 * @param isControl Specifies if the caller is the control program. The
 *                  specific return codes depend on this.
 *
 * @param end If not NULL, this defines the end of arg, that is to be verified.
 *            The character end points to and later ones are excluded from
 *            verification.
 */
int check_chars(const char* arg, int isControl, const char* end) {
    int i = 0;
    char buffer[CONTROL_MAX_ID_SIZE + 1];
    size_t distance = 0;

    if (end) {
        strncpy(buffer, arg, CONTROL_MAX_ID_SIZE);
        buffer[CONTROL_MAX_ID_SIZE] = '\0';
        distance = end - arg;
        if (CONTROL_MAX_ID_SIZE < distance) {
            return E_CONTROL_INVALID_INFO;
        }
        buffer[distance] = '\0';
        arg = buffer;
    }

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

int control_check_chars(const char* arg) {
    return check_chars(arg, 1, NULL);
}

int roc_check_chars(const char* arg) {
    return check_chars(arg, 0, NULL);
}

int mapper_check_chars(const char* arg, const char* end) {
    return (check_chars(arg, 0, end) == E_ROC_OK) ? EXIT_SUCCESS :
            EXIT_FAILURE;
}

/**
 * Validate the given port number.
 *
 * Returns a caller specific error code if the given port number exceeds the
 * maximum allowed length or contains either of LF, CR or ':', which are
 * regarded invalid or if it is a nuber, but out of the valid range of port
 * numbers.
 *
 * @param arg The port number, which is to be verified.
 *
 * @param isControl Defines if the caller is the control program.
 *
 * @param isDestination Defines if the given port number is a airport's port
 *                      number.
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
        if (LONG_MIN == port || LONG_MAX == port || 65536 <= port
                || 0 >= port) {

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

int control_check_port(const char* arg) {
    return check_port(arg, 1, 0);
}

int roc_check_port(const char* arg) {
    return check_port(arg, 0, 0);
}

int roc_check_destination_port(const char* arg) {
    return check_port(arg, 0, 1);
}

/**
 * Comparer function used while ordering the flight information or airport IDs.
 *
 * Returns -1, 0 or 1 if arg0 is lower, equal or greater than arg1.
 *
 * @param arg0  The one argument to compare.
 *
 * @param arg1  The other argument to compare to.
 */
static int string_comparator(const void* arg0, const void* arg1) {
    return strcmp(*(char* const*)arg0, *(char* const*)arg1);
}

void control_sort_plane_log(char** planesLog, int loggedPlanes) {
    qsort(planesLog, loggedPlanes, sizeof(char*), string_comparator);
}

void mapper_sort_control_map(char** controlMap, int mappedControls) {
    qsort(controlMap, mappedControls, sizeof(char*), string_comparator);
}

/**
 * Query the airport's port number from the mapper.
 *
 * Returns E_ROC_FAILED_TO_CONNECT_MAPPER or E_ROC_FAILED_TO_FIND_ENTRY if it
 * cannot connect to the mapper using the given mapperPort respectively if
 * mapper cannot find the given airport ID. E_ROC_OK is returned on success.
 *
 * @param mapperPort  The port number at which the mapper is listening.
 *
 * @param destination The airport ID to look for.
 *
 * @param controlPort Output parameter, which is set to the registered
 *                    control's port number on success.
*/
int roc_find_destination_port(int mapperPort, const char* destination, long
        int* controlPort) {
    int mapperSocket = 0;
    FILE* streamToMapper = NULL;
    char buffer[ROC_MAX_INFO_SIZE + 1];
    char* end = NULL;

    mapperSocket = control_open_mapper_conn(mapperPort);
    if (0 > mapperSocket) {
        return E_ROC_FAILED_TO_CONNECT_MAPPER;
    }

    if (EXIT_SUCCESS != open_socket_stream(mapperSocket, &streamToMapper)) {
        roc_close_conn(mapperSocket);
        return E_ROC_FAILED_TO_CONNECT_MAPPER;
    }

    fprintf(streamToMapper, "?%s\n", destination);
    fflush(streamToMapper);

    if (!fgets(buffer, sizeof(buffer), streamToMapper)) {
        fclose(streamToMapper);
        control_close_conn(mapperSocket);
        return E_ROC_FAILED_TO_CONNECT_MAPPER;
    }

    *controlPort = (int)strtol(buffer, &end, 10);

    if ('\n' != *end || *controlPort <= 0 || 65535 < *controlPort) {
        fclose(streamToMapper);
        control_close_conn(mapperSocket);
        return E_ROC_FAILED_TO_FIND_ENTRY;
    }

    fclose(streamToMapper);
    control_close_conn(mapperSocket);

    return E_ROC_OK;
}

int roc_resolve_control(int mapperPort, const char* destination) {
    int success = E_ROC_OK;
    char* end = NULL;
    long controlPort = strtol(destination, &end, 10);

    if (LONG_MIN == controlPort || LONG_MAX == controlPort) {
        return 0;
    }

    if ('\0' != *end) {
        success = roc_find_destination_port(mapperPort, destination,
                &controlPort);

        if (E_ROC_OK != success) {
            error_return_roc((enum RocErrorCodes)success);
        }
    } else {
        if (controlPort <= 0 || 65535 < controlPort) {
            return 0;
        }
    }

    return (int)controlPort;
}

/**
 * Remove the trailing LF from the given string if present.
 *
 * @param text  The string to be trimmed.
 */
void trim_string_end(char* text) {
    char* found = NULL;

    found = strrchr(text, '\n');
    if (found) {
        *found = '\0';
    }
}

void roc_trim_string_end(char* text) {
    trim_string_end(text);
}

void mapper_trim_string_end(char* text) {
    trim_string_end(text);
}

int open_socket_stream(int socketNumber, FILE** stream) {
    *stream = fdopen(socketNumber, "r+");

    if (!*stream) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int control_register_id(int mapperPort, int acceptPort, const char* id) {
    int success = E_CONTROL_OK;
    int mapperSocket = 0;
    FILE* streamToMapper = NULL;

    mapperSocket = control_open_mapper_conn(mapperPort);
    if (0 > mapperSocket) {
        return E_CONTROL_FAILED_TO_CONNECT;
    }

    success = open_socket_stream(mapperSocket, &streamToMapper);
    if (EXIT_SUCCESS != success) {
        mapper_close_conn(mapperSocket);
        return E_CONTROL_FAILED_TO_CONNECT;
    }

    fprintf(streamToMapper, "!%s:%d\n", id, acceptPort);
    fclose(streamToMapper);
    control_close_conn(mapperSocket);

    return E_CONTROL_OK;
}
