all: server.c client.c
	gcc server.c -o server
	gcc client.c -o client -pthread

clean:
	rm -f server client