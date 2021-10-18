#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAXLINE 1024

int main(int argc, char** argv) {
    if (argc != 2) { 
        printf("usage: udpclient \n");
        exit(1);
    }

    const short int port = 10001;

    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &serveraddr.sin_addr);

    socklen_t server_len = sizeof(serveraddr); 
    struct sockaddr *reply_addr; 
    reply_addr = malloc(server_len); 
    char send_line[MAXLINE], recv_line[MAXLINE + 1]; 
    socklen_t len; 
    int n;

    while (fgets(send_line, MAXLINE, stdin) != NULL) {
        int i = strlen(send_line); 
        if (send_line[i - 1] == '\n') { 
            send_line[i - 1] = 0; 
        } 
        printf("now sending %s\n", send_line); 
        size_t rt = sendto(sockfd, send_line, strlen(send_line), 0, (struct sockaddr *) &serveraddr, server_len); 
        if (rt < 0) { 
            fprintf(stderr, "send failed "); 
        } 
        printf("send bytes: %zu \n", rt); 
        len = 0; 
        n = recvfrom(sockfd, recv_line, MAXLINE, 0, reply_addr, &len); 
        if (n < 0) {
            printf("recvfrom failed");
            exit(1);
        }
        recv_line[n] = 0; 
        fputs(recv_line, stdout); 
        fputs("\n", stdout);
    }
}