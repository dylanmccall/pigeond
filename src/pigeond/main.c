/**
 * Pigeond
 * Creates a virtual network device that transmits packets using the Avian
 * Carrier over IP protocol.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include "command_runner.h"
#include "command_server.h"
#include "pigeon_tunnel.h"
#include "pigeon_transmit.h"

#include <stdio.h>
#include <string.h>

// We hard-code the device name so we can create a persistent device. If we
// decide not to use a persistent device, it is better to use a format string
// like "pigeon%d" so it will be assigned a unique identifier.

#define PIGEON_TUNNEL_NAME "pigeon0"

int main() {
	PigeonTransmit *pigeon_transmit = NULL;
	CommandRunner *command_runner = NULL;
	CommandServer *command_server = NULL;
	PigeonTunnel *pigeon_tunnel = NULL;
	bool success = true;

	pigeon_transmit = pigeon_transmit_new();
	command_runner = command_runner_new();
	command_server = command_server_new(command_runner);

	pigeon_tunnel = pigeon_tunnel_open(PIGEON_TUNNEL_NAME);

	if (pigeon_tunnel != NULL) {
		const char *dev_name = pigeon_tunnel_get_dev_name(pigeon_tunnel);
		printf("Opened tunnel device %s\n", dev_name);
	} else {
		perror("Error opening tunnel device");
		success = false;
	}

	success &= pigeon_transmit_start(pigeon_transmit);
	success &= command_server_start(command_server);

	if (success) {
		// Very ugly placeholder code to read from the device and dump results
		// to the screen (with some simple heuristics to show strings).

		if (!pigeon_tunnel_set_mtu(pigeon_tunnel, PIGEON_TRANSMIT_MTU)) {
			perror("Error setting MTU");
		}

		char buffer[2048];
		memset(buffer, 0, sizeof(buffer));
		while(command_server_is_running(command_server)) {
			PigeonFrame *pigeon_frame = pigeon_tunnel_read(pigeon_tunnel);
			if (pigeon_frame != NULL) {
				pigeon_frame_print_header(pigeon_frame);
				pigeon_frame_print_data(pigeon_frame);
				printf("\n");
				pigeon_frame_free(pigeon_frame);
			} else {
				perror("Error reading from tunnel device");
			}
		}
	};
	command_runner = NULL;

	printf("Exiting…\n");

	command_server_stop(command_server);
	pigeon_transmit_stop(pigeon_transmit);

	pigeon_tunnel_close(pigeon_tunnel);
	pigeon_tunnel = NULL;

	command_server_free(command_server);
	command_server = NULL;

	command_runner_free(command_runner);
	command_runner = NULL;

	pigeon_transmit_free(pigeon_transmit);
	pigeon_transmit = NULL;

	return 0;
}
