#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "log.h"

#define SERV_PORT 10001
#define MAX_LINE 1024
#define BLOCK_QUEUE_SIZE 32
#define THREAD_NUMBER 3
#define LISTENQ 32

//定义一个队列
typedef struct {
    int number;  //队列里的描述字最大个数
    int *fd;     //这是一个数组指针
    int front;   //当前队列的头位置
    int rear;    //当前队列的尾位置
    pthread_mutex_t mutex;  //锁
    pthread_cond_t cond;    //条件变量
} block_queue;

typedef struct {
    pthread_t thread_tid;        /* thread ID */
    long thread_count;    /* # connections handled */
} Thread;

Thread *thread_array;

//初始化队列
void block_queue_init(block_queue *blockQueue, int number) {
    blockQueue->number = number;
    blockQueue->fd = calloc(number, sizeof(int));
    blockQueue->front = blockQueue->rear = 0;
    pthread_mutex_init(&blockQueue->mutex, NULL);
    pthread_cond_init(&blockQueue->cond, NULL);
}

//往队列里放置一个描述字fd
void block_queue_push(block_queue *blockQueue, int fd) {
    //一定要先加锁，因为有多个线程需要读写队列
    pthread_mutex_lock(&blockQueue->mutex);
    //将描述字放到队列尾的位置
    blockQueue->fd[blockQueue->rear] = fd;
    //如果已经到最后，重置尾的位置
    if (++blockQueue->rear == blockQueue->number) {
        blockQueue->rear = 0;
    }
    printf("push fd %d\n", fd);
    //通知其他等待读的线程，有新的连接字等待处理
    pthread_cond_signal(&blockQueue->cond);
    //解锁
    pthread_mutex_unlock(&blockQueue->mutex);
}

//从队列里读出描述字进行处理
int block_queue_pop(block_queue *blockQueue) {
    //加锁
    pthread_mutex_lock(&blockQueue->mutex);
    //判断队列里没有新的连接字可以处理，就一直条件等待，直到有新的连接字入队列
    while (blockQueue->front == blockQueue->rear)
        pthread_cond_wait(&blockQueue->cond, &blockQueue->mutex);
    //取出队列头的连接字
    int fd = blockQueue->fd[blockQueue->front];
    //如果已经到最后，重置头的位置
    if (++blockQueue->front == blockQueue->number) {
        blockQueue->front = 0;
    }
    printf("pop fd %d", fd);
    //解锁
    pthread_mutex_unlock(&blockQueue->mutex);
    //返回连接字
    return fd;
}

char rot13_char(char c) {
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}

void loop_echo(int fd) {
    char outbuf[MAX_LINE + 1];
    size_t outbuf_used = 0;
    ssize_t result;
    while (1) {
        char ch;
        result = recv(fd, &ch, 1, 0);

        //断开连接或者出错
        if (result == 0) {
            break;
        } else if (result == -1) {
            error(1, errno, "read error");
            break;
        }

        if (outbuf_used < sizeof(outbuf)) {
            outbuf[outbuf_used++] = rot13_char(ch);
        }

        if (ch == '\n') {
            send(fd, outbuf, outbuf_used, 0);
            outbuf_used = 0;
            continue;
        }
    }
}

void* thread_run(void *arg) {
    pthread_t tid = pthread_self();
    pthread_detach(tid);

    block_queue *blockQueue = (block_queue *) arg;
    while (1) {
        int fd = block_queue_pop(blockQueue);
        printf("get fd in thread, fd==%d, tid == %d", fd, tid);
        loop_echo(fd);
    }

    return NULL;
}

int main(int c, char **v) {
    //int listener_fd = tcp_server_listen(SERV_PORT);
    int listener_fd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listener_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(listener_fd, LISTENQ);

    block_queue blockQueue;
    block_queue_init(&blockQueue, BLOCK_QUEUE_SIZE);

    thread_array = calloc(THREAD_NUMBER, sizeof(Thread));
    int i;
    for (i = 0; i < THREAD_NUMBER; i++) {
        pthread_create(&(thread_array[i].thread_tid), NULL, thread_run, (void *) &blockQueue);
    }

    while (1) {
        struct sockaddr_storage ss;
        socklen_t slen = sizeof(ss);
        int fd = accept(listener_fd, (struct sockaddr *) &ss, &slen);
        if (fd < 0) {
            error(1, errno, "accept failed");
        } else {
            block_queue_push(&blockQueue, fd);
        }
    }

    return 0;
}