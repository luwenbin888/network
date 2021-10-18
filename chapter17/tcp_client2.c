#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "log.h"

#define SERVER_PORT 10001

static void sig_pipe(int signo) {
    printf("\nreceived sig_pipe\n");
    exit(0);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        error(1, 0, "usage: reliable_client02 <IPaddress>");
    }

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    socklen_t serveraddr_len = sizeof(server_addr);
    if (connect(sockfd, (struct sockaddr*)&server_addr, serveraddr_len) < 0) {
        error(1, errno, "connection failure");
    }

    //signal(SIGPIPE, SIG_IGN);
    signal(SIGPIPE, sig_pipe);

    char *msg = "network programming";
    ssize_t n_written;

    int count = 10000000;
    while (count > 0) {
        n_written = send(sockfd, msg, strlen(msg), 0);
        fprintf(stdout, "send into buffer %ld \n", n_written);
        if (n_written <= 0) {
            error(1, errno, "send error");
            return -1;
        }
        count--;
        
        /**
         client 在不断的发送数据，server 则每次接受数据之后都 sleep 一会，也就导致接收速度小于发送速度，进而导致 server 终止的时候接收缓冲区还有数据没有被读取，
         server 终止触发 close 调用，close 调用时如果接收缓冲区有尚未被应用程序读取的数据时，不发 FIN 包，直接发 RST 包。
         client 每次发送数据之后 sleep 2秒，再试就会出SIGPIPE 了。
        */
        //sleep(2);
    }
    return 0;
}