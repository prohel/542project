all: server

twilio: twilio.o
	clang `curl-config --cflags` -c twilio.c

server: LinkedList.c server.c twilio.o
	clang LinkedList.c server.c -o server twilio.o `curl-config --libs`


run: server
	./server 3000

clean:
	rm server twilio.o
