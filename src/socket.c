#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include "runtime/runtime.h"
#include "logging.h"
#include <string.h>

void socket_init(Runtime *rt) {
Socket *sock = &rt->socket;
	//stub for socket path
	strcpy(sock->socket_path, "/tmp/rmfc_socket");

    sock->server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock->server_fd < 0) {
        perror("socket_init: socket()");
        sock->enabled = false;
        return;
    }

    memset(&sock->address, 0, sizeof(sock->address));
    sock->address.sun_family = AF_UNIX;
    strncpy(sock->address.sun_path, rt->socket.socket_path,
            sizeof(sock->address.sun_path) - 1);

    unlink(sock->address.sun_path); // clear stale socket file from a previous run

    sock->addrlen = sizeof(sock->address);

    if (bind(sock->server_fd, (struct sockaddr *)&sock->address, sock->addrlen) < 0) {
        perror("socket_init: bind()");
		logger_write(rt, 0, "Failed to bind() socket");
        close(sock->server_fd);
        sock->enabled = false;
        return;
    }

    if (listen(sock->server_fd, 1) < 0) {
        perror("socket_init: listen()");
		logger_write(rt, 0, "Failed start socket listener");
        close(sock->server_fd);
        sock->enabled = false;
        return;
    }

    sock->client_fd = -1;
    sock->enabled = true;

	logger_write(rt, 2, "Initialized Socket");
}

void socket_cleanup(Runtime *rt) {
	Socket *sock = &rt->socket;

    if (sock->client_fd >= 0) close(sock->client_fd);
    if (sock->server_fd >= 0) close(sock->server_fd);
    unlink(sock->address.sun_path);
    sock->enabled = false;
}
