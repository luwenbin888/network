#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int make_socket(u_int16_t port) {
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        printf("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sock, 1024) < 0) {
        printf("listen");
        exit(EXIT_FAILURE);
    }

    return sock;
}

size_t readn(int fd, void *buffer, size_t size) { 
    char *buffer_pointer = buffer; 
    int length = size; 
    while (length > 0) { 
        int result = read(fd, buffer_pointer, length); 
        if (result < 0) { 
            if (errno == EINTR) continue; /* 考虑非阻塞的情况，这里需要再次调用read */ 
            else return (-1); 
        } 
        else if (result == 0) break; /* EOF(End of File)表示套接字关闭 */ 
        length -= result; 
        buffer_pointer += result; 
    } 
    return (size - length); /* 返回的是实际读取的字节数*/
}

void read_data(int sockfd) {
    ssize_t n; 
    char buf[1024]; 
    int time = 0; 
    for (;;) { 
        fprintf(stdout, "block in read\n");
        // server will try to read 1K data each time 
        if ((n = readn(sockfd, buf, 1024)) == 0) return; 
        time++; 
        fprintf(stdout, "1K read for %d \n", time); 
        usleep(1000); 
    }
}

int main(int argc, char **argv) {
    int sock = make_socket(10001);
    struct sockaddr_in clientaddr;
    socklen_t cliaddrlen = sizeof(clientaddr);

    fprintf(stdout, "Server ready for client connection...\n");

    for (;;) {
        int connfd = accept(sock, (struct sockaddr*)&clientaddr, &cliaddrlen);
        read_data(connfd);
        close(connfd);
        fprintf(stdout, "Server receive client completed... wait for next client connection\n");
    }
}