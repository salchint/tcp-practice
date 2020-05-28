/*
 *protocol.h
 */

#pragma once

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

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
#define CONTROL_MAX_ID_SIZE 512

/*
 *The maximum number of planes that can be logged.
 */
#define CONTROL_MAX_PLANE_COUNT 512

/*
 *Allocate a map as a contignuous chunk.
 */
char** control_alloc_log(int rows, int columns);

/*
 *Create a server-like socket and wait for incoming connections.
 *Returns the socket's file descriptor.
 */
int control_open_incoming_conn(int* port);

/*
 *Close the given socket.
 */
void control_close_conn(int closeSocket);

/*
 *Validate the given port number.
 */
int control_check_port(const char* arg);

/*
 *Search for invalid characters in the arguments.
 */
int control_check_chars(const char* arg);

#endif
