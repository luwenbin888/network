#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/un.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define UNIX_SOCK_FILE "/var/lib/test.sock"
#define MAXLINE 1024


int main(int argc, char **argv) {
    // creates an unix tcp socket
    int sockfd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("socket create error\n");
        exit(1);
    }

    unlink(UNIX_SOCK_FILE);

    int connfd;
    socklen_t clilen;

    struct sockaddr_un serveraddr, clientaddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sun_family = AF_LOCAL;
    strcpy(serveraddr.sun_path, UNIX_SOCK_FILE);

    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        printf("bind error\n");
        exit(1);
    }

    if (listen(sockfd, 16) < 0) {
        printf("listen error\n");
        exit(1);
    }

    clilen = sizeof(clientaddr);

    printf("Unix socket created at %s, wait for connection ...", UNIX_SOCK_FILE);

    connfd = accept(sockfd, (struct sockaddr*)&clientaddr, &clilen);
    if (connfd < 0) {
        printf("accept error\n");
        exit(1);
    }

    char buffer[MAXLINE];

    while (1) {
        bzero(buffer, MAXLINE);
        if (read(connfd, buffer, MAXLINE) == 0) {
            printf("client quit\n");
            break;
        }
        printf("received: %s\n", buffer);

        char sendline[MAXLINE];
        sprintf(sendline, "Hi, %s", buffer);

        int sendbuf_len = strlen(sendline);
        if (write(connfd, sendline, sendbuf_len) != sendbuf_len) {
            printf("send error\n");
            exit(1);
        }
    }

    close(connfd);
    close(sockfd);

    exit(0);
}