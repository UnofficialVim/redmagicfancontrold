#include "logging.h"
#include "runtime/runtime.h"
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <limits.h>

static bool get_exe_dir(char *out, size_t out_size) {
  char exe_path[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
  if (len < 0) {
    perror("get_exe_dir: readlink");
    return false;
  }
  exe_path[len] = '\0';

  char *slash = strrchr(exe_path, '/');
  if (!slash) return false;
  *slash = '\0';

  if (strlen(exe_path) >= out_size) return false;
  memcpy(out, exe_path, strlen(exe_path) + 1);
  return true;
}

void socket_init(Runtime *rt) {
  Socket *sock = &rt->socket;

  char exe_dir[PATH_MAX];
  if (!get_exe_dir(exe_dir, sizeof(exe_dir))) {
    logger_write(rt, 0, "socket_init: failed to resolve executable directory");
    sock->enabled = false;
    return;
  }

  int n = snprintf(sock->socket_path, sizeof(sock->socket_path), "%s/rmfc_socket", exe_dir);
  if (n < 0 || (size_t)n >= sizeof(sock->socket_path)) {
    logger_write(rt, 0, "socket_init: socket path too long for buffer");
    sock->enabled = false;
    return;
  }

  sock->server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock->server_fd < 0) {
    perror("socket_init: socket()");
    sock->enabled = false;
    return;
  }

  memset(&sock->address, 0, sizeof(sock->address));
  sock->address.sun_family = AF_UNIX;

  if (strlen(sock->socket_path) >= sizeof(sock->address.sun_path)) {
    logger_write(rt, 0, "socket_init: path exceeds sun_path limit (108 bytes)");
    close(sock->server_fd);
    sock->enabled = false;
    return;
  }
  strncpy(sock->address.sun_path, sock->socket_path, sizeof(sock->address.sun_path) - 1);

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
  int flags = fcntl(sock->server_fd, F_GETFL, 0);
  fcntl(sock->server_fd, F_SETFL, flags | O_NONBLOCK);
  logger_write(rt, 2, "Initialized Socket");
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
      logger_write(rt, 1, "accept() failed");
    return;
  }

  sock->client_fd = fd;

  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);

  logger_write(rt, 2, "Client connected");
}
// handle the send function to auto alocate the buffer and its byte size
void socket_send(Runtime *rt, const char *message) {
  char *tmp_message = strdup(message);
  size_t message_length = strlen(tmp_message);
  char *message_with_newline =
      malloc(message_length +
             2); // allocate space for the message, newline, and null terminator
  if (message_with_newline == NULL) {
    logger_write(rt, 1, "Failed to allocate memory for message");
    free(tmp_message);
    return;
  }
  strcpy(message_with_newline, tmp_message);
  message_with_newline[message_length] = '\n';
  message_with_newline[message_length + 1] = '\0';
  Socket *sock = &rt->socket;
  message_length = strlen(message_with_newline);
  if (send(sock->client_fd, message_with_newline, message_length, 0) == -1) {
    logger_write(rt, 1, "Failed to send message: %s error: %s", message,
                 strerror(errno));
  } else {
    logger_write(rt, 2, "Message sent successfully: %s", message);
  }
}

void socket_receive(Runtime *rt) {
  Socket *sock = &rt->socket;

  char buffer[256];

  ssize_t n = recv(sock->client_fd, buffer, sizeof(buffer) - 1, 0);

  if (n < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
      return; // no data yet, not a disconnect

    close(sock->client_fd);
    sock->client_fd = -1;
    logger_write(rt, 2, "Client disconnected (error)");
    return;
  }
  if (n == 0) {
    close(sock->client_fd);
    sock->client_fd = -1;
    logger_write(rt, 2, "Client disconnected");
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
