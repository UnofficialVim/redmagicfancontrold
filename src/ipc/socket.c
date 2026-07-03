#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../runtime/runtime.h"

void socket_init(Runtime *rt) {
	if (!rt) return;
	rt->socket.enabled = false;
	rt->socket.server_fd = -1;
	rt->socket.client_fd = -1;
	rt->socket.addrlen = 0;
}

void socket_cleanup(Runtime *rt) {
	if (!rt) return;
	if (rt->socket.client_fd >= 0) close(rt->socket.client_fd);
	if (rt->socket.server_fd >= 0) close(rt->socket.server_fd);
	rt->socket.enabled = false;
	rt->socket.client_fd = -1;
	rt->socket.server_fd = -1;
	rt->socket.addrlen = 0;
}
