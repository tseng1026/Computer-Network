#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAXCLNT	1024

// static void init_server(unsigned short port)
int main(int argc, char *argv[]){

	// judge if one's inputs correct
	if (argc != 2){
		fprintf(stderr, "usage: %s [port]\n", argv[0]);
		exit(1);
	}

	// initialize the server
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	int port = atoi(argv[1]);

	if (sock < 0){fprintf(stderr, "[Error] Something has gone wrong with socket!\n"); return 1;}

	struct sockaddr_in sockaddr;
	bzero(&sockaddr, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr.sin_port = htons(port);

	if (bind(sock, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0){fprintf(stderr, "[Error] Something has gone wrong with binding!\n"); return 1;}
	if (listen(sock, 1024) < 0){fprintf(stderr, "[Error] Something has gone wrong with listening!\n"); return 1;}


	// initialize the client
	int clntP[1024] = {0};
	int clntN[1024] = {0};			// times
	int clntPort[1024] = {0};		// port
	char clntAddr[1024][50] = {""};	// addr

	// initialize some parameters
	int maxfd = getdtablesize();
	fd_set sel;						// main select solution
	fd_set tmp;						// temp select solution
	FD_ZERO(&sel);
	FD_SET(sock, &sel);
	struct timeval time;
	time.tv_sec = 5;

	while (1){
		tmp = sel;
		int k = select(maxfd + 1, &tmp, NULL, NULL, &time);
		if (k == 0) continue;

		for (int i = 0; i <= maxfd; i ++){

			// check new connection (if it is ready)
			if (FD_ISSET(i, &tmp) && i == sock){

				// accept the client's connection
				struct sockaddr_in clntaddr;
				int clntlen = sizeof(clntaddr);

				int clnt = accept(sock, (struct sockaddr*)&clntaddr, (socklen_t*)&clntlen);
				if (clnt < 0){fprintf(stderr, "[Error] Something has gone wrong with connecting!\n"); return 1;}

				getpeername(sock, (struct sockaddr*)&clntaddr, (socklen_t*)&clntlen);
				strcpy(clntAddr[clnt], inet_ntoa(clntaddr.sin_addr));
				clntPort[clnt] = ntohs(clntaddr.sin_port);
				
				// send the packet of testing
				char buffer_test[60] = "";
				recv(clnt, buffer_test, sizeof(buffer_test), 0);
				
				// transfer the connection from i to clnt
				clntP[clnt] = clnt;
				FD_SET(clnt, &sel);
				clntN[clnt] = atoi(buffer_test);
			}

			// check old connection (send or recv)
			else if (FD_ISSET(i, &tmp)){
				/////

				int ret;
				int clnt = clntP[i];
				clntN[clnt] --;

				// send the message to the client
				char buffer_send[60] = "test test\n";
				// fgets(buffer_send, sizeof(buffer_send), stdin);
				ret = send(clnt, buffer_send, sizeof(buffer_send), 0);

				// receive the message from the client
				char buffer_recv[60] = "";
				ret = recv(clnt, buffer_recv, sizeof(buffer_recv), 0);
				if (ret < 0){continue;}
				else if (ret == 0) ;
				else {fprintf(stderr, "recv from %s:%d\n", clntAddr[clnt], clntPort[clnt]);}

				
				// close the clientfd
				if (clntN[i] == 0 || ret == 0){
					clntP[i] = 0;
					close(clnt);
					FD_CLR(i, &sel);
				}
			}
		}
	}
	close(sock);

	return 0;
}