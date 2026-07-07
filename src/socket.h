#ifndef SOCKET_H
#define SOCKET_H
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/un.h>

struct Runtime;

typedef struct Socket {
    bool enabled;
    int server_fd;
    int client_fd;
    int addrlen;
    struct sockaddr_un address;
    char socket_path[64];
} Socket;

void socket_init(struct Runtime *rt);
void socket_cleanup(struct Runtime *rt);

#endif // SOCKET_H