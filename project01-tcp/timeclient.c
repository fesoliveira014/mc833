/*
 * MC833 Projeto 1 - Servidor Concorrente Sobre TCP
 * Matheus Mostiack Pomaleski 	RA 118124
 * Felipe Santos Oliveira 		RA 119383 
 * client.c
 * Baseado no Simple Stream Cliente presente em
 * http://beej.us/guide/bgnet/output/html/multipage/clientserver.html#simpleclient
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
#include <sys/time.h>
 
#include <arpa/inet.h>
 
#define PORT "3535" // the port client will be connecting to
 
#define MAXDATASIZE 1024 // max number of bytes we can get at once

void show_menu();
void show_genres();
void mywrite(int new_fd, char buf[]);
void readAndPrint(int sockfd);
int strToInt(char buf[]);
static struct timeval tm1;
static inline void start();
static inline void stop(int i);
 
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

	//printf("got address info\n");
	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
					if ((sockfd = socket(p->ai_family, p->ai_socktype,
													p->ai_protocol)) == -1)
					{
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

	//printf("estabilishing connection...\n");

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
									s, sizeof s);
	//printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");
			exit(1);
	}

	buf[numbytes] = '\0';

//	printf("%s\n\n",buf);
	
	int numfilmes = strToInt(&(buf[45]));

	char mywritebuf[MAXDATASIZE];
	char option;
	int id, num_exemplares;

	do
	{
//		show_menu();
		scanf(" %c", &option);
		switch(option)
		{
			case '1':
			{
				start();
				mywrite(sockfd, "1\0");
				readAndPrint(sockfd);
				stop(1);
				break;
			}
			case '2':
			{
				mywritebuf[0] = '2';
				mywritebuf[1] = ' ';
//				show_genres();
				while(1)
				{
					scanf(" %c", &(mywritebuf[2]));
					if(mywritebuf[2] < '1' || mywritebuf[2] > '7')
					{
//						printf("Opcao invalida.\n");
//						show_genres();
					}
					else
						break;
				}
				mywritebuf[3] = '\0';
				start();
				mywrite(sockfd, mywritebuf);
				readAndPrint(sockfd);
				stop(2);
				break;
			}
			case '3':
			{
				mywritebuf[0] = '3';
				mywritebuf[1] = ' ';
//				printf("Insira o id do filme: ");
				while(1)
				{
					scanf(" %d", &id);
					if(id < 1 || id > numfilmes)
					{
//						printf("Id invalido (Possuimos %d filmes).\n", numfilmes);
//						printf("Insira o id do filme: ");
					}
					else
						break;
				}
				sprintf(&(mywritebuf[2]), "%d", id);
				start();
				mywrite(sockfd, mywritebuf);
				readAndPrint(sockfd);
				stop(3);
				break;
			}
			case '4':
			{
				mywritebuf[0] = '4';
				mywritebuf[1] = ' ';
//				printf("Insira o id do filme: ");
				while(1)
				{
					scanf(" %d", &id);
					if(id < 1 || id > numfilmes)
					{
//						printf("Id invalido (Possuimos %d filmes).\n", numfilmes);
//						printf("Insira o id do filme: ");
					}
					else
						break;
				}
				sprintf(&(mywritebuf[2]), "%d", id);
				start();
				mywrite(sockfd, mywritebuf);
				readAndPrint(sockfd);
				stop(4);
				break;
			}
			case '5':
			{
				start();
				mywrite(sockfd, "5\0");
				readAndPrint(sockfd);
				stop(5);
				break;
			}
			case '6':
			{
				mywritebuf[0] = '6';
				mywritebuf[1] = ' ';
//				printf("Insira o id do filme: ");
				while(1)
				{
					scanf(" %d", &id);
					if(id < 1 || id > numfilmes)
					{
//						printf("Id invalido (Possuimos %d filmes).\n", numfilmes);
//						printf("Insira o id do filme:  ");
					}
					else
						break;
				}
//				printf("Insira o novo numero de exemplares: ");
				scanf(" %d", &num_exemplares);
				sprintf(&(mywritebuf[2]), "%d %d", id, num_exemplares);
				start();
				mywrite(sockfd, mywritebuf);
				readAndPrint(sockfd);
				stop(6);
				break;
			}
			case '7':
			{
				mywritebuf[0] = '7';
				mywritebuf[1] = ' ';
//				printf("Insira o id do filme: ");
				while(1)
				{
					scanf(" %d", &id);
					if(id < 1 || id > numfilmes)
					{
//						printf("Id invalido (Possuimos %d filmes).\n", numfilmes);
//						printf("Insira o id do filme: ");
					}
					else
						break;
				}
				sprintf(&(mywritebuf[2]), "%d", id);
				start();
				mywrite(sockfd, mywritebuf);
				readAndPrint(sockfd);
				stop(7);
				break;
			}
			
			case '8':
			{
				mywrite(sockfd, "8\0");
				break;
			}

			default:
			{
				printf("Isso não é uma opcao valida.\n");
				break; 
			}
		};

//		printf("\n");

	}while(option != '8');

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
 
void mywrite(int sockfd, char buf[]){
	char sendBuf[MAXDATASIZE];
	int i;
	for(i = 0; buf[i] != '\0' && buf[i] != '\n'; i++){
		sendBuf[i] = buf[i];
	}
	sendBuf[i] = buf[i];
	i++;
	if (write(sockfd, sendBuf, i) == -1)
		perror("Erro no write");
}

void readAndPrint(int sockfd){
	int msgsize, i; 
	char readbuf[MAXDATASIZE];
	if((msgsize = read(sockfd, readbuf, MAXDATASIZE-1)) == -1) {
		perror("Erro no read");
		exit(1);
	}
	while(msgsize != 0){
		for(i = 0; i < msgsize && readbuf[i] != '\0'; i++){
//			printf("%c", readbuf[i]);
		}
		if(i < msgsize)
			break;
		if((msgsize = read(sockfd, readbuf, MAXDATASIZE-1)) == -1) {
			perror("Erro no read");
			exit(1);
		}
	}
	return;
}

int strToInt(char buf[]){
	int id = 0;
	int i = 0;
	while(buf[i] >= '0' && buf[i] <= '9'){
		id = id*10;
		id+=buf[i]-'0';
		i++;
	}
	return id;
}

static inline void start()
{
    gettimeofday(&tm1, NULL);
}

static inline void stop(int i)
{
    struct timeval tm2;
    gettimeofday(&tm2, NULL);
    unsigned long long t = 1000000 * (tm2.tv_sec - tm1.tv_sec) + (tm2.tv_usec - tm1.tv_usec);
    printf("%d: %llu us\n", i, t);
}
