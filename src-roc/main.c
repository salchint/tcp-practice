#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../inc/errorReturn.h"
#include "../inc/protocol.h"

/**
 * The airplane ID.
 */
char* id = NULL;

/**
 * The port used to connect to the mapper.
 */
int mapperPort = 0;

/**
 * Keep the connection open for planes.
 */
int keepListening = 1;

/**
 * I/O stream to the currently connected airport.
 */
FILE* streamToControl;

/**
 * The number of airports to visit.
 */
int destinationCount = 0;

/**
 * The flight route.
 */
int* destinationControls = NULL;

/**
 * The number of used entries in the destinations-log.
 */
int loggedDestinations = 0;

/**
 * The buffer holding all the logged destination infos.
 */
char** destinationInfoLogs = NULL;

/**
 * Validate the command line arguments.
 *
 * Returns E_ROC_OK on success, but does not return if the number of
 * command line arguments is wrong or if an argument contains invalid
 * characters or if a invalid port number is given. Instead the program exits
 * and returns a specific error code.
 *
 * @param argc  The number of command line arguments.
 *
 * @param argv  The string array containing all the given arguments.
 */
void check_args(int argc, char* argv[]) {
    int i = 0;
    int success = 0;
    int mapperNeeded = 0;

    if (argc < 3) {
        error_return_roc(E_ROC_INVALID_ARGS_COUNT);
    }

    /*airplane Id*/
    success = roc_check_chars(argv[1]);
    if (E_ROC_OK != success) {
        error_return_roc(success);
    }

    /*mapper port*/
    success = roc_check_port(argv[2]);
    if (E_ROC_OK != success) {
        error_return_roc(success);
    }

    /*zero or more destinations (controls)*/
    for (i = 3; i < argc; i++) {
        success = roc_check_destination_port(argv[i]);
        if (E_ROC_INVALID_MAPPER_PORT == success) {
            mapperNeeded = 1;
        } else {
            if (E_ROC_OK != success) {
                error_return_roc(success);
            }
        }
    }

    /*Check if a mapper is needed and available*/
    if (mapperNeeded && '-' == argv[2][0] && '\0' == argv[2][1]) {
        error_return_roc(E_ROC_MAPPER_REQUIRED);
    }
}

/**
 * Open a set of streams representing bidirectional communication to a client.
 *
 * Returns E_ROC_OK if the file stream could be opend,
 * E_ROC_FAILED_TO_CONNECT_CONTROL else.
 *
 * @param fileToControlNo The socket, which should be used for client
 *                        communication.
 */
int open_stream(int fileToControlNo) {
    int success = open_socket_stream(fileToControlNo, &streamToControl);
    return (EXIT_SUCCESS == success) ? E_ROC_OK :
            E_ROC_FAILED_TO_CONNECT_CONTROL;
}

/**
 * Print the collected airport infos to stdout.
 *
 * Dump all log entries of visited airports to stdout.
 */
void print_info_logs() {
    int i = 0;

    for (i = 0; i < loggedDestinations; i++) {
        fprintf(stdout, "%s\n", destinationInfoLogs[i]);
    }
    fflush(stdout);
}

/**
 * Send my airplane's id and get the airport's info back.
 *
 * Exchange data with the visited airport via the socket connection.
 *
 * @param currentInfo The next free log entry.
 */
int get_airport_info(char* currentInfo) {
    fprintf(streamToControl, "%s\n", id);
    fflush(streamToControl);

    if (!fgets(currentInfo, ROC_MAX_INFO_SIZE, streamToControl)) {
        return E_ROC_FAILED_TO_CONNECT_CONTROL;
    }
    roc_trim_string_end(currentInfo);

    return E_ROC_OK;
}

/**
 * Get the airport info from all the destinations.
 *
 * Visit all the destinations and exchange data with the respective controls
 * via socket connections. In case one of the destinations cannot be contacted,
 * continue with the next one. In this case E_ROC_FAILED_TO_CONNECT_CONTROL is
 * returned upon exiting the program.
 */
void visit_all_targets() {
    int i = 0;
    int destinationSocket = 0;
    int success = 1;
    char* currentInfo = NULL;

    for (i = 0; i < destinationCount; i++) {
        destinationSocket = roc_open_destination_conn(destinationControls[i]);
        if (0 > destinationSocket) {
            success = 0;
            continue;
        }

        if (E_ROC_OK != open_stream(destinationSocket)) {
            success = 0;
            roc_close_conn(destinationSocket);
            continue;
        }

        currentInfo = destinationInfoLogs[loggedDestinations];
        if (E_ROC_OK != get_airport_info(currentInfo)) {
            success = 0;
            fclose(streamToControl);
            roc_close_conn(destinationSocket);
            continue;
        }

        if (E_ROC_OK != roc_check_chars(currentInfo)) {
            success = 0;
            fclose(streamToControl);
            roc_close_conn(destinationSocket);
            continue;
        }

        loggedDestinations += 1;
        fclose(streamToControl);
        roc_close_conn(destinationSocket);
    }

    print_info_logs();

    if (!success) {
        error_return_roc(E_ROC_FAILED_TO_CONNECT_CONTROL);
    }
}

int main(int argc, char* argv[]) {
    int i = 0;

    check_args(argc, argv);

    id = argv[1];

    if ('-' != argv[2][0]) {
        mapperPort = (int)strtol(argv[2], NULL, 10);
    }

    destinationControls = (int*)malloc((argc - 3) * sizeof(int));
    for (i = 3; i < argc; i++) {
        destinationControls[destinationCount] = roc_resolve_control(mapperPort,
                argv[i]);
        if (destinationControls[destinationCount]) {
            destinationCount += 1;
        }
    }

    destinationInfoLogs = roc_alloc_log(ROC_MAX_DESTINATION_COUNT,
            ROC_MAX_INFO_SIZE);
    loggedDestinations = 0;

    visit_all_targets();

    free(destinationInfoLogs);
    return EXIT_SUCCESS;
}

