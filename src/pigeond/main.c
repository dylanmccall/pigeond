/**
 * Pigeond
 * Creates a virtual network device that transmits packets using the Avian
 * Carrier over IP protocol.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include "command_runner.h"
#include "command_server.h"
#include "pigeon_tunnel.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

// We hard-code the device name so we can create a persistent device. If we
// decide not to use a persistent device, it is better to use a format string
// like "pigeon%d" so we will be assigned a unique identifier.

#define PIGEON_TUNNEL_NAME "pigeon0"

int main() {
	PigeonTunnel *pigeon_tunnel = NULL;
	CommandRunner *command_runner = NULL;
	CommandServer *command_server = NULL;
	bool success = true;

	pigeon_tunnel = pigeon_tunnel_open(PIGEON_TUNNEL_NAME);
	command_runner = command_runner_new();
	command_server = command_server_new(command_runner);

	if (pigeon_tunnel != NULL) {
		const char *dev_name = pigeon_tunnel_get_dev_name(pigeon_tunnel);
		printf("Created tunnel device %s\n", dev_name);
	} else {
		perror("Error creating tunnel device\n");
	}

	success &= command_server_start(command_server);

	if (success) {
		// Very ugly placeholder code to read from the device and dump results
		// to the screen (with some simple heuristics to show strings).
		char buffer[2048];
		memset(buffer, 0, sizeof(buffer));
		while(true) {
			int result = pigeon_tunnel_read(pigeon_tunnel, buffer, sizeof(buffer));
			if (result < 0) {
				perror("Error reading from tun device");
			} else {
				printf("Read %d bytes from device:\n", result);
				bool in_alnum = false;
				for (int i = 0; i < result; i++) {
					char value = buffer[i];
					bool is_alnum = isalnum(value);

					if (is_alnum != in_alnum) {
						printf("\"");
						if (!is_alnum) {
							printf(" ");
						}
					}

					if (is_alnum) {
						printf("%c", value);
					} else {
						printf("%x ", buffer[i]);
					}

					in_alnum = is_alnum;
				}
				printf("\n");
			}
		}
		// command_server_join(command_server);
	};
	command_runner = NULL;

	printf("Exiting…\n");

	command_server_stop(command_server);

	command_server_free(command_server);
	command_server = NULL;

	command_runner_free(command_runner);
	command_runner = NULL;

	pigeon_tunnel_close(pigeon_tunnel);
	pigeon_tunnel = NULL;

	return 0;
}
