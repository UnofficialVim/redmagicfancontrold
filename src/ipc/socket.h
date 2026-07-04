#ifndef SOCKET_H
#define SOCKET_H
#include <stdbool.h>

struct Runtime;

typedef struct Socket {
    bool enabled;
    int server_fd;
    int client_fd;
    int addrlen;
} Socket;

void socket_init(struct Runtime *rt);
void socket_cleanup(struct Runtime *rt);

#endif // SOCKET_H