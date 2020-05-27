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

#define CONTROL_MAX_CONNECTIONS 16

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
