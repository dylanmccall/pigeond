#include "command_runner.h"

#include "util.h"

#include <string.h>
#include <stdarg.h>

struct _CommandRunner {
};

typedef struct {
	const char **tokens;
	unsigned int tokens_count;
	char* response_buffer;
	size_t buffer_size;
	size_t buffer_index;
} CommandContext;

typedef CommandResult (*command_fn_t)(CommandContext *);

int _command_respond_printf(CommandContext *context, const char *format_str, ...);
size_t _command_param_string(CommandContext *context, const char **out_str);
long _command_param_long(CommandContext *context);
long _command_param_positive_long(CommandContext *context, long default_value);
void _command_show_primes(CommandContext *context, unsigned int show_count, int direction);

CommandResult run_command_not_found(CommandContext *context);
CommandResult run_command_help(CommandContext *context);
CommandResult run_command_stop(CommandContext *context);

typedef struct {
	const char* name;
	const char* help_name;
	const char* description;
	command_fn_t command_fn;
} CommandInfo;

const CommandInfo COMMANDS[] = {
	{
		.name = "help",
		.help_name = "help",
		.description = "Return a brief summary of supported commands.",
		.command_fn = run_command_help
	},
	{
		.name = "stop",
		.help_name = "stop",
		.description = "Stop the network interface and exit the program",
		.command_fn = run_command_stop
	}
};

const size_t COMMANDS_COUNT = sizeof(COMMANDS) / sizeof(*COMMANDS);

const CommandInfo COMMAND_NOT_FOUND = {
	.name = NULL,
	.help_name = NULL,
	.description = NULL,
	.command_fn = run_command_not_found
};


CommandRunner *command_runner_new() {
	CommandRunner *command_runner = malloc(sizeof(CommandRunner));
	memset(command_runner, 0, sizeof(*command_runner));
	return command_runner;
}

void command_runner_free(CommandRunner *command_runner) {
	free(command_runner);
}

const CommandInfo *get_command_by_name(CommandContext *context) {
	const char *command_name;

	if (context->tokens_count > 0) {
		command_name = context->tokens[0];
	}

	if (command_name == NULL) {
		return NULL;
	}

	for (size_t i = 0; i < COMMANDS_COUNT; i++) {
		const CommandInfo *command = &COMMANDS[i];
		if (strcmp(command->name, command_name) == 0) {
			return command;
		}
	}

	return &COMMAND_NOT_FOUND;
}

CommandResult command_runner_run(CommandRunner *command_runner, const char **tokens, unsigned int tokens_count, char *response_buffer, int buffer_size, size_t *out_size) {
	CommandContext context = {
		.tokens=tokens,
		.tokens_count=tokens_count,
		.response_buffer=response_buffer,
		.buffer_size=buffer_size,
		.buffer_index=0
	};

	CommandResult result;
	const CommandInfo *command = get_command_by_name(&context);

	if (command == NULL) {
		result = COMMAND_SUCCESS;
	} else {
		result = command->command_fn(&context);
	}

	if (context.buffer_index > buffer_size) {
		result = COMMAND_RESPONSE_TOO_LARGE;
		context.buffer_index = 0;
		_command_respond_printf(&context, "Error: response is too large\n");
	}

	if (context.buffer_index <= buffer_size) {
		*out_size = context.buffer_index;
	} else {
		*out_size = buffer_size;
	}

	return result;
}

CommandResult run_command_not_found(CommandContext *context) {
	const char * command_name;

	if (context->tokens_count > 0) {
		command_name = context->tokens[0];
	}

	if (command_name) {
		_command_respond_printf(context, "%s: command not found\n", command_name);
	} else {
		_command_respond_printf(context, "command not found\n", command_name);
	}

	return COMMAND_SUCCESS;
}

CommandResult run_command_help(CommandContext *context) {
	UNUSED(context);
	// We dangerously assume the client supports ANSI escape codes, because it
	// looks pretty :)
	for (size_t i = 0; i < COMMANDS_COUNT; i++) {
		const CommandInfo *command = &COMMANDS[i];
		if (command->help_name != NULL) {
			_command_respond_printf(context, "\033[1m%s\033[0m\n%s\n\n", command->help_name, command->description);
		}
	}
	return COMMAND_SUCCESS;
}

CommandResult run_command_stop(CommandContext *context) {
	_command_respond_printf(context, "Exiting…\n");
	return COMMAND_SUCCESS_STOP;
}

int _command_respond_printf(CommandContext *context, const char *format_str, ...) {
	char *buffer_start;
	size_t buffer_remaining;
	int result_size;

	buffer_start = context->response_buffer + context->buffer_index;

	if (context->buffer_size > context->buffer_index) {
		buffer_remaining = context->buffer_size - context->buffer_index;
	} else {
		buffer_remaining = 0;
	}

	va_list args;
	va_start(args, format_str);
	result_size = vsnprintf(buffer_start, buffer_remaining, format_str, args);
    va_end(args);

	if (result_size > 0 && result_size < buffer_remaining) {
		context->buffer_index += result_size;
	} else if (result_size >= buffer_remaining) {
		context->buffer_index += result_size;
	}

	return result_size;
}

size_t _command_param_string(CommandContext *context, const char **out_str) {
	if (context->tokens_count > 1) {
		*out_str = context->tokens[1];
		return *out_str != NULL ? strlen(*out_str) : 0;
	} else {
		return 0;
	}
}

long _command_param_long(CommandContext *context) {
	const char *param_str;

	if (context->tokens_count > 1) {
		param_str = context->tokens[1];
	}

	if (param_str) {
		return atol(param_str);
	} else {
		return 0;
	}
}

long _command_param_positive_long(CommandContext *context, long default_value) {
	const char *param_str;

	if (context->tokens_count > 1) {
		param_str = context->tokens[1];
	}

	if (param_str) {
		long value = atol(param_str);
		if (value > 0) {
			return value;
		} else {
			return 0;
		}
	} else {
		return default_value;
	}
}
