#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include "errorReturn.h"
#include "protocol.h"

/*
 *The airplane ID.
 */
char* id = NULL;

/*
 *The port used to connect to the mapper.
 */
int mapperPort = 0;

/*
 *Keep the connection open for planes.
 */
int keepListening = 1;

/*
 *I/O stream to the currently connected airport.
 */
FILE* streamToControl;

/*
 *The number of airports to visit.
 */
int controlCount = 0;

/*
 *The flight route.
 */
int* destinationControls = NULL;

/*
 *The number of used entries in the destinations-log.
 */
int loggedDestinations = 0;

/*
 *The buffer holding all the logged destination infos.
 */
char** destinationInfoLogs = NULL;

/*
 *Validate the command line arguments.
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
    success = roc_check_chars(argv[2]);
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

/*
 *Open a set of streams representing bidirectional communication to a player.
 */
int open_stream(int fileToPlaneNo) {
    streamToControl = fdopen(fileToPlaneNo, "r+");

    if (!streamToControl) {
        return E_CONTROL_FAILED_TO_CONNECT;
    }

    return E_CONTROL_OK;
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
        destinationControls[controlCount] = roc_resolve_control(mapperPort, argv[i - 3]);
        if (destinationControls[controlCount]) {
            controlCount += 1;
        }
    }

    destinationInfoLogs = roc_alloc_log(ROC_MAX_DESTINATION_COUNT, ROC_MAX_INFO_SIZE);
    loggedDestinations = 0;

    /*visit_all_targets();*/

    free(destinationInfoLogs);
    return EXIT_SUCCESS;
}

