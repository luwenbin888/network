#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "log.h"
#include "util.h"

#define SERVER_PORT 10001
#define MAXLINE 1024

int main(int argc, char** argv) {
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1; 
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        error(1, errno, "bind error");
    }

    if (listen(sockfd, 32) < 0) {
        error(1, errno, "listen error");
    }

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t clientaddr_len;
        printf("Await client connection...\n");
        int connfd = accept(sockfd, (struct sockaddr*)&client_addr, &clientaddr_len);
        if (connfd < 0) {
            printf("accept failure...\n");
            continue;
        }

        char recv[MAXLINE], send[MAXLINE];
        int client_exit = 0;
        while (1) {
            int rc = read(connfd, recv, MAXLINE);
            if (rc < 0) {
                printf("client read error, close current connection...\n");
                break;
            }
            if (rc == 0) {
                printf("client close, close current connection...\n");
                break;
            }

            recv[rc] = '\0';

            
            if (strcmp(recv, "pwd")==0) {
                printf("client call pwd\n");
                char* msg = pwd();
                int len = strlen(msg);
                int wn = write(connfd, msg, len);
                if (wn < 0) {
                    printf("write error...\n");
                }
            }
            else if (strncmp(recv, "cd", 2)==0) {
                printf("client call cd\n");
                char* dir = recv+3;
                if (dir == NULL || strcmp(dir,"")==0) {
                    char* msg = "invalid path";
                    write(connfd, msg, strlen(msg));
                }
                printf("client request to change dir to %s\n", dir);
                if(cd(dir) == 0) {
                    char* msg = "change dir success";
                    write(connfd, msg, strlen(msg));
                }
                else {
                    char* msg = "change dir failed";
                    write(connfd, msg, strlen(msg));
                }
            }
            else if (strcmp(recv, "ls")==0) {
                printf("client call ls\n");
                char* msg = ls();
                int len = strlen(msg);
                int wn = write(connfd, msg, len);
                if (wn < 0) {
                    printf("write error...\n");
                }
            }
            else if (strcmp(recv, "quit")==0) {
                printf("client quit, close client connection...\n");
                char* msg = "received, will close";
                int len = strlen(msg);
                int wn = write(connfd, msg, len);
                if (wn < 0) {
                    printf("write error...\n");
                }
                break;
            }
            else {
                char* error_msg = "invalid command";
                int wn = write(connfd, error_msg, strlen(error_msg));
                if (wn < 0) {
                    printf("Error write to client...\n");
                }
            }
        }
        close(connfd);
    }
}