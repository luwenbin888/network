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
    int sockfd; 
    struct sockaddr_un servaddr;

    sockfd = socket(AF_LOCAL, SOCK_STREAM, 0); 
    if (sockfd < 0) { 
        printf("socket create failed\n");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr)); 
    servaddr.sun_family = AF_LOCAL; 
    strcpy(servaddr.sun_path, UNIX_SOCK_FILE);

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) { 
        printf("connect failed\n");
        exit(1);
    }

    char send_line[MAXLINE]; 
    bzero(send_line, MAXLINE); 
    char recv_line[MAXLINE];

    while (fgets(send_line, MAXLINE, stdin) != NULL) { 
        int nbytes = sizeof(send_line); 
        if (write(sockfd, send_line, nbytes) != nbytes) {
            printf("write error\n");
            exit(1);
        } 
        if (read(sockfd, recv_line, MAXLINE) == 0) {
            printf("server terminated prematurely\n");
            exit(1);
        }
        fputs(recv_line, stdout); 
    }

    exit(0);
}