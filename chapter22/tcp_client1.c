#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/poll.h>
#include "log.h"

#define SERV_PORT 10001
#define INIT_SIZE 128
#define MAXLINE 1024

int main(int argc, char** argv) {
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    struct linger ling;
    ling.l_onoff = 1; 
    ling.l_linger = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
    close(sockfd);
}