#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

#include "../inc/errorReturn.h"
#include "../inc/protocol.h"

/*
 *I/O stream to the connected plane or airport.
 */
/*FILE* streamToClient;*/

/*
 *The number of used entries in the airport map.
 */
int mappedControls = 0;

/*
 *The buffer holding all the mapped airports.
 */
char** controlMap = NULL;

/*
 *Mutex protecting the read/write operations on the global state.
 */
static pthread_mutex_t controlMapGuard = PTHREAD_MUTEX_INITIALIZER;

/*
 *Mutex protecting the client socket variable set upon accept.
 */
static pthread_mutex_t clientSocketGuard = PTHREAD_MUTEX_INITIALIZER;

/*
 *Open a stream representing bidirectional communication to a client.
 */
int open_stream(int fileToClientNo, FILE** streamToClient) {
    *streamToClient = fdopen(fileToClientNo, "r+");

    if (!*streamToClient) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/*
 *Search for the given airport ID in the control map.
 *Returns the index of the map entry if found, -1 else.
 */
int find_entry(const char* id) {
    int i = 0;
    size_t distance = 0;

    distance = strrchr(id, ':') - id;

    if (MAPPER_MAX_ID_SIZE < distance) {
        distance = strrchr(id, '\n') - id;
    }

    if (MAPPER_MAX_ID_SIZE < distance) {
        distance = strlen(id);
    }

    for (i = 0; i < mappedControls; i++) {
        if (0 == strncmp(id, controlMap[i], distance)) {
            if (strlen(controlMap[i]) == distance) {
                return i;
            }
        }
    }

    return -1;
}

/*
 *Add a new entry to the control map.
 */
void add_entry(char* id) {
    char* end;
    int port = 0;
    const char* seperator = strrchr(id, ':');
    size_t distance = seperator - id;
    char* currentEntry = controlMap[mappedControls];
    int* currentEntryValue = (int*)(currentEntry + MAPPER_MAX_ID_SIZE);

    if (!seperator) {
        return;
    }

    if (0 <= find_entry(id)) {
        return;
    }

    mapper_trim_string_end(id);

    if ((distance + 1) >= strlen(id)) {
        return;
    }

    if (EXIT_SUCCESS != mapper_check_chars(id, seperator)) {
        return;
    }

    port = strtol(seperator + 1, &end, 10);
    if ('\0' != *end) {
        return;
    }

    strncpy(currentEntry, id, MIN(distance, MAPPER_MAX_ID_SIZE));
    currentEntry[MAPPER_MAX_ID_SIZE - 1] = '\0';
    *currentEntryValue = port;

    if (mappedControls < MAPPER_MAX_CONTROL_COUNT) {
        mappedControls += 1;
    }
}

/*
 *Reply the port number of the given control.
 */
void reply_entry(const char* id, FILE* streamToClient) {
    int found = -1;
    int* currentEntryValue = NULL;

    found = find_entry(id);
    if (0 <= found) {
        currentEntryValue = (int*)(controlMap[found] + MAPPER_MAX_ID_SIZE);
    }

    if (currentEntryValue) {
        fprintf(streamToClient, "%d\n", *currentEntryValue);
    } else {
        fputs(";\n", streamToClient);
    }
}

/*
 *Reply all the mapped controls.
 */
void reply_all(FILE* streamToClient) {
    int i = 0;
    char* currentEntry = NULL;
    int* currentEntryValue = NULL;

    mapper_sort_control_map(controlMap, mappedControls);

    for (i = 0; i < mappedControls; i++) {
        currentEntry = controlMap[i];
        currentEntryValue = (int*)(currentEntry + MAPPER_MAX_ID_SIZE);
        fprintf(streamToClient, "%s:%d\n", currentEntry, *currentEntryValue);
    }
}

/*
 *Process the client's request.
 */
void process_requests(int fileToClientNo) {
    char buffer[128];
    FILE* streamToClient = NULL;

    if (EXIT_SUCCESS != open_stream(fileToClientNo, &streamToClient)) {
        return;
    }

    while (1) {
        if (!fgets(buffer, sizeof(buffer), streamToClient)) {
            break;
        }

        pthread_mutex_lock(&controlMapGuard);

        switch (buffer[0]) {
            case '!':
                add_entry(buffer + 1);
                break;
            case '?':
                reply_entry(buffer + 1, streamToClient);
                break;
            case '@':
                reply_all(streamToClient);
                break;
            default:
                break;
        }

        pthread_mutex_unlock(&controlMapGuard);
    }

    /*printf("Close stream\n");*/
    fclose(streamToClient);
}

/*
 *The new launched thread's starting point.
 */
void* thread_main(void* parameter) {
    int clientSocket = *(int*)parameter;
    pthread_mutex_unlock(&clientSocketGuard);

    process_requests(clientSocket);

    /*printf("Close connection\n");*/
    mapper_close_conn(clientSocket);

    return NULL;
}

/*
 *Listen on an ephemeral port for clients.
 */
int listen_for_clients() {
    int success = EXIT_SUCCESS;
    int port = 0;
    int acceptSocket = 0;
    int clientSocket = 0;
    pthread_t clientThread;
    pthread_attr_t clientThreadOptions;

    acceptSocket = mapper_open_incoming_conn(&port);
    fprintf(stdout, "%d\n", port);
    fflush(stdout);

    listen(acceptSocket, CONTROL_MAX_CONNECTIONS);

    pthread_attr_init(&clientThreadOptions);
    pthread_attr_setdetachstate(&clientThreadOptions,
            PTHREAD_CREATE_DETACHED);

    while (1) {
        pthread_mutex_lock(&clientSocketGuard);
        clientSocket = accept(acceptSocket, NULL, NULL);
        if (0 > clientSocket) {
            pthread_mutex_unlock(&clientSocketGuard);
            continue;
        }

        if (0 != pthread_create(&clientThread, &clientThreadOptions,
                thread_main, &clientSocket)) {
            pthread_mutex_unlock(&clientSocketGuard);
            success = EXIT_FAILURE;
            break;
        }
    }

    mapper_close_conn(acceptSocket);
    pthread_attr_destroy(&clientThreadOptions);
    return success;
}

int main(int argc, char* argv[]) {
    int success = EXIT_SUCCESS;

    controlMap = mapper_alloc_map(MAPPER_MAX_CONTROL_COUNT, MAPPER_MAX_ID_SIZE
            + sizeof(int));
    mappedControls = 0;

    success = listen_for_clients();

    free(controlMap);
    return success;
}

