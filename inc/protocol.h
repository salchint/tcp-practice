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
 /**
  * Returns a if it is greater than b, else b.
  */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
 /**
  * Returns a if it is lower than b, else b.
  */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/**
 * The maximum amount of concurrently connected planes.
 */
#define CONTROL_MAX_CONNECTIONS 16

/**
 * The maximum length of a plane's ID.
 */
#define CONTROL_MAX_ID_SIZE 80

/**
 * The maximum number of planes that can be logged.
 */
#define CONTROL_MAX_PLANE_COUNT 1024

/**
 * The maximum number of destinations that can be logged.
 */
#define ROC_MAX_DESTINATION_COUNT 1024

/**
 * The maximum length of a airport's info text.
 */
#define ROC_MAX_INFO_SIZE 80

/**
 * The maximum number of controls, that the map can hold.
 */
#define MAPPER_MAX_CONTROL_COUNT 1024

/**
 * The maximum length of a map entry.
 */
#define MAPPER_MAX_ID_SIZE 80

/**
 * Allocate a map of airports and port numbers.
 *
 * The allocated map contains a header containing pointers to all rows.
 * All rows have the same length. This returns a pointer to the start of rows,
 * which in turn are pointers to columns each. You can free this "array of
 * arrays" via one single free() invocation passing on the returned
 * pointers-pointer.
 *
 * @param rows The number of map entries (1st demension).
 *
 * @param columns The size of a map entry, i.e. the maximum length of the
 *                airport ID + the port number (2nd dimension).
 */
char** mapper_alloc_map(int rows, int columns);

/**
 *Allocate an array of airport info strings.
 *
 * The allocated logging buffer contains a header containing pointers to all
 * rows.  All rows have the same length. This returns a pointer to the start of
 * rows, which in turn are pointers to columns each. You can free this "array
 * of arrays" via one single free() invocation passing on the returned
 * pointers-pointer.
 *
 * @param rows The number of log entries (1st demension).
 *
 * @param columns The size of a log entry, i.e. the maximum length of the
 *                airport ID (2nd dimension).
 */
char** roc_alloc_log(int rows, int columns);

/**
 *Allocate an array of airplane IDs.
 *
 * The allocated logging buffer contains a header containing pointers to all
 * rows.  All rows have the same length. This returns a pointer to the start of
 * rows, which in turn are pointers to columns each. You can free this "array
 * of arrays" via one single free() invocation passing on the returned
 * pointers-pointer.
 *
 * @param rows The number of log entries (1st demension).
 *
 * @param columns The size of a log entry, i.e. the maximum length of the
 *                airplane ID (2nd dimension).
 */
char** control_alloc_log(int rows, int columns);

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
int control_open_incoming_conn(int* port);

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
int mapper_open_incoming_conn(int* port);

/**
 * Open connection to the given destination airport.
 *
 * Returns a TCP client socket, which is created and connected to the server at
 * the given port. In case of an error, -1 is returned.
 */
int roc_open_destination_conn(int port);

/**
 * Open connection to the given mapper.
 *
 * Returns a TCP client socket, which is created and connected to the server at
 * the given port. In case of an error, -1 is returned.
 *
 * @param port  The port number at which the TCP server is listening.
 */
int control_open_mapper_conn(int port);

/**
 * Close the given socket.
 *
 * This closes the given socket and frees the associated system resources.
 */
void control_close_conn(int closeSocket);

/**
 * Close the given socket.
 *
 * This closes the given socket and frees the associated system resources.
 */
void roc_close_conn(int closeSocket);

/**
 * Close the given socket.
 *
 * This closes the given socket and frees the associated system resources.
 */
void mapper_close_conn(int closeSocket);

/**
 * Search for invalid characters in the arguments.
 *
 * Returns E_CONTROL_INVALID_INFO if the given arg exceeds the maximum
 * allowed length or contains either of LF, CR or ':', which are regarded
 * invalid, E_CONTROL_OK on success.
 *
 * @param arg The argument, which is to be verified.
 */
int control_check_chars(const char* arg);

/**
 * Search for invalid characters in the arguments.
 *
 * Returns E_ROC_INVALID_ARGS_COUNT if the given arg exceeds the maximum
 * allowed length or contains either of LF, CR or ':', which are regarded
 * invalid, E_ROC_OK on success.
 *
 * @param arg The argument, which is to be verified.
 */
int roc_check_chars(const char* arg);

/**
 * Search for invalid characters in the arguments.
 *
 * Returns EXIT_FAILURE if the given arg exceeds the maximum
 * allowed length or contains either of LF, CR or ':', which are regarded
 * invalid, EXIT_SUCCESS on success.
 *
 * @param arg The argument, which is to be verified.
 *
 * @param end If not NULL, this defines the end of arg, that is to be verified.
 *            The character end points to and later ones are excluded from
 *            verification.
 */
int mapper_check_chars(const char* arg, const char* end);

/**
 * Validate the given port number.
 *
 * Returns E_CONTROL_INVALID_PORT if the given port number exceeds the
 * maximum allowed length or contains either of LF, CR or ':', which are
 * regarded invalid or if it is a nuber, but out of the valid range of port
 * numbers. E_CONTROL_OK is retured on success.
 *
 * @param arg The port number, which is to be verified.
 */
int control_check_port(const char* arg);

/**
 * Validate the given port number.
 *
 * Returns E_ROC_INVALID_MAPPER_PORT if the given port number exceeds the
 * maximum allowed length or contains either of LF, CR or ':', which are
 * regarded invalid or if it is a nuber, but out of the valid range of port
 * numbers. E_ROC_OK is retured on success.
 *
 * @param arg The port number, which is to be verified.
 */
int roc_check_port(const char* arg);

/**
 *Validate the given port number.
 *
 * Returns E_ROC_INVALID_MAPPER_PORT if the given port number exceeds the
 * maximum allowed length or contains either of LF, CR or ':', which are
 * regarded invalid or if it is a nuber, but out of the valid range of port
 * numbers. E_ROC_OK is retured on success. '-' is regarded valid too.
 *
 * @param arg The argument, which is to be verified.
 */
int roc_check_destination_port(const char* arg);

/**
 * Sort the collected airplane identifiers in lexicographic order.
 *
 * @param planesLog Pointer to the logs holding the logged airplane IDs.
 *
 * @param loggedPlanes  The number of used entries in planesLog.
 */
void control_sort_plane_log(char** planesLog, int loggedPlanes);

/**
 * Sort the collected airport identifiers in lexicographic order.
 *
 * @param controlMap  Pointer to the logs holding the logged airplane IDs.
 *
 * @param mappedControls  The number of used entries in controlMap.
 */
void mapper_sort_control_map(char** controlMap, int mappedControls);

/**
 * Look up the given destination airport if needed.
 *
 * Returns the port number of the given airport on success, 0 else.
 *
 * @param mapperPort  The port number at which the mapper is listening.
 *
 * @param destination The airport ID to be looked up.
 */
int roc_resolve_control(int mapperPort, const char* destination);

/**
 * Remove the trailing LF from the given string if present.
 *
 * @param text  The string to be trimmed.
 */
void roc_trim_string_end(char* text);

/**
 * Remove the trailing LF from the given string if present.
 *
 * @param text  The string to be trimmed.
 */
void mapper_trim_string_end(char* text);

/**
 * Open a stream object from the given client socket.
 *
 * Returns EXIT_SUCCESS on success, EXIT_FAILURE else.
 *
 * @param socketNumber  The file descriptor of the socket, which we need to
 *                      read from or write to.
 *
 * @param stream  Output parameter, the file stream, which is created on
 *                success.
 */
int open_socket_stream(int socketNumber, FILE** stream);

/**
 * Register the airport's port number with the mapper.
 *
 * Returns E_CONTROL_FAILED_TO_CONNECT if the mapper cannot be connected to
 * using the given port number or if we cannot open a file stream from the
 * client socket. E_CONTROL_OK is returned on success.
 *
 * @param mapperPort  The port number at which the mapper is listening.
 *
 * @param acceptPort  This control's port number, that is to be registered with
 *                    the mapper.
 *
 * @param id  This airport's ID, that is to be registered with the mapper.
*/
int control_register_id(int mapperPort, int acceptPort, const char* id);

#endif
