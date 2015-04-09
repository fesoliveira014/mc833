/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "5050" // the port client will be connecting to 

#define MAXDATASIZE 512 // max number of bytes we can get at once 

void show_menu();
void show_genres();

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	char title[30];
	char genero;

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';

	printf("%s\n",buf);

	int option = 0;

	do
	{
		show_menu();
		scanf(" %d", &option);

		switch(option)
		{
			case 1:
			{
				if(write(sockfd, "1", sizeof(char)));
				if((numbytes = read(sockfd, buf, MAXDATASIZE-1)) == -1)
				{
					perror("recv");
					exit(1);
				}
				while(buf[0] != '\0'){
					printf("%s\n", buf);
					if((numbytes = read(sockfd, buf, MAXDATASIZE-1)) == -1)
					{
						perror("recv");
						exit(1);
					}
				}
				break;
			}
			case 2:
			{
				write(sockfd, "2", sizeof(char));
				if((numbytes = read(sockfd, buf, MAXDATASIZE-1)) == -1)
				{
					perror("recv");
					exit(1);
				}
				printf("%s\n", buf);
				break;
			}
			case 3:
			{
				write(sockfd, "3", sizeof(char));
				if((numbytes = read(sockfd, buf, MAXDATASIZE-1)) == -1)
				{
					perror("recv");
					exit(1);
				}
				printf("%s\n", buf);
				break;
			}
			case 4:
			{
				write(sockfd, "4", sizeof(char));
				if((numbytes = read(sockfd, buf, MAXDATASIZE-1)) == -1)
				{
					perror("recv");
					exit(1);
				}
				printf("%s\n", buf);
				break;
			}
			case 5:
			{
				write(sockfd, "5", sizeof(char));
				if((numbytes = read(sockfd, buf, MAXDATASIZE-1)) == -1)
				{
					perror("recv");
					exit(1);
				}
				printf("%s\n", buf);
				break;
			}
			case 6:
			{
				write(sockfd, "6", sizeof(char));
				if((numbytes = read(sockfd, buf, MAXDATASIZE-1)) == -1)
				{
					perror("recv");
					exit(1);
				}
				printf("%s\n", buf);
				break;
			}
			case 7:
			{
				write(sockfd, "7", sizeof(char));
				if((numbytes = read(sockfd, buf, MAXDATASIZE-1)) == -1)
				{
					perror("recv");
					exit(1);
				}
				printf("%s\n", buf);
				break;
			}
			case 8:
			{
				write(sockfd, "8", sizeof(char));
				if((numbytes = read(sockfd, buf, MAXDATASIZE-1)) == -1)
				{
					perror("recv");
				}
				printf("%s\n", buf);
				break;
			}

			default:
			{
				printf("Isso não é uma opcao valida.\n");
				break;
			}
		};

		printf("\n");

	}while(option != 8);

	close(sockfd);

	return 0;
}

void show_menu() 
{
	printf("Escolha uma opcao:\n");
	printf("1 - Listar todos os titulos dos filmes e o ano de lancamento\n");
	printf("2 - Listar todos os titulos dos filmes e o ano de lancamento de um genero\n");
	printf("3 - Mostrar sinopse de um filme\n");
	printf("4 - Mostrar todas as informacoes de um filme\n");
	printf("5 - Mostrar todas as informacoes de todos os filmes\n");
	printf("6 - Alterar estoque\n");
	printf("7 - Ver estoque de um filme\n");
	printf("8 - Sair\n");
	printf("\nOpcao: ");
}

/*
ACAO=1, DRAMA=2, COMEDIA=3, AVENTURA=4, TERROR=5, SUSPENSE=6, DOCUMENTARIO=7
*/

void show_genres()
{
	printf("\n");
	printf("Escolha o genero: ACAO=1, DRAMA=2, COMEDIA=3, AVENTURA=4, TERROR=5, SUSPENSE=6, DOCUMENTARIO=7\n");
	printf("Genero: ");
}