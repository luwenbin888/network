#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "log.h"

#define SERVER_PORT 10001
#define MAXLINE 1024

/**
 * This client uses select for IO multiplexing, it monitors STDIN and socket read
 **/
int main(int argc, char** argv) {
    if (argc != 2) { 
        error(1, 0, "usage: select01 "); 
    }

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    bzero(&server_addr, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        error(1, errno, "connect error");
    }

    char recv_line[MAXLINE], send_line[MAXLINE];
    int n;

    // this readmask is used in loop
    fd_set readmask;
    // this readmask is readonly
    fd_set allreads;
    FD_ZERO(&allreads);
    // listen stdin
    FD_SET(0, &allreads);
    // listen sockfd
    FD_SET(sockfd, &allreads);

    for (;;) {
        // reset readmask
        readmask = allreads;
        // block and check which IO event is ready
        int rc = select(sockfd+1, &readmask, NULL, NULL, NULL);
        if (rc < 0) {
            error(1, errno, "select error");
        }

        // if socket has any data to read
        if (FD_ISSET(sockfd, &readmask)) {
            n = read(sockfd, recv_line, MAXLINE);
            if (n < 0) {
                error(1, errno, "read error");
            }
            if (n == 0) {
                printf("Server terminated...\n");
                exit(EXIT_SUCCESS);
            }
            recv_line[n] = '\0';
            fputs(recv_line, stdout);
            fputs("\n", stdout);
        }

        // if stdin has any input
        if (FD_ISSET(STDIN_FILENO, &readmask)) {
            if (fgets(send_line, MAXLINE, stdin) != NULL) {
                int i = strlen(send_line);
                if (send_line[i-1] == '\n') {
                    send_line[i-1] = '\0';
                }

                //printf("now sending %s\n", send_line);
                ssize_t wn = write(sockfd, send_line, strlen(send_line));
                if (wn < 0) {
                    error(1, errno, "write failed");
                }
                //printf("send bytes: %zu \n", wn);
                if (strcmp(send_line, "quit")==0) {
                    shutdown(sockfd, SHUT_WR);
                    printf("Client half closed, no more write to server...\n");
                    FD_CLR(0, &allreads);
                }
            }
        }
    }
}