#include "socket.h"
#include "logger.h"
#include "runtime/runtime.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void socket_init(Runtime *rt) {
  Socket *sock = &rt->socket;
  char *running_dir = get_running_dir();

  int n = snprintf(sock->socket_path, sizeof(sock->socket_path),
                  "%s/rmfc_socket", running_dir);
  if (n < 0 || (size_t)n >= sizeof(sock->socket_path)) {
   logger_warn("socket_init: socket path too long for buffer");
   sock->enabled = false;
   return;
  }

  sock->server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock->server_fd < 0) {
    logger_errno(LOGGER_WARN, "socket_init: failed to create socket");
    sock->enabled = false;
    return;
  }

  memset(&sock->address, 0, sizeof(sock->address));
  sock->address.sun_family = AF_UNIX;

  if (strlen(sock->socket_path) >= sizeof(sock->address.sun_path)) {
    logger_warn("socket_init: path exceeds sun_path limit (108 bytes)");
    close(sock->server_fd);
    sock->enabled = false;
    return;
  }
  strncpy(sock->address.sun_path, sock->socket_path,
          sizeof(sock->address.sun_path) - 1);

  unlink(sock->address.sun_path); // clear stale socket file from a previous run
  sock->addrlen = sizeof(sock->address);

  if (bind(sock->server_fd, (struct sockaddr *)&sock->address, sock->addrlen) <
      0) {
    logger_errno(LOGGER_WARN, "Failed to bind socket");
    close(sock->server_fd);
    sock->enabled = false;
    return;
  }
  if (listen(sock->server_fd, 1) < 0) {
    logger_errno(LOGGER_WARN, "Failed to start socket listener");
    close(sock->server_fd);
    sock->enabled = false;
    return;
  }
  sock->client_fd = -1;
  sock->enabled = true;
  int flags = fcntl(sock->server_fd, F_GETFL, 0);
  fcntl(sock->server_fd, F_SETFL, flags | O_NONBLOCK);
  logger_info("Initialized Socket");
}

void socket_cleanup(Runtime *rt) {
  Socket *sock = &rt->socket;

  if (sock->client_fd >= 0)
    close(sock->client_fd);
  if (sock->server_fd >= 0)
    close(sock->server_fd);
  unlink(sock->address.sun_path);
  sock->enabled = false;
}

void socket_accept(Runtime *rt) {
  Socket *sock = &rt->socket;

  int fd = accept(sock->server_fd, NULL, NULL);

  if (fd < 0) {
    if (errno != EAGAIN && errno != EWOULDBLOCK)
      logger_errno(LOGGER_WARN, "Failed to accept client");
    return;
  }

  sock->client_fd = fd;
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);

  logger_info("Client connected");
}
// handle the send function to auto alocate the buffer and its byte size
void socket_send(Runtime *rt, const char *message) {
  char *tmp_message = strdup(message);
  size_t message_length = strlen(tmp_message);
  char *message_with_newline =
      malloc(message_length +
             2); // allocate space for the message, newline, and null terminator
  if (message_with_newline == NULL) {
    logger_warn("Failed to allocate memory for message");
    free(tmp_message);
    return;
  }
  strcpy(message_with_newline, tmp_message);
  message_with_newline[message_length] = '\n';
  message_with_newline[message_length + 1] = '\0';
  Socket *sock = &rt->socket;
  message_length = strlen(message_with_newline);
  if (send(sock->client_fd, message_with_newline, message_length, 0) == -1) {
    logger_errno(LOGGER_WARN, "Failed to send message: %s", message);
  } else {
    logger_errno(LOGGER_INFO, "Message sent successfully: %s", message);
  }
}

void socket_receive(Runtime *rt) {
  Socket *sock = &rt->socket;

  char buffer[256];

  ssize_t n = recv(sock->client_fd, buffer, sizeof(buffer) - 1, 0);

  if (n < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      logger_errno(LOGGER_TRACE, "No data yet");
      return; // no data yet, not a disconnect
    }
  }
  if (n == 0) {
    close(sock->client_fd);
    sock->client_fd = -1;
    logger_info("Client disconnected");
    return;
  }

  buffer[strcspn(buffer, "\n")] = '\0';

  const char *response = "Command received";
  socket_send(rt, response);

  if (strncmp(buffer, "getCpuTemp", 10) == 0) {
    char *message = "42";
    socket_send(rt, message);
  } else if (strncmp(buffer, "getFanRPM", 9) == 0) {
    // Get fan info
    char *message = "1200";
    socket_send(rt, message);
  } else if (strncmp(buffer, "getFanSpeed", 11) == 0) {
    // Get fan speed
    char *message = "3";
    socket_send(rt, message);
  } else {
    char message[256];
    snprintf(message, sizeof(message), "Unknown command: %s", buffer);
    socket_send(rt, message);
  }
}
