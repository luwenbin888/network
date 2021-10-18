#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PORT 10001
#define BUFSIZE 1024
#define LISTENQLEN 32 

int main(int argc, char ** argv) {
    // create tcp socket
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);

    // create server address
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind error\n");
        exit(EXIT_FAILURE);
    }

    // listen
    if (listen(sockfd, LISTENQLEN) < 0) {
        perror("listen error\n");
        exit(EXIT_FAILURE);
    }

    printf("Server started, listening for client...\n");

    // accept
    struct sockaddr_in client_addr;
    socklen_t cliaddr_len = sizeof(client_addr);
    int connfd = accept(sockfd, (struct sockaddr*)&client_addr, &cliaddr_len);
    if (connfd < 0) {
        perror("accept error\n");
        exit(EXIT_FAILURE);
    }

    // read from client
    char buf[BUFSIZE];
    ssize_t n = read(connfd, buf, BUFSIZE);
    if (n < 0) {
        perror("read error\n");
        exit(EXIT_FAILURE);
    }
    if (n == 0) {
        printf("client quit\n");
        exit(EXIT_SUCCESS);
    }
    buf[n] = '\0';
    printf("Client message: %s\n", buf);

    printf("server is terminating...\n");
    exit(EXIT_SUCCESS);
}