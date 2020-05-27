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
            return E_CONTROL_INVALID_PORT;
        }
    }

    return E_CONTROL_OK;
}

