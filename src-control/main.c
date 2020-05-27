#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "errorReturn.h"
#include "protocol.h"

/*
 *The airport ID.
 */
char* id = NULL;

/*
 *The airport info text.
 */
char* info = NULL;

/*
 *The port used to connect to the mapper.
 */
int mapperPort = 0;

/*
 *Upon receiving some message, execute it as long as it is valid.
 */
/*int process_command(const char* command) {*/
    /*
     *if (0 == strncmp("EARLY", command, 5u)) {
     *    error_return(stderr, E_EARLY_GAME_OVER);
     *} else if (0 == strncmp("DONE", command, 4u)) {
     *    return 0;
     *} else if (0 == strncmp("YT", command, 2u)) {
     *    if (!('\0' == command[2]
     *            || '\n' == command[2]
     *            || EOF == command[2])) {
     *        error_return(stderr, E_COMMS_ERROR);
     *    }
     *    make_move(playersCount);
     *} else if (0 == strncmp("HAP", command, 3u)) {
     *    player_process_move_broadcast(command, playerPositions, playerRankings,
     *            playersCount, ownId, thisPlayer, &players, &path);
     *} else {
     *    error_return(stderr, E_COMMS_ERROR);
     *}
     *return 1;
     */
/*}*/

/*
 *Validate the command line arguments.
 */
int check_args(int argc, char* argv[]) {
    int success = E_CONTROL_OK;

    if (argc < 3 || 4 < argc) {
        error_return_control(E_CONTROL_INVALID_ARGS_COUNT);
    }

    success = control_check_chars(argv[1]);
    if (E_CONTROL_OK != success) {
        return success;
    }
    success = control_check_chars(argv[2]);
    if (E_CONTROL_OK != success) {
        return success;
    }

    if (4 == argc) {
        success = control_check_chars(argv[3]);
        if (E_CONTROL_OK != success) {
            return success;
        }
        success = control_check_port(argv[3]);
        if (E_CONTROL_OK != success) {
            return success;
        }
    }

    return success;
}

/*
 *Listen on an ephemeral port for planes.
 */
void listen_for_planes() {
    int port = 0;
    int acceptSocket = 0;

    acceptSocket = control_open_incoming_conn(&port);
    printf("%d\n", port);

    /*listen(acceptSocket, CONTROL_MAX_CONNECTIONS);*/
}

int main(int argc, char* argv[]) {
    /*int i = 0;*/

    check_args(argc, argv);

    id = argv[1];
    info = argv[2];

    if (4 == argc) {
        mapperPort = (int)strtol(argv[3], NULL, 10);
    }

    listen_for_planes();

    return EXIT_SUCCESS;
}

