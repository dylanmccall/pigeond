#ifndef _COMMAND_RUNNER_H
#define _COMMAND_RUNNER_H

/**
 * command_runner.h
 * Generic command runner for server commands.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    COMMAND_SUCCESS = 0,
    COMMAND_SUCCESS_STOP,
    COMMAND_RESPONSE_TOO_LARGE,
    COMMAND_ERROR,
    COMMAND_NOT_IMPLEMENTED,
} CommandResult;

typedef struct _CommandRunner CommandRunner;

CommandRunner *command_runner_new();
void command_runner_free(CommandRunner *command_runner);
CommandResult command_runner_run(CommandRunner *command_runner, const char **tokens, unsigned int tokens_count, char *response_buffer, int buffer_size, size_t *out_size);

#endif
