#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <asm-generic/ioctls.h>

#include <time.h>

#include "SocketHandler.h"

int SocketHandler::connectTo(const std::string address, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Create client socket");
    }

    struct sockaddr_in server_address;
    struct hostent *server = gethostbyname(address.c_str());

    bzero(
            (char*) &server_address,
            sizeof (server_address));

    bcopy(
            (char*) server->h_addr,
            (char*) &server_address.sin_addr.s_addr,
            server->h_length);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    // connect
    int ret = connect(sock, (struct sockaddr*) &server_address, sizeof (server_address));

    if (ret < 0) {
        perror("Error connecting to host");
    }
    return sock;
}

int SocketHandler::startServer(int port) {
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("ERROR opening server socket");
        return server_socket;
    }

    struct sockaddr_in server_address;
    bzero((char*) &server_address, sizeof (server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    // start server
    int ret = bind(server_socket, (struct sockaddr*) &server_address, sizeof (server_address));
    if (ret < 0) {
        perror("Error opening server socket");
        return ret;
    }
    listen(server_socket, 5);

    return server_socket;
}

int SocketHandler::accept(int server_socket, long usec) {
    struct sockaddr_in client_address;
    socklen_t len = sizeof (client_address);
    
    if (waitForData({server_socket}, usec) != 0) return -1;
    
    int client_socket = ::accept(server_socket, (struct sockaddr*) &client_address, &len);
    
    if (client_socket < 0) {
        perror("SocketHandler::accept");
        return(client_socket);
    }
    return client_socket;
};

int SocketHandler::closeSocket(int socket) {
    return close(socket);
}

int SocketHandler::waitForData(const std::vector<int> &sockets, int usec) {
    
    struct timeval tv;
    fd_set readfds;
    int max_fd(0);

    tv.tv_sec = 0;
    tv.tv_usec = usec < 0 ? 0 : usec;

    FD_ZERO(&readfds);

    for (auto it : sockets) {
        if (it < 0) continue;
        FD_SET(it, &readfds);
        if (it > max_fd) max_fd = it;
    }

    int ret = select(max_fd+1, &readfds, NULL, NULL, &tv);

    if (ret == 0) return -1;
    if (ret < 0) return -2;
    
    for (u_int i = 0; i < sockets.size(); i++) {
        if (FD_ISSET(sockets[i], &readfds)) return i;
    }
    
    std::cerr << "Select returned in undefined state: " << std::endl;
    
    return -2;
}
