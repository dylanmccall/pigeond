#include "command_server.h"

#include "long_thread.h"

#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

struct _CommandServer {
	LongThread *long_thread;
	int server_fd;
	CommandRunner *command_runner;
	char receive_buffer[COMMAND_SERVER_RECEIVE_BUFFER_LENGTH+1];
	char response_buffer[COMMAND_SERVER_RESPONSE_BUFFER_LENGTH+1];
};

void *_command_server_thread(void *arg);
int _get_tokens(char *command, char **tokens, int buffer_size);

bool _command_server_thread_start(LongThread *long_thread, void *data);
bool _command_server_thread_stop(LongThread *long_thread, void *data);
LongThreadResult _command_server_thread_loop(LongThread *long_thread, void *data);

CommandServer *command_server_new(CommandRunner *command_runner) {
	CommandServer *command_server = malloc(sizeof(CommandServer));
	memset(command_server, 0, sizeof(*command_server));
	command_server->long_thread = long_thread_new((LongThreadOptions){
		.name="commandserver",
		.start_fn=_command_server_thread_start,
		.stop_fn=_command_server_thread_stop,
		.loop_fn=_command_server_thread_loop,
		.data=command_server
	});
	command_server->command_runner = command_runner;
	return command_server;
}

void command_server_free(CommandServer *command_server) {
	long_thread_free(command_server->long_thread);
	free(command_server);
}

bool command_server_start(CommandServer *command_server) {
	return long_thread_start(command_server->long_thread);
}

bool command_server_wait(CommandServer *command_server) {
	return long_thread_wait(command_server->long_thread);
}

int command_server_join(CommandServer *command_server) {
	return long_thread_join(command_server->long_thread);
}

bool command_server_stop(CommandServer *command_server) {
	return long_thread_stop(command_server->long_thread);
}

bool command_server_is_running(CommandServer *command_server) {
	return long_thread_is_running(command_server->long_thread);
}

bool _command_server_thread_start(LongThread *long_thread, void *data) {
	CommandServer *command_server = (CommandServer *)data;

	bool error = false;
	struct sockaddr_in server_addr;

	if (!error) {
		command_server->server_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (command_server->server_fd == -1) {
			perror("Error opening socket");
			error = true;
		}
	}

	if (!error) {
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons(COMMAND_SERVER_PORT);
		if (bind(command_server->server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
			perror("Could not bind to port");
			error = true;
		}
	}

	return !error;
}

bool _command_server_thread_stop(LongThread *long_thread, void *data) {
	CommandServer *command_server = (CommandServer *)data;

	bool error = false;

	if (!error) {
		close(command_server->server_fd);
	}

	return !error;
}

LongThreadResult _command_server_thread_loop(LongThread *long_thread, void *data) {
	CommandServer *command_server = (CommandServer *)data;

	struct sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);

	int receive_size = recvfrom(
		command_server->server_fd, command_server->receive_buffer, COMMAND_SERVER_RECEIVE_BUFFER_LENGTH, 0,
		(struct sockaddr *) &client_addr, &client_addr_size
	);

	if (receive_size == -1) {
		perror("Error receiving data");
		return LONG_THREAD_CONTINUE;
	} else {
		command_server->receive_buffer[receive_size] = '\0';
	}

	char *tokens[32];
	int tokens_count = _get_tokens(command_server->receive_buffer, (char **) &tokens, 32);

	size_t response_size;
	CommandResult command_result = command_runner_run(
		command_server->command_runner, (const char **)tokens, tokens_count,
		command_server->response_buffer, COMMAND_SERVER_RESPONSE_BUFFER_LENGTH, &response_size
	);

	if (command_result >= COMMAND_ERROR) {
		fprintf(stderr, "Error running command: %d\n", command_result);
	}

	if (response_size > 0) {
		int sent = sendto(command_server->server_fd, command_server->response_buffer, response_size, 0, (struct sockaddr *)&client_addr, client_addr_size);

		if (sent < 0) {
			perror("Error sending response");
		}
	}

	if (command_result == COMMAND_SUCCESS_STOP) {
		// Exit manually. Otherwise, we may not see the signal from the
		// main thread  because we may be blocking on recvfrom by then. It
		// is possible to kill the thread manually, but at this point
		// unnecessary.
		return LONG_THREAD_STOP;
	}

	return LONG_THREAD_CONTINUE;
}

int _get_tokens(char *command, char **tokens, int buffer_size) {
	int count = 0;

	char *token;
	char *saveptr;

	token = strtok_r(command, " \t\n", &saveptr);
	tokens[count] = token;
	count++;

	while (token != NULL && count < buffer_size) {
		token = strtok_r(NULL, " \t\n", &saveptr);
		tokens[count] = token;
		count++;
	}

	return count;
}
