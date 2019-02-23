#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <pthread.h>

struct args{
	int n;
	int t;
	char nchar[20];
	char hostandport[512];
} *contents[1024];

void *thread_function(void* argv){

	int n = ((struct args*)argv)->n;
	int t = ((struct args*)argv)->t;
	char nchar[512] = "";
	char hostandport[512] = "";
	strcpy(nchar, ((struct args*)argv)->nchar);
	strcpy(hostandport, ((struct args*)argv)->hostandport);

	while (n != 0){
		n --;

		// initialize the client
		int sock = socket(AF_INET, SOCK_STREAM, 0);

		char addr[512] = "";
		char port[512] = "";
		sscanf(hostandport, "%[^:]:%s", addr, port);

		struct addrinfo hint, *res;
		memset(&hint, 0, sizeof(hint));
		hint.ai_family = AF_INET;
		hint.ai_socktype = SOCK_STREAM;
		getaddrinfo(addr, port, &hint, &res);
		getnameinfo(res->ai_addr, res->ai_addrlen, addr, sizeof(addr), NULL, 0, NI_NUMERICHOST);

		struct sockaddr_in sockaddr;
		bzero(&sockaddr, sizeof(sockaddr));
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_addr.s_addr = inet_addr(addr);
		sockaddr.sin_port = htons(atoi(port));

		if (sock < 0){
			fprintf(stderr, "timeout when connect to server %s\n", addr); sleep(t / 1000); continue;}

		if (connect(sock, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0){
			fprintf(stderr, "timeout when connect to server %s\n", addr); sleep(t / 1000); continue;}

		const double beg = clock();

		char buffer_test[60] = "";
		strcpy(buffer_test, nchar);
		send(sock, buffer_test, sizeof(buffer_test), 0);
	
		int ret;

		// send the message to the server
		char buffer_send[60] = "pusheeeeen\n";
		ret = send(sock, buffer_send, sizeof(buffer_send), 0);
		if (ret < 0) break;

		// receive the message from the client
		char buffer_recv[60] = "";
		ret = recv(sock, buffer_recv, sizeof(buffer_recv), 0);
		if (ret < 0) break;


		const double end = clock();
		if (end - beg > t){fprintf(stderr, "timeout when connect to %s\n", addr);}
		else fprintf(stderr, "recv from %s, RTT = %f msec\n", addr, end - beg);
		close(sock);
	}
	fprintf(stderr, "\n");
	return 0;
}

int main(int argc, char *argv[]){

	// judge if one's inputs correct
	if (argc < 2){
		fprintf(stderr, "usage: %s [host]:[port]\n", argv[0]);
		exit(1);
	}

	int w = 1;
	int n = -1, t = 1000;
	if (argv[w][0] == '-' && argv[w][1] == 'n'){n = atoi(argv[w + 1]); w = w + 2;}
	if (argv[w][0] == '-' && argv[w][1] == 't'){t = atoi(argv[w + 1]); w = w + 2;}
	if (n == 0) n = -1;

	pthread_t tid[1024];

	int tmp = n;
	for (int k = w; k < argc; k ++){
		n = tmp;
		
		int tmp = k - w;
		contents[tmp] = (struct args*)malloc(sizeof(struct args));
		contents[tmp]->n = n;
		contents[tmp]->t = t;
		if (n == -1) strcpy(contents[tmp]->nchar, "-1");
		else strcpy(contents[tmp]->nchar, argv[2]);
		strcpy(contents[tmp]->hostandport, argv[k]);
		pthread_create(&tid[tmp], NULL, thread_function, (void*)contents[tmp]);
	}

	for (int k = w; k < argc; k ++){
		int tmp = k - w;
		pthread_join(tid[tmp], NULL);
	}

	return 0;
}