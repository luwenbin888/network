#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define MAXLINE 1024

static int count;

static void recvfrom_int(int signo) {
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}

int main(int argc, char **argv) {
    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    const short port = 10001;
    struct sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        printf("bind error \n");
        exit(1);
    }

    socklen_t clientaddrlen;
    char message[MAXLINE];
    count = 0;

    signal(SIGINT, recvfrom_int);

    struct sockaddr_in clientaddr;
    clientaddrlen = sizeof(clientaddr);
    for (;;) {
        int n = recvfrom(sockfd, message, MAXLINE, 0, (struct sockaddr*)&clientaddr, &clientaddrlen);
        message[n] = '\0';
        printf("received %d bytes: %s\n", n, message);

        char sendline[MAXLINE];
        sprintf(sendline, "Hi, %s", message);

        sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr*)&clientaddr, clientaddrlen);

        count++;
    }
}