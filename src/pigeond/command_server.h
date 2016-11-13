#ifndef _COMMAND_SERVER_H
#define _COMMAND_SERVER_H

/**
 * command_server.h
 * Runs a server that listens for commands on UDP port 9163.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include "command_runner.h"

#include <stdbool.h>
#include <stdlib.h>

#define COMMAND_SERVER_PORT 9163
#define COMMAND_SERVER_RECEIVE_BUFFER_LENGTH 255
#define COMMAND_SERVER_RESPONSE_BUFFER_LENGTH 1452

// We constrain the buffer size to 1452 bytes (1500 - 40 - 8), as suggested
// over here: http://stackoverflow.com/questions/22773391/what-is-the-maximum-buffer-length-allowed-by-the-sendto-function-in-c
// Note that packets can still exceed the size allowed by the network.

typedef struct _CommandServer CommandServer;

CommandServer *command_server_new(CommandRunner *command_runner);
void command_server_free(CommandServer *command_server);
bool command_server_start(CommandServer *command_server);
bool command_server_wait(CommandServer *command_server);
int command_server_join(CommandServer *command_server);
bool command_server_stop(CommandServer *command_server);
bool command_server_is_working(CommandServer *command_server);

#endif
