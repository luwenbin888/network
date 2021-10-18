#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>

#define MESSAGE_SIZE 1024000

void send_data(int sockfd) {
    char *query;
    query = malloc(MESSAGE_SIZE+1);
    for (int i = 0; i < MESSAGE_SIZE; i++) {
        query[i] = 'a';
    }
    query[MESSAGE_SIZE] = '\0';

    const char* cp;
    cp = query;

    size_t remaining = strlen(query);
    while (remaining) {
        // send will block here until all data has written to kernel buffer
        int n_written = send(sockfd, cp, remaining, 0);
        fprintf(stdout, "send into buffer %ld \n", n_written); 
        if (n_written <= 0) { 
            printf("send failure"); 
            return; 
        } 
        remaining -= n_written; 
        cp += n_written;
    }
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage client <IPAddress>");
        exit(EXIT_FAILURE);
    }

    int sock = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(10001);
    inet_pton(AF_INET, argv[1], &serveraddr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        printf("connect failed");
        exit(EXIT_FAILURE);
    }

    send_data(sock);
    exit(0);
}