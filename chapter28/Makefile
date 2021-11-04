.PHONY: all 
all: build target

build: libgeektime.a

libgeektime.a: acceptor.o buffer.o channel_map.o channel.o event_loop_thread.o poll_dispatcher.o event_loop_thread.o event_loop.o log.o read.o tcp_connection.o tcp_server.o thread_pool.o utils.o
	ar rc $@ $^

acceptor.o: ../lib/acceptor.c
	gcc -c -g $<
buffer.o: ../lib/buffer.c
	gcc -c -g $<
channel_map.o: ../lib/channel_map.c
	gcc -c -g $<
channel.o: ../lib/channel.c
	gcc -c -g $<
buffer.o: ../lib/buffer.c
	gcc -c -g $<
poll_dispatcher.o: ../lib/poll_dispatcher.c
	gcc -c -g $< -lpthread
event_loop.o: ../lib/event_loop.c
	gcc -c -g $<
event_loop_thread.o: ../lib/event_loop_thread.c
	gcc -c -g $<
log.o: ../lib/log.c
	gcc -c -g $<
read.o: ../lib/read.c
	gcc -c -g $<
tcp_connection.o: ../lib/tcp_connection.c
	gcc -c -g $<
tcp_server.o: ../lib/tcp_server.c
	gcc -c -g $<
thread_pool.o: ../lib/thread_pool.c
	gcc -c -g $<
utils.o: ../lib/utils.c
	gcc -c -g $<

target: tcpserver
tcpserver: tcp_server.c
	gcc -g tcp_server.c -L. -lgeektime -lpthread -o tcp_server

.PHONY: clean
clean:
	rm *.o libgeektime.a tcp_server *.out