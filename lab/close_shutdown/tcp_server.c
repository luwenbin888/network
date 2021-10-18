#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define SERVER_PORT 10001

static void sig_int(int signo) { 
    printf("\nserver quit...\n"); 
    exit(EXIT_SUCCESS);
}

static void sig_pipe(int signo) {
    printf("Received sig_pipe, server quit...\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("socket error...\n");
    }

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("bind failed...\n");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 32) < 0) {
        printf("listen failure...\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sig_int); 
    signal(SIGPIPE, sig_pipe);

    printf("Await client connection...\n");

    struct sockaddr_in client_addr;
    socklen_t clientaddr_len;
    int connfd = accept(sockfd, (struct sockaddr*)&client_addr, &clientaddr_len);
    if (connfd < 0) {
        printf("accept failure...\n");
        exit(EXIT_FAILURE);
    }

    int buf_len = 256;
    char buffer[buf_len];
    
    while (1) {
        // if no data in input buffer, read will block
        // if client send FIN, read will return 0
        int n = read(connfd, &buffer, buf_len);
        if (n < 0) {
            printf("read error, quit...\n");
            exit(EXIT_FAILURE);
        }
        if (n == 0) {
            printf("connection closed by client...\n");
            exit(EXIT_SUCCESS);
        }
        buffer[n] = '\0';
        printf("Received %s from client\n", buffer);
        char send_line[buf_len]; 
        sprintf(send_line, "Hi, %s", buffer);
        int send_msg_len = strlen(send_line);
        // if socket is closed by client, first write will return RST, the next will trigger SIG_PIPE
        int wn = write(connfd, &send_line, send_msg_len);
        if (wn != send_msg_len) {
            printf("Write error...\n");
            exit(EXIT_FAILURE);
        }
    }
}