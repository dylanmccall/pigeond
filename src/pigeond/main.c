/**
 * Pigeond
 * Creates a virtual network device that transmits packets using the Avian
 * Carrier over IP protocol.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include "command_runner.h"
#include "command_server.h"

#include <stdio.h>

int main() {
	CommandRunner *command_runner = NULL;
	CommandServer *command_server = NULL;
	bool success = true;

	command_runner = command_runner_new();
	command_server = command_server_new(command_runner);

	success &= command_server_start(command_server);

	if (success) {
		command_server_join(command_server);
	};
	command_runner = NULL;

	printf("Exiting…\n");

	command_server_stop(command_server);

	command_server_free(command_server);
	command_server = NULL;

	command_runner_free(command_runner);
	command_runner = NULL;

	return 0;
}
