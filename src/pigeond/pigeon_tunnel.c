#include "pigeon_tunnel.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <net/ethernet.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#define CLONE_DEV "/dev/net/tun"

struct _PigeonTunnel {
	int tun_fd;
	char dev_name[IFNAMSIZ];
};

PigeonTunnel *pigeon_tunnel_open(const char *dev_name) {
	bool error = false;
	PigeonTunnel *pigeon_tunnel = NULL;
	struct ifreq ifr;
	int tun_fd;

	if (!error) {
		tun_fd = open(CLONE_DEV, O_RDWR);
		if (tun_fd < 0) {
			perror("Error opening " CLONE_DEV);
			error = true;
		}
	}

	if (!error) {
		int result;
		ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
		strncpy(ifr.ifr_name, dev_name, IFNAMSIZ);
		result = ioctl(tun_fd, TUNSETIFF, (void *) &ifr);
		if (result < 0) {
			error = true;
		}
	}

	if (!error) {
		int result;
		result = ioctl(tun_fd, TUNSETPERSIST, 1);
		if (result < 0) {
			error = true;
		}
	}

	if (!error) {
		pigeon_tunnel = malloc(sizeof(PigeonTunnel));
		if (pigeon_tunnel == NULL) {
			error = true;
		}
	}

	if (!error) {
		memset(pigeon_tunnel, 0, sizeof(*pigeon_tunnel));
		pigeon_tunnel->tun_fd = tun_fd;
		strcpy(pigeon_tunnel->dev_name, ifr.ifr_name);
	} else {
		close(tun_fd);
		free(pigeon_tunnel);
		pigeon_tunnel = NULL;
	}

	return pigeon_tunnel;
}

bool pigeon_tunnel_close(PigeonTunnel *pigeon_tunnel) {
	bool error = false;

	if (!error) {
		if (pigeon_tunnel == NULL) {
			error = true;
		}
	}

	if (!error) {
		close(pigeon_tunnel->tun_fd);
	}

	if (!error) {
		free(pigeon_tunnel);
	}

	return !error;
}

const char *pigeon_tunnel_get_dev_name(PigeonTunnel *pigeon_tunnel) {
	return pigeon_tunnel->dev_name;
}

PigeonFrame *pigeon_tunnel_read(PigeonTunnel *pigeon_tunnel) {
	PigeonFrame *pigeon_frame;
	char buffer[ETHER_MAX_LEN] = {0};
	size_t bytes_read = read(pigeon_tunnel->tun_fd, &buffer, sizeof(buffer));

	if (bytes_read < 0) {
		perror("Error reading from tunnel device");
		pigeon_frame = NULL;
	} else if (bytes_read > ETHER_MAX_LEN) {
		fprintf(stderr, "Dropping oversized frame");
		fprintf(stderr, "Packet is %lu bytes. Expected <= %d bytes.\n", (unsigned long)bytes_read, ETHER_MAX_LEN);
		pigeon_frame = NULL;
		// We don't worry about undersized packets. Those will be padded automatically.
	} else {
		pigeon_frame = pigeon_frame_new(buffer, bytes_read);
	}

	return pigeon_frame;
}

int pigeon_tunnel_write(PigeonTunnel *pigeon_tunnel, PigeonFrame *pigeon_frame) {
	const char *buffer;
	size_t buffer_size = pigeon_frame_get_buffer(pigeon_frame, &buffer);
	size_t bytes_written = write(pigeon_tunnel->tun_fd, buffer, buffer_size);

	if (bytes_written < 0) {
		perror("Error writing to tunnel device");
	}

	return bytes_written;
}
