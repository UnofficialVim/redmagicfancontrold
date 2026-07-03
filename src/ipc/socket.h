#ifndef SOCKET_H
#define SOCKET_H

typedef struct Runtime Runtime;
#include <stdbool.h>

typedef struct Socket {
    bool enabled;
    int server_fd;
    int client_fd;
    int addrlen;
} Socket;

void socket_init(Runtime *rt);
void socket_cleanup(Runtime *rt);

#endif // SOCKET_H