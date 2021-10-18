#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "log.h"

#define SERVER_PORT 10001

//客户端程序
int main(int argc, char **argv) {
    if (argc != 2) {
        error(1, 0, "usage: reliable_client01 <IPaddress>");
    }

    /*
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = SERVER_PORT;
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        error(1, errno, "connect error");
    }
    */

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
    
    char buf[128];
    int len;
    int rc;

    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        len = strlen(buf);
        rc = send(sockfd, buf, len, 0);
        if (rc < 0)
            error(1, errno, "write failed");
        rc = read(sockfd, buf, sizeof(buf));
        if (rc < 0)
            error(1, errno, "read failed");
        else if (rc == 0)
            error(1, 0, "peer connection closed\n");
        else
            fputs(buf, stdout);
    }
    exit(0);
}