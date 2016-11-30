#include "pigeon_tunnel.h"

#include "long_thread.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#define CLONE_DEV "/dev/net/tun"

struct _PigeonTunnel {
	PigeonFramePipeHandle frame_pipe_ref_tx;
	int tun_fd;
	const char *dev_name_template;
	char dev_name[IFNAMSIZ];
	LongThread *write_thread;
	LongThread *read_thread;
};

LongThreadResult _pigeon_tunnel_write_thread_loop(LongThread *long_thread, void *data);
LongThreadResult _pigeon_tunnel_read_thread_loop(LongThread *long_thread, void *data);

PigeonTunnel *pigeon_tunnel_new(const char *dev_name_template, PigeonFramePipeHandle frame_pipe_ref_tx) {
	PigeonTunnel *pigeon_tunnel = malloc(sizeof(PigeonTunnel));
	memset(pigeon_tunnel, 0, sizeof(*pigeon_tunnel));
	pigeon_tunnel->dev_name_template = dev_name_template;
	pigeon_tunnel->frame_pipe_ref_tx = frame_pipe_ref_tx;
	return pigeon_tunnel;
}

void pigeon_tunnel_free(PigeonTunnel *pigeon_tunnel) {
	if (pigeon_tunnel->write_thread) {
		long_thread_free(pigeon_tunnel->write_thread);
	}

	if (pigeon_tunnel->read_thread) {
		long_thread_free(pigeon_tunnel->read_thread);
	}

	if (pigeon_tunnel->tun_fd) {
		close(pigeon_tunnel->tun_fd);
	}

	free(pigeon_tunnel);
}

bool pigeon_tunnel_init(PigeonTunnel *pigeon_tunnel) {
	bool error = false;

	if (!error) {
		pigeon_tunnel->tun_fd = open(CLONE_DEV, O_RDWR);
		if (pigeon_tunnel->tun_fd < 0) {
			perror("Error opening " CLONE_DEV);
			error = true;
		}
	}

	if (!error) {
		struct ifreq ifr = {0};
		strncpy(ifr.ifr_name, pigeon_tunnel->dev_name_template, IFNAMSIZ);
		ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
		if (ioctl(pigeon_tunnel->tun_fd, TUNSETIFF, (void *) &ifr) < 0) {
			error = true;
		} else {
			strncpy(pigeon_tunnel->dev_name, ifr.ifr_name, IFNAMSIZ);
		}
	}

	if (!error) {
		pigeon_tunnel->write_thread = long_thread_new((LongThreadOptions){
			.name="tunnel-write",
			.loop_fn=_pigeon_tunnel_write_thread_loop,
			.data=pigeon_tunnel
		});
		pigeon_tunnel->read_thread = long_thread_new((LongThreadOptions){
			.name="tunnel-read",
			.loop_fn=_pigeon_tunnel_read_thread_loop,
			.data=pigeon_tunnel
		});
		error = pigeon_tunnel->write_thread == NULL || pigeon_tunnel->read_thread == NULL;
	}

	return !error;
}

bool pigeon_tunnel_start(PigeonTunnel *pigeon_tunnel) {
	bool error = false;

	if (!error) {
		error = pigeon_tunnel->write_thread == NULL || pigeon_tunnel->read_thread == NULL;
	}

	if (!error) {
		bool write_started = long_thread_start(pigeon_tunnel->write_thread);
		bool read_started = long_thread_start(pigeon_tunnel->read_thread);
		error = !write_started || !read_started;
	}

	return !error;
}

bool pigeon_tunnel_wait(PigeonTunnel *pigeon_tunnel) {
	bool write_error, read_error;

	if (pigeon_tunnel->write_thread) {
		write_error = long_thread_wait(pigeon_tunnel->write_thread);
	} else {
		write_error = false;
	}

	if (pigeon_tunnel->read_thread) {
		read_error = long_thread_wait(pigeon_tunnel->read_thread);
	} else {
		read_error = false;
	}

	return !write_error && !read_error;
}

int pigeon_tunnel_join(PigeonTunnel *pigeon_tunnel) {
	int write_result, read_result;

	if (pigeon_tunnel->write_thread) {
		write_result = long_thread_join(pigeon_tunnel->write_thread);
	} else {
		write_result = 0;
	}

	if (pigeon_tunnel->read_thread) {
		read_result = long_thread_join(pigeon_tunnel->read_thread);
	} else {
		read_result = 0;
	}

	return (write_result == 0 && read_result == 0) ? 0 : -1;
}

bool pigeon_tunnel_stop(PigeonTunnel *pigeon_tunnel) {
	bool write_error, read_error;

	if (pigeon_tunnel->write_thread) {
		write_error = long_thread_stop(pigeon_tunnel->write_thread);
	} else {
		write_error = false;
	}

	if (pigeon_tunnel->read_thread) {
		read_error = long_thread_stop(pigeon_tunnel->read_thread);
	} else {
		read_error = false;
	}

	return !write_error && !read_error;
}

bool pigeon_tunnel_is_running(PigeonTunnel *pigeon_tunnel) {
	bool write_running, read_running;

	if (pigeon_tunnel->write_thread) {
		write_running = long_thread_is_running(pigeon_tunnel->write_thread);
	} else {
		write_running = false;
	}

	if (pigeon_tunnel->read_thread) {
		read_running = long_thread_is_running(pigeon_tunnel->read_thread);
	} else {
		read_running = false;
	}

	return write_running || read_running;
}

const char *pigeon_tunnel_get_dev_name(PigeonTunnel *pigeon_tunnel) {
	return pigeon_tunnel->dev_name;
}

bool pigeon_tunnel_set_mtu(PigeonTunnel *pigeon_tunnel, int mtu) {
	bool error = false;
	int socket_fd;

	if (!error) {
		socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (socket_fd < 0) {
			error = true;
		}
	}

	if (!error) {
		struct ifreq ifr = {0};
		strncpy(ifr.ifr_name, pigeon_tunnel->dev_name, IFNAMSIZ);
		ifr.ifr_addr.sa_family = AF_INET;
		ifr.ifr_mtu = mtu;
		if (ioctl(socket_fd, SIOCSIFMTU, (void *) &ifr) < 0) {
			error = true;
		}
	}

	return !error;
}

int pigeon_tunnel_get_mtu(PigeonTunnel *pigeon_tunnel) {
	bool error = false;
	int socket_fd;
	struct ifreq ifr = {0};

	if (!error) {
		socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (socket_fd < 0) {
			error = true;
		}
	}

	if (!error) {
		strncpy(ifr.ifr_name, pigeon_tunnel->dev_name, IFNAMSIZ);
		ifr.ifr_addr.sa_family = AF_INET;
		if (ioctl(socket_fd, SIOCGIFMTU, (void *) &ifr) < 0) {
			error = true;
		}
	}

	if (!error) {
		return ifr.ifr_mtu;
	} else {
		return -1;
	}
}

bool pigeon_tunnel_frames_wait(PigeonTunnel *pigeon_tunnel) {
	return pigeon_frame_pipe_wait(pigeon_tunnel->frame_pipe_ref_tx);
}

bool pigeon_tunnel_frames_has_next(PigeonTunnel *pigeon_tunnel) {
	return pigeon_frame_pipe_has_next(pigeon_tunnel->frame_pipe_ref_tx);
}

PigeonFrame *pigeon_tunnel_frames_pop(PigeonTunnel *pigeon_tunnel) {
	return pigeon_frame_pipe_pop(pigeon_tunnel->frame_pipe_ref_tx);
}

bool pigeon_tunnel_frames_push(PigeonTunnel *pigeon_tunnel, PigeonFrame *pigeon_frame) {
	return pigeon_frame_pipe_push(pigeon_tunnel->frame_pipe_ref_tx, pigeon_frame);
}

LongThreadResult _pigeon_tunnel_write_thread_loop(LongThread *long_thread, void *data) {
	PigeonTunnel *pigeon_tunnel = (PigeonTunnel *)data;

	PigeonFrame *pigeon_frame = pigeon_tunnel_frames_pop(pigeon_tunnel);

	if (pigeon_frame) {
		printf("tunnel-write: Got next frame\n");

		pigeon_frame_print_header(pigeon_frame);
		pigeon_frame_print_data(pigeon_frame);

		const unsigned char *buffer;
		size_t buffer_size = pigeon_frame_get_buffer(pigeon_frame, &buffer);
		size_t bytes_written = write(pigeon_tunnel->tun_fd, buffer, buffer_size);

		if (bytes_written < 0) {
			perror("Error writing to tunnel device");
		}

		pigeon_frame_free(pigeon_frame);
	}

	return LONG_THREAD_CONTINUE;
}

LongThreadResult _pigeon_tunnel_read_thread_loop(LongThread *long_thread, void *data) {
	PigeonTunnel *pigeon_tunnel = (PigeonTunnel *)data;

	PigeonFrame *pigeon_frame;
	// FIXME: It would be better to use the MTU for the buffer size here, but
	// pigeon_tunnel_get_mtu is very inefficient.
	unsigned char buffer[ETHER_MAX_LEN] = {0};
	ssize_t bytes_read = read(pigeon_tunnel->tun_fd, &buffer, sizeof(buffer));

	if (bytes_read < 0) {
		perror("Error reading from tunnel device");
		pigeon_frame = NULL;
	} else if (bytes_read > ETHER_MAX_LEN) {
		fprintf(stderr, "Dropping oversized frame");
		fprintf(stderr, "Frame is %lu bytes. Expected <= %d bytes.\n", (unsigned long)bytes_read, ETHER_MAX_LEN);
		pigeon_frame = NULL;
	} else {
		// We don't worry about undersized packets. Those will be padded automatically.
		pigeon_frame = pigeon_frame_new(buffer, bytes_read);
	}

	if (pigeon_frame != NULL) {
		printf("tunnel-read: Sending next frame\n");
		pigeon_tunnel_frames_push(pigeon_tunnel, pigeon_frame);
	}

	return LONG_THREAD_CONTINUE;
}
