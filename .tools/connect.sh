#!/bin/sh
#
# Connects to a unix domain socket
# usage: ./connect <PATH>
SOCKET_PATH="$1"

if [ -n "$SOCKET_PATH" ]; then
    echo "Connecting to socket at: $SOCKET_PATH"
    socat - UNIX-CONNECT:"$SOCKET_PATH"
else
    echo "No path specified, using default socket"
    socat - UNIX-CONNECT:/tmp/rmfc_socket
fi