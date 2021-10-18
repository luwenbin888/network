#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "log.h"

#define SERVER_PORT 10001
#define LISTENQ 32
#define MAXLINE 1024

int main(int argc, char** argv) {
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1; 
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        error(1, errno, "bind failure");
    }

    if (listen(sockfd, LISTENQ) < 0) {
        error(1, errno, "listen failure");
    }

    printf("Await client connection...\n");

    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    socklen_t clientaddr_len;
    int connfd = accept(sockfd, (struct sockaddr*)&client_addr, &clientaddr_len);
    if (connfd < 0) {
        error(1, errno, "accept failure");
    }

    char buffer[MAXLINE];

    while (1) {
        bzero(buffer, MAXLINE);
        ssize_t rc = read(connfd, buffer, MAXLINE);
        if (rc < 0) {
            error(1, errno, "read error");
        }
        if (rc == 0) {
            printf("client closed...\n");
            exit(EXIT_SUCCESS);
        }
        buffer[rc] = '\0';
        
        printf("Server received ");
        printf(buffer);
        printf(" from client\n");

        ssize_t wn = write(connfd, buffer, rc);
        if (wn != rc) {
            error(1, 0, "write error");
        }
    }
}