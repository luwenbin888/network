#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <stdio.h>

int make_socket(u_int16_t port) {
    // create socket
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // create server addr and bind
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr*) &serveraddr, sizeof(serveraddr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    return sock;
}

int main(int argc, char **argv) {
    int sock = make_socket(10001);
    printf("created server socket %d\n", sock);
    exit(0);
}