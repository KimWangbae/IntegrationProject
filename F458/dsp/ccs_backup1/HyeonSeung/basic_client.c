#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

typedef struct sockaddr_in	si;
typedef struct sockaddr *	sap;

void err_handler(char *msg)
{
	fputs(msg, stderr);
	fputs('\n', stderr);
	exit(1);
}

int main(int argc, char **argv)
{
	int sock;
	int str_len;
	si serv_addr;
	char msg[32];

	
	if(argc != 3)
	{
		printf("Usage: %s <ip> <port>\n", argv[0]);
		exit(-1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);

	if(serv_sock == -1)
		err_handler("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_familly = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2])); 
	
	if(connect(sock,, (sap)&serv_addr, sizeof(serv_addr)) == -1)
		err_handler("connect() error");

	if(listen(serv_sock, 10) == -1)
		err_handler("listen() error");

	str_len = read(sock, msg, sizeof(msg) - 1 );
	
	if(clnt_sock == -1)
		err_handler("read() error");

	printf("msg from server: %s\n", msg);

	close(sock);

	return 0;
}


