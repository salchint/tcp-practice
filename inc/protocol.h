/*
 *protocol.h
 */

#pragma once

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>

#include "errorReturn.h"

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/*
 *The maximum amount of concurrently connected planes.
 */
#define CONTROL_MAX_CONNECTIONS 16

/*
 *The maximum length of a plane's ID.
 */
#define CONTROL_MAX_ID_SIZE 80

/*
 *The maximum number of planes that can be logged.
 */
#define CONTROL_MAX_PLANE_COUNT 1024

/*
 *The maximum number of destinations that can be logged.
 */
#define ROC_MAX_DESTINATION_COUNT 1024

/*
 *The maximum length of a airport's info text.
 */
#define ROC_MAX_INFO_SIZE 80

/*
 *The maximum number of controls, that the map can hold.
 */
#define MAPPER_MAX_CONTROL_COUNT 1024

/*
 *The maximum length of a map entry.
 */
#define MAPPER_MAX_ID_SIZE 80

/*
 *Allocate a map of airports and port numbers.
 */
char** mapper_alloc_map(int rows, int columns);

/*
 *Allocate an array of airport info strings.
 */
char** roc_alloc_log(int rows, int columns);

/*
 *Allocate an array of airplane IDs.
 */
char** control_alloc_log(int rows, int columns);

/*
 *Create a server-like socket and wait for incoming connections.
 *Returns the socket's file descriptor.
 */
int control_open_incoming_conn(int* port);

/*
 *Create a server-like socket and wait for incoming connections.
 *Returns the socket's file descriptor.
 */
int mapper_open_incoming_conn(int* port);

/*
 *Open connection to the given destination airport.
 */
int roc_open_destination_conn(int port);

/*
 *Open connection to the given mapper.
 */
int control_open_mapper_conn(int port);

/*
 *Close the given socket.
 */
void control_close_conn(int closeSocket);

/*
 *Close the given socket.
 */
void roc_close_conn(int closeSocket);

/*
 *Close the given socket.
 */
void mapper_close_conn(int closeSocket);

/*
 *Validate the given port number.
 */
int control_check_port(const char* arg);

/*
 *Validate the given port number.
 */
int roc_check_port(const char* arg);

/*
 *Validate the given port number.
 */
int roc_check_destination_port(const char* arg);

/*
 *Search for invalid characters in the arguments.
 */
int control_check_chars(const char* arg);

/*
 *Search for invalid characters in the arguments.
 */
int roc_check_chars(const char* arg);

/*
 *Search for invalid characters in the arguments.
 */
int mapper_check_chars(const char* arg, const char* end);

/*
 *Sort the collected airplane identifiers in lexicographic order.
 */
void control_sort_plane_log(char** planesLog, int loggedPlanes);

/*
 *Sort the collected airport identifiers in lexicographic order.
 */
void mapper_sort_control_map(char** controlMap, int mappedControls);

/*
 *Look up the given destination airport if needed.
 *Returns the port number of the given airport on success, 0 else.
 */
int roc_resolve_control(int mapperPort, const char* destination);

/*
 *Remove the trailing LF from the given string if present.
 */
void roc_trim_string_end(char* text);

/*
 *Remove the trailing LF from the given string if present.
 */
void mapper_trim_string_end(char* text);

/*
 *Open a stream object from the given client socket.
 */
int open_socket_stream(int socketNumber, FILE** stream);

/*
 *Register the airport's port number with the mapper.
*/
int control_register_id(int mapperPort, int acceptPort, const char* id);

#endif
