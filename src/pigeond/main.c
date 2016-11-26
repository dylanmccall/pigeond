/**
 * Pigeond
 * Creates a virtual network device that transmits packets using the Avian
 * Carrier over IP protocol.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include "command_runner.h"
#include "command_server.h"
#include "pigeon_frame_pipe.h"
#include "pigeon_link.h"
#include "pigeon_tunnel.h"

#include <stdio.h>
#include <string.h>

// We hard-code the device name so we can create a persistent device. If we
// decide not to use a persistent device, it is better to use a format string
// like "pigeon%d" so it will be assigned a unique identifier.

#define PIGEON_TUNNEL_NAME "pigeon0"

int main() {
	PigeonFramePipe *pigeon_frame_pipe = NULL;
	PigeonTunnel *pigeon_tunnel = NULL;
	PigeonLink *pigeon_link = NULL;
	CommandRunner *command_runner = NULL;
	CommandServer *command_server = NULL;
	bool error = false;

	pigeon_frame_pipe = pigeon_frame_pipe_new();

	pigeon_tunnel = pigeon_tunnel_new(
		PIGEON_TUNNEL_NAME,
		pigeon_frame_pipe_get_tx(pigeon_frame_pipe)
	);

	pigeon_link = pigeon_link_new(
		pigeon_frame_pipe_get_rx(pigeon_frame_pipe)
	);

	command_runner = command_runner_new();

	command_server = command_server_new(command_runner);

	if (!error) {
		if (!pigeon_tunnel_init(pigeon_tunnel)) {
			perror("Error initializing tunnel device");
			error = true;
		} else {
			const char *dev_name = pigeon_tunnel_get_dev_name(pigeon_tunnel);
			printf("Opened tunnel device %s\n", dev_name);
		}
	}

	if (!error) {
		if (!pigeon_link_init(pigeon_link)) {
			perror("Error initializing pigeon link");
			error = true;
		}

		pigeon_link_print_debug_info(pigeon_link);
	}

	if (!error) {
		if (!pigeon_tunnel_set_mtu(pigeon_tunnel, PIGEON_LINK_MTU)) {
			perror("Error setting MTU");
			error = true;
		} else {
			printf("Set MTU to %d\n", PIGEON_LINK_MTU);
		}
	}

	if (!error) {
		if (!command_server_start(command_server)) {
			printf("Error starting command server\n");
			error = true;
		}
	}

	// TODO: Enable seccomp here. It is important this happens before we
	//       process data.
	// TODO: We should have separate init and start for command_server, so
	//       it doesn't run until after seccomp is enabled.

	if (!error) {
		if (!pigeon_tunnel_start(pigeon_tunnel)) {
			printf("Error starting pigeon tunnel\n");
			error = true;
		}
	}

	if (!error) {
		if (!pigeon_link_start(pigeon_link)) {
			printf("Error starting pigeon link\n");
			error = true;
		}
	}

	if (!error) {
		// Very ugly placeholder code to read from the device and dump results
		// to the screen.
		char buffer[2048];
		memset(buffer, 0, sizeof(buffer));
		while(command_server_is_running(command_server)) {
			PigeonFrame *pigeon_frame = pigeon_tunnel_frames_pop(pigeon_tunnel);
			if (pigeon_frame != NULL) {
				pigeon_frame_print_header(pigeon_frame);
				pigeon_frame_print_data(pigeon_frame);
				printf("\n");
				pigeon_frame_free(pigeon_frame);
			} else {
				fprintf("Error reading from tunnel device\n");
			}
		}
	};
	command_runner = NULL;

	printf("Exiting…\n");

	command_server_stop(command_server);
	pigeon_link_stop(pigeon_link);
	pigeon_tunnel_stop(pigeon_tunnel);

	pigeon_tunnel = NULL;

	command_server_free(command_server);
	command_server = NULL;

	command_runner_free(command_runner);
	command_runner = NULL;

	pigeon_link_free(pigeon_link);
	pigeon_link = NULL;

	return 0;
}
