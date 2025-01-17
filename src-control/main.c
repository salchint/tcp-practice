#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

#include "../inc/errorReturn.h"
#include "../inc/protocol.h"

/**
 * The airport ID.
 */
char* id = NULL;

/**
 * The airport info text.
 */
char* info = NULL;

/**
 * The port used to connect to the mapper.
 */
int mapperPort = 0;

/**
 * Keep the connection open for planes.
 */
int keepListening = 1;

/**
 * I/O stream to the connected plane.
 */
FILE* streamToPlane;

/**
 * The number of used entries in the planes log.
 */
int loggedPlanes = 0;

/**
 * The buffer holding all the logged planes.
 */
char** planesLog = NULL;

/**
 * Mutex protecting the read/write operations on the global state.
 */
static pthread_mutex_t planesLogGuard = PTHREAD_MUTEX_INITIALIZER;

/**
 * Mutex protecting the client socket variable set upon accept.
 */
static pthread_mutex_t clientSocketGuard = PTHREAD_MUTEX_INITIALIZER;

/**
 * Validate the command line arguments.
 *
 * Returns E_CONTROL_OK on success, but does not return if the number of
 * command line arguments is wrong or if an argument contains invalid
 * characters or if a invalid port number is given. Instead the program exits
 * and returns a specific error code.
 *
 * @param argc  The number of command line arguments.
 *
 * @param argv  The string array containing all the given arguments.
 */
int check_args(int argc, char* argv[]) {
    int success = E_CONTROL_OK;

    if (argc < 3 || 4 < argc) {
        error_return_control(E_CONTROL_INVALID_ARGS_COUNT);
    }

    success = control_check_chars(argv[1]);
    if (E_CONTROL_OK != success) {
        error_return_control(success);
    }
    success = control_check_chars(argv[2]);
    if (E_CONTROL_OK != success) {
        error_return_control(success);
    }

    if (4 == argc) {
        success = control_check_chars(argv[3]);
        if (E_CONTROL_OK != success) {
            error_return_control(success);
        }
        success = control_check_port(argv[3]);
        if (E_CONTROL_OK != success) {
            error_return_control(success);
        }
    }

    return success;
}

/**
 * Open a stream representing bidirectional communication to a client.
 *
 * Returns E_CONTROL_OK if the file stream could be opend,
 * E_CONTROL_FAILED_TO_CONNECT else.
 *
 * @param fileToPlaneNo The socket, which sould be used for client
 *                      communication.
 */
int open_stream(int fileToPlaneNo) {
    int success = open_socket_stream(fileToPlaneNo, &streamToPlane);
    return (EXIT_SUCCESS == success) ? E_CONTROL_OK :
            E_CONTROL_FAILED_TO_CONNECT;
}

/**
 * Receive the visiting plane's ID.
 *
 * Receive incoming client transmissions in order to log visiting airplanes.
 * Alternatively a request "log" can be handled. In this case all log entries
 * are replied to the caller.
 *
 * @param fileToPlaneNo The socket, which sould be used for client
 *                      communication.
 */
void log_plane(int fileToPlaneNo) {
    int i = 0;

    if (E_CONTROL_OK != open_stream(fileToPlaneNo)) {
        return;
    }

    pthread_mutex_lock(&planesLogGuard);

    if (!fgets(planesLog[loggedPlanes], CONTROL_MAX_ID_SIZE, streamToPlane)) {
        return;
    }
    if (0 == strncmp("log", planesLog[loggedPlanes], 3)) {
        control_sort_plane_log(planesLog, loggedPlanes);

        for (i = 0; i < loggedPlanes; i++) {
            fprintf(streamToPlane, "%s", planesLog[i]);
        }
        fputs(".\n", streamToPlane);
    } else {
        loggedPlanes += 1;
        fprintf(streamToPlane, "%s\n", info);
    }

    pthread_mutex_unlock(&planesLogGuard);
    fclose(streamToPlane);
}

/**
 * The new launched thread's starting point.
 *
 * This is the thread function, which runs individual client connections.
 */
void* thread_main(void* parameter) {
    int planeSocket = *(int*)parameter;
    pthread_mutex_unlock(&clientSocketGuard);

    log_plane(planeSocket);

    control_close_conn(planeSocket);

    return NULL;
}

/**
 * Listen on an ephemeral port for planes.
 *
 * The program exits and returns a specific error code if no new thread could
 * be created for an incoming client connection, the incoming connection cannot
 * be accepted or this airport fails to register with the mapper.
 *
 * @param port  Output parameter, the ephemeral port, which this control is
 *              listening on.
 */
void listen_for_planes(int* port) {
    int success = E_CONTROL_OK;
    int acceptSocket = 0;
    int planeSocket = 0;
    pthread_t planeThread;
    pthread_attr_t planeThreadOptions;

    *port = 0;

    acceptSocket = control_open_incoming_conn(port);

    if (mapperPort) {
        success = control_register_id(mapperPort, *port, id);
        if (E_CONTROL_OK != success) {
            error_return_control(success);
        }
    }

    fprintf(stdout, "%d\n", *port);
    fflush(stdout);

    listen(acceptSocket, CONTROL_MAX_CONNECTIONS);

    pthread_attr_init(&planeThreadOptions);
    pthread_attr_setdetachstate(&planeThreadOptions,
            PTHREAD_CREATE_DETACHED);

    while (keepListening) {
        pthread_mutex_lock(&clientSocketGuard);
        planeSocket = accept(acceptSocket, NULL, NULL);
        if (0 > planeSocket) {
            pthread_mutex_unlock(&clientSocketGuard);
            error_return_control(E_CONTROL_FAILED_TO_CONNECT);
        }

        if (0 != pthread_create(&planeThread, &planeThreadOptions, thread_main,
                &planeSocket)) {
            pthread_mutex_unlock(&clientSocketGuard);
            error_return_control(E_CONTROL_FAILED_TO_CONNECT);
        }
    }

    control_close_conn(acceptSocket);
    pthread_attr_destroy(&planeThreadOptions);
}

int main(int argc, char* argv[]) {
    int port = 0;

    check_args(argc, argv);

    id = argv[1];
    info = argv[2];

    if (4 == argc) {
        mapperPort = (int)strtol(argv[3], NULL, 10);
    }

    planesLog = control_alloc_log(CONTROL_MAX_PLANE_COUNT, CONTROL_MAX_ID_SIZE);
    loggedPlanes = 0;

    listen_for_planes(&port);

    free(planesLog);
    return EXIT_SUCCESS;
}

