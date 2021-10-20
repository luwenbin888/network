#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define SERVER_PORT 10001
#define MAXLINE 1024

int main(int argc, char** argv) {
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);

    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    listen(sockfd, 32);

    int connfd = accept(sockfd, malloc(sizeof(struct sockaddr_in)), malloc(sizeof(socklen_t)));
    if (connfd < 0) {
        printf("accept returns %d, with errno %d, errmsg %s\n", connfd, errno, strerror(errno));
    }

    if (errno == EAGAIN) {
        while (1) {
            printf("Try to accept...\n");
            connfd = accept(sockfd, malloc(sizeof(struct sockaddr_in)), malloc(sizeof(socklen_t)));
            if (connfd > 0) break;
            sleep(1);
        }
    }
    printf("Accepted, new socket fd %d\n", connfd);

    // after accept, the accepted socket is blocking
    fcntl(connfd, F_SETFL, O_NONBLOCK);

    char buf[MAXLINE];
    int rc = recv(connfd, buf, MAXLINE, 0);
    if (rc < 0) {
        printf("read returns %d, with errno %d, errmsg %s\n", rc, errno, strerror(errno));
    }
    if (rc == 0) {
        printf("client closed...\n");
        exit(0);
    }

    if (errno == EAGAIN) {
        while (1) {
            printf("Try to read...\n");
            rc = recv(connfd, buf, MAXLINE, 0);
            if (rc == 0) {
                printf("client closed...\n");
                exit(0);
            }
            if (rc > 0) {
                buf[rc-2] = '\0';
                break;
            }
            sleep(1);
        }
    }

    printf("Got msg from client: %s\n", buf);

    // if write buffer is available, it will write all data into buffer
    int wn = write(connfd, buf, rc-3);
    printf("write returns %d, with errno %d, errmsg %s\n", wn, errno, strerror(errno));

    sleep(2);

    close(connfd);
    close(sockfd);

    exit(0);
}