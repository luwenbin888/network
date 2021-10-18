#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "protocol.h"

#define SERVER_PORT 10001

int main(int argc, char** argv) {
    if (argc != 2) { 
        error(1, 0, "usage: tcpclient "); 
    }

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    socklen_t serveraddr_len = sizeof(server_addr);
    if (connect(sockfd, (struct sockaddr*)&server_addr, serveraddr_len) < 0) {
        error(1, errno, "connection failure");
    }

    struct message msg;

    int n;
    while (fgets(msg.buf, sizeof(msg.buf), stdin) != NULL) {
        n = strlen(msg.buf);
        msg.message_type = htonl(1);
        msg.message_length = htonl(n);
        if (send(sockfd, (char*)&msg, sizeof(msg.message_type) + sizeof(msg.message_type) + n, 0) < 0) {
            error(1, errno, "send failure");
        }
    }

    exit(EXIT_SUCCESS);
}