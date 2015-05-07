/*
 * MC833 Projeto 1 - Servidor Concorrente Sobre TCP
 * Matheus Mostiack Pomaleski 	RA 118124
 * Felipe Santos Oliveira 		RA 119383 
 * server.c
 * Baseado no simple stream server presente em 
 * http://beej.us/guide/bgnet/output/html/multipage/clientserver.html#simpleserver
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>


#define PORT "3535"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold

#define MAXDATASIZE 1024 // max number of bytes we can get at once

typedef struct {
	char titulo[30];
	int ano;
	int idGenero;
	char genero[15];
	int duracao;
	char sinopse[400];
	char diretor[40];
	char elenco[100];
	int num_exemplares;
	int id;
} filme;

struct timeval tm1;
static inline void start()
{
    gettimeofday(&tm1, NULL);
}

static inline void stop()
{
    struct timeval tm2;
    gettimeofday(&tm2, NULL);
    unsigned long long t = 1000000 * (tm2.tv_sec - tm1.tv_sec) + (tm2.tv_usec - tm1.tv_usec);
    printf("%llu us\n", t);
}

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

	// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void mywrite(int new_fd, char buf[]){
	char sendBuf[MAXDATASIZE];
	int i;
	for(i = 0; buf[i] != '\0' && buf[i] != '\n'; i++){
		sendBuf[i] = buf[i];
	}
	sendBuf[i] = buf[i];
	i++;
	if (write(new_fd, sendBuf, i) == -1)
		perror("Erro no write");
}

int myread(int new_fd, char buf[]){
	int msgsize;
	if((msgsize = read(new_fd, buf, MAXDATASIZE-1)) == -1) {
			perror("Erro no read");
			exit(1);
		}
	return msgsize;
}

void writeFilmInfo(char buf[], filme locadora[], int id){
	sprintf(buf, "%s (%d): Id: %d; Genero: %s; Duracao: %d min; Sinopse: %s Diretor: %s; Elenco: %s.\n", 
								locadora[id].titulo, locadora[id].ano, locadora[id].id, locadora[id].genero, locadora[id].duracao,
								locadora[id].sinopse, locadora[id].diretor, locadora[id].elenco);
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
	
void conexao(char *argv, int new_fd){
	char writebuf[MAXDATASIZE], auxbuf[MAXDATASIZE], readbuf[MAXDATASIZE];
	int i, id;
	filme locadora [1000];
	FILE *fp;
	fp=fopen(argv, "r+");
	if(fp == NULL){
		printf("Arquivo de Banco de Dados nao encontrado\n");
		return;
	}
	int numfilmes;
	fread(&numfilmes, sizeof(int), 1, fp);
	fread(locadora, sizeof(filme), numfilmes, fp);
	fclose(fp);
	sprintf(writebuf, "Bem vindo ao servidor da locadora, possuimos %d filmes", numfilmes);
	mywrite(new_fd, writebuf);
	
	myread(new_fd, readbuf);
	while(readbuf[0] != '8'){
		switch(readbuf[0])
		{
			case '1':
			{
				start();
				for(i = 0; i < numfilmes; i++){
					sprintf(writebuf, "%s (%d)\n", locadora[i].titulo, locadora[i].ano);
					mywrite(new_fd, writebuf);
				}
				mywrite(new_fd, "\0");
				printf("Tempo de Processamento da Operacao 1: ");
				stop();
				break;
			}
			case '2':
			{
				start();
				if(readbuf[2] < '0' || readbuf[2] > '9'){
					mywrite(new_fd, "Genero Invalido");
					break;
				}
				sprintf(auxbuf, "%d", locadora[i].ano);
				for(i = 0; i < numfilmes; i++){
					if(locadora[i].idGenero != readbuf[2]-'0')
						continue;
					strcpy(writebuf, locadora[i].titulo);
					sprintf(auxbuf, " (%d)\n", locadora[i].ano);
					strcat(writebuf, auxbuf);
					mywrite(new_fd, writebuf);
				}
				mywrite(new_fd, "\0");
				printf("Tempo de Processamento da Operacao 2: ");
				stop();
				break;
			}
			case '3':
			{
				start();
				id = strToInt(&(readbuf[2]))-1;
				strcpy(writebuf, locadora[id].sinopse);
				strcat(writebuf, "\0");
				mywrite(new_fd, writebuf);
				printf("Tempo de Processamento da Operacao 3: ");
				stop();
				break;
			}
			case '4':
			{
				start();
				id = strToInt(&(readbuf[2]))-1;
				writeFilmInfo(writebuf, locadora, id);
				mywrite(new_fd, writebuf);
				mywrite(new_fd, "\0");
				printf("Tempo de Processamento da Operacao 4: ");
				stop();
				break;
			}
			case '5':
			{
				start();
				for(i = 0; i < numfilmes; i++){
					writeFilmInfo(writebuf, locadora, i);
					mywrite(new_fd, writebuf);
				}
				mywrite(new_fd, "\0");
				printf("Tempo de Processamento da Operacao 5: ");
				stop();
				break;
			}
			case '6':
			{
				start();
				id = strToInt(&(readbuf[2]))-1;
				int shift = 3;
				int aux = id+1;
				while(aux > 0){
					shift++;
					aux = aux/10;
				}
				locadora[id].num_exemplares = strToInt(&(readbuf[shift]));
				sprintf(writebuf, "Numero de exemplares de %d atualizado para %d\n", id+1, locadora[id].num_exemplares);
				fp=fopen(argv, "w+");
				fwrite(&numfilmes, sizeof(int), 1, fp);
				fwrite(locadora, sizeof(filme), numfilmes, fp);
				fclose(fp);
				mywrite(new_fd, writebuf);
				mywrite(new_fd, "\0");
				printf("Tempo de Processamento da Operacao 6: ");
				stop();
				break;
			}
			case '7':
			{
				start();
				fp=fopen(argv, "r+");
				if(fp == NULL){
					printf("Arquivo de Banco de Dados nao encontrado\n");
					return;
				}
				fread(&numfilmes, sizeof(int), 1, fp);
				fread(locadora, sizeof(filme), numfilmes, fp);
				fclose(fp);
				
				id = strToInt(&(readbuf[2]))-1;
				sprintf(writebuf, "O filme %d possui %d exemplares\n", id+1, locadora[id].num_exemplares);
				mywrite(new_fd, writebuf);
				mywrite(new_fd, "\0");
				printf("Tempo de Processamento da Operacao 7: ");
				stop();
				break;
			}
			default:
			{
				mywrite(new_fd, "Isso não é uma opcao valida.\n");
				break;
			}
		}
		myread(new_fd, readbuf);
	}

	close(new_fd);
	exit(0);
}
	
int main(int argc, char *argv[])
	{
	if(argc != 2){
		printf("Sintaxe: ./server [nome_do_arquivo_de_bd]\n");
		return 0;
	}

	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("Servidor: erro no bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "Servidor: falhou no bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("Erro no listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}


	printf("Servidor: esperando conexoes...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("Erro no accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("Servidor: conectei com %s\n", s);

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			conexao(argv[1], new_fd);
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}
