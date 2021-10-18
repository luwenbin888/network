#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "log.h"

#define SERVER_PORT 10001
#define LISTENQ 32

int main(int argc, char **argv) {
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        error(1, 0, "bind error");
    }

    if (listen(sockfd, LISTENQ) < 0) {
        error(1, 0, "listen error");
    }

    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    socklen_t clientaddr_len = sizeof(client_addr);

    int connfd = accept(sockfd, (struct sockaddr*)&client_addr, &clientaddr_len);
    if (connfd < 0) {
        error(1, 0, "accept error");
    }

    char buf[1024];

    for (;;) {
        int n = read(connfd, buf, 1024);
        if (n < 0) { 
            error(1, errno, "error read"); 
        } 
        else if (n == 0) { 
            error(1, 0, "client closed \n"); 
        }

        sleep(5);

        int write_nc = send(connfd, buf, n, 0); 
        printf("send bytes: %zu \n", write_nc);
        if (write_nc < 0) { 
            error(1, errno, "error write");
        }
    }
}