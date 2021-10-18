#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define SERVER_PORT 10001

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage tcpclient <server_ip>\n");
        exit(EXIT_FAILURE);
    }
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("connect failure...\n");
        exit(EXIT_FAILURE);
    }

    int buf_len = 256;
    char buffer[buf_len];
    char recv_buffer[buf_len];

    while (1) {
        bzero(buffer, buf_len);
        fgets(buffer, buf_len, stdin);
        int len = strlen(buffer);
        if (buffer[len-1] == '\n') buffer[len-1] = '\0';
        int n = write(sockfd, buffer, len-1);
        printf("Write %s to server\n", buffer);

        bzero(&recv_buffer, buf_len);
        int rn = read(sockfd, recv_buffer, buf_len);
        if (rn < 0) {
            printf("Read error...\n");
            exit(EXIT_FAILURE);
        }
        if (rn == 0) {
            printf("Server terminated...\n");
            exit(EXIT_SUCCESS);
        }
        recv_buffer[rn] = '\0';
        printf("Received %s from server...\n", recv_buffer);
    }
}