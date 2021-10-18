#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PORT 10001

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: simpletcpclient <server_ip>\n");
        exit(EXIT_FAILURE);
    }

    // create tcp socket
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket error\n");
        exit(EXIT_FAILURE);
    }

    // create server address
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    // connect
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect error\n");
        exit(EXIT_FAILURE);
    }

    // send message to tcp server
    char* message = "Hello World!";
    ssize_t n = send(sockfd, message, strlen(message), 0);
    printf("message %s sent to server, ack %d bytes sent\n", message, n);

    printf("client terminating...\n");
    exit(EXIT_SUCCESS);
}