#include "command_server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

struct _CommandServer {
	pthread_t thread;
	pthread_mutex_t thread_mutex;
	pthread_mutex_t thread_started_mutex;
	pthread_cond_t thread_started;
	int server_fd;
	CommandRunner *command_runner;
	volatile bool thread_stop;
	volatile bool finished;
};

void *_command_server_thread(void *arg);

CommandServer *command_server_new(CommandRunner *command_runner) {
	CommandServer *command_server = malloc(sizeof(CommandServer));
	memset(command_server, 0, sizeof(*command_server));
	command_server->thread = 0;
	pthread_mutex_init(&command_server->thread_mutex, NULL);
	pthread_mutex_init(&command_server->thread_started_mutex, NULL);
	pthread_cond_init(&command_server->thread_started, NULL);
	command_server->command_runner = command_runner;
	return command_server;
}

void command_server_free(CommandServer *command_server) {
	free(command_server);
}

bool command_server_start(CommandServer *command_server) {
	bool error = false;
	struct sockaddr_in server_addr;

	pthread_mutex_lock(&command_server->thread_mutex);
	{
		if (command_server->thread != 0) {
			error = true;
		}

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

		if (!error) {
			command_server->thread_stop = false;
			if (pthread_create(&command_server->thread, NULL, _command_server_thread, command_server) != 0) {
				perror("Error creating control server thread");
				error = true;
			}
		}
	}
	pthread_mutex_unlock(&command_server->thread_mutex);

	return !error;
}

bool command_server_wait(CommandServer *command_server) {
	pthread_cond_wait(&command_server->thread_started, &command_server->thread_started_mutex);
	return true;
}

int command_server_join(CommandServer *command_server) {
	return pthread_join(command_server->thread, NULL);
}

bool command_server_stop(CommandServer *command_server) {
	bool error = false;

	pthread_mutex_lock(&command_server->thread_mutex);
	{
		if (command_server->thread == 0) {
			error = true;
		}

		if (!error) {
			command_server->thread_stop = true;
			// Note: the thread will block on recvfrom. We conveniently work
			// around that because sending a stop command through UDP is the
			// only (accepted) way to exit. We may be able to use
			// pthread_cancel safely since open files are cleaned up by the
			// parent thread.
			if (pthread_join(command_server->thread, NULL) != 0) {
				perror("Error exiting control server thread");
				error = true;
			}
		}

		if (!error) {
			close(command_server->server_fd);
			command_server->thread = 0;
		}
	}
	pthread_mutex_unlock(&command_server->thread_mutex);

	return !error;
}

bool command_server_is_running(CommandServer *command_server) {
	return command_server->thread != 0 && command_server->finished == false;
}

int get_tokens(char *command, char **tokens, int buffer_size) {
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

void *_command_server_thread(void *arg) {
	CommandServer *command_server = (CommandServer *)arg;

	char receive_buffer[COMMAND_SERVER_RECEIVE_BUFFER_LENGTH+1] = {0};
	char response_buffer[COMMAND_SERVER_RESPONSE_BUFFER_LENGTH+1] = {0};

	pthread_cond_signal(&command_server->thread_started);

	while (!command_server->thread_stop) {
		struct sockaddr_in client_addr;
		socklen_t client_addr_size = sizeof(client_addr);

		int receive_size = recvfrom(
			command_server->server_fd, receive_buffer, COMMAND_SERVER_RECEIVE_BUFFER_LENGTH, 0,
			(struct sockaddr *) &client_addr, &client_addr_size
		);

		if (receive_size == -1) {
			perror("Error receiving data");
			continue;
		} else {
			receive_buffer[receive_size] = '\0';
		}

		char *tokens[32];
		int tokens_count = get_tokens(receive_buffer, (char **) &tokens, 32);

		size_t response_size;
		CommandResult command_result = command_runner_run(
			command_server->command_runner, (const char **)tokens, tokens_count,
			response_buffer, COMMAND_SERVER_RESPONSE_BUFFER_LENGTH, &response_size
		);

		if (command_result >= COMMAND_ERROR) {
			fprintf(stderr, "Error running command: %d\n", command_result);
		}

		if (response_size > 0) {
			int sent = sendto(command_server->server_fd, response_buffer, response_size, 0, (struct sockaddr *)&client_addr, client_addr_size);

			if (sent < 0) {
				perror("Error sending response");
			}
		}

		if (command_result == COMMAND_SUCCESS_STOP) {
			// Exit manually. Otherwise, we may not see the signal from the
			// main thread  because we may be blocking on recvfrom by then. It
			// is possible to kill the thread manually, but at this point
			// unnecessary.
			command_server->thread_stop = true;
		}
	}

	command_server->finished = true;

	return NULL;
}
