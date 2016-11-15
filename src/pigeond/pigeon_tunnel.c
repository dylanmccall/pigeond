#include "pigeon_tunnel.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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

	memset(&ifr, 0, sizeof(ifr));

	if (!error) {
		tun_fd = open(CLONE_DEV, O_RDWR);
		if (tun_fd < 0) {
			perror("Error opening " CLONE_DEV);
			error = true;
		}
	}

	if (!error) {
		int result;
		ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
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

int pigeon_tunnel_read(PigeonTunnel *pigeon_tunnel, char *out_buffer, size_t buffer_size) {
	int result = read(pigeon_tunnel->tun_fd, out_buffer, buffer_size);
	return result;
}

int pigeon_tunnel_write(PigeonTunnel *pigeon_tunnel, char *value, size_t size) {
	return true;
}
