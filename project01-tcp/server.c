/*
** server.c -- a stream socket server demo
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


#define PORT "3535"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold

#define MAXDATASIZE 512 // max number of bytes we can get at once 

typedef struct {
	int id;
	char titulo[30];
	int ano;
	int genero;
	char sinopse[400];
	int num_exemplares;
} filme;

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
	if (write(new_fd, buf, MAXDATASIZE-1) == -1)
		perror("Erro no write");
}

int myread(int new_fd, char buf[], int size){
	int numbytes;
	if ((numbytes = read(new_fd, buf, MAXDATASIZE-1)) == -1) {
		perror("Erro no read");
		exit(1);
	}
	return numbytes;
}

void writeFilmInfo(char buf[], filme locadora[], int id){
	strcpy(buf, locadora[id].titulo);
	strcat(buf, " (");
	char buf2[6];
	sprintf(buf2, "%d", locadora[id].ano);
	strcat(buf, buf2);
	strcat(buf, "): Genero: ");
	switch(locadora[id].genero){
		case 1:
			strcat(buf, "Acao; Sinopse: ");
			break;
		case 2:
			strcat(buf, "Drama; Sinopse:  ");
			break;
		case 3:
			strcat(buf, "Comedia; Sinopse:  ");
			break;
		case 4:
			strcat(buf, "Aventura; Sinopse:  ");
			break;
		case 5:
			strcat(buf, "Terror; Sinopse:  ");
			break;
		case 6:
			strcat(buf, "Suspense; Sinopse:  ");
			break;
		case 7:
			strcat(buf, "Documentario; Sinopse:  ");
			break;
	}
	strcat(buf, locadora[id].sinopse);
}
	
int strToInt(char buf[]){
	int id = 0;
	int i = 0;
	while(buf[i] != '\0'){
		id = id*10;
		id+=buf[i]-'0';
		i++;
	}
	return id;
}
	
void conexao(int new_fd, filme locadora[], int numfilmes){
	int numbytes;
	char buf[MAXDATASIZE], buf2[MAXDATASIZE];
	numbytes = myread(new_fd, buf, MAXDATASIZE-1);

/*
1 - Listar todos os titulos dos filmes e o ano de lancamento
2 - Listar todos os titulos dos filmes e o ano de lancamento de um genero
3 - Mostrar sinopse de um filme
4 - Mostrar todas as informacoes de um filme
5 - Mostrar todas as informacoes de todos os filmes
6 - Alterar estoque
7 - Ver estoque de um filme
8 - Sair
*/	
	int i, id, genero;
	while(buf[0] != '8'){

		switch(buf[0])
		{
			case '1':
			{
				for(i = 0; i < numfilmes; i++){
					strcpy(buf, locadora[i].titulo);
					strcat(buf, " (");
					sprintf(buf2, "%d", locadora[i].ano);
					strcat(buf, buf2);
					strcat(buf, ")");
					mywrite(new_fd, buf);
				}
				mywrite(new_fd, "\0");
				break;
			}
			case '2':
			{
				numbytes = myread(new_fd, buf, MAXDATASIZE-1);
				if(buf[0] < '0' || buf[0] > '9'){
					mywrite(new_fd, "Genero Invalido");
					break;
				}
				sprintf(buf2, "%d", locadora[i].ano);
				for(i = 0; i < numfilmes; i++){
					if(locadora[i].genero != buf[0]-'0')
						continue;
					strcpy(buf, locadora[i].titulo);
					strcat(buf, " (");
					sprintf(buf2, "%d", locadora[i].ano);
					strcat(buf, buf2);
					strcat(buf, ")");
					mywrite(new_fd, buf);
				}
				mywrite(new_fd, "\0");
				break;
			}
			case '3':
			{
				numbytes = myread(new_fd, buf, MAXDATASIZE-1);
				id = strToInt(buf);
				strcpy(buf, locadora[id].sinopse);
				mywrite(new_fd, buf);
				break;
			}
			case '4':
			{
				numbytes = myread(new_fd, buf, MAXDATASIZE-1);
				id = strToInt(buf);
				writeFilmInfo(buf, locadora, id);
				mywrite(new_fd, buf);
				break;
			}
			case '5':
			{
				for(i = 0; i < numfilmes; i++){
					writeFilmInfo(buf, locadora, i);
					mywrite(new_fd, buf);
				}
				mywrite(new_fd, "\0");
				break;
			}
			case '6':
			{
				numbytes = myread(new_fd, buf, MAXDATASIZE-1);
				id = strToInt(buf);
				numbytes = myread(new_fd, buf, MAXDATASIZE-1);
				locadora[id].num_exemplares = strToInt(buf);
				break;
			}
			case '7':
			{
				numbytes = myread(new_fd, buf, MAXDATASIZE-1);
				id = strToInt(buf);
				sprintf(buf, "%d", locadora[id].num_exemplares);
				mywrite(new_fd, buf);
			}
			default:
			{
				printf("Isso não é uma opcao valida.\n");
				break;
			}
		}
		numbytes = myread(new_fd, buf, MAXDATASIZE-1);
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
	filme locadora [1000];
	FILE *fp;
	fp=fopen(argv[1], "r+");
	if(fp == NULL){
		printf("Arquivo de Banco de Dados nao encontrado\n");
		return 0;
	}
	int numfilmes;
	fread(&numfilmes, sizeof(int), 1, fp);
	fread(locadora, sizeof(filme), numfilmes, fp);
	fclose(fp);
	int i;
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
			mywrite(new_fd, "Bem vindo ao servidor da locadora");
			conexao(new_fd, locadora, numfilmes);
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}