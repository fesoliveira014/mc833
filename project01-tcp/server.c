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

void mywrite(int sockfd, char buf[], struct sockaddr_storage their_addr, socklen_t addr_len){	
	char sendBuf[MAXDATASIZE];
	int i;
	for(i = 0; buf[i] != '\0' && buf[i] != '\n'; i++){
		sendBuf[i] = buf[i];
	}
	sendBuf[i] = buf[i];
	i++;
	if (sendto(sockfd, sendBuf, i, 0,(struct sockaddr *)&their_addr, sizeof(their_addr)) == -1)
		perror("Erro no write");
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
	
void conexao(char *argv, int sockfd){
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
	struct sockaddr_storage their_addr;
    socklen_t addr_len;
	
	while(1){
		addr_len = sizeof(their_addr);
		int aux;
		do{ 
			aux = (recvfrom(sockfd, readbuf, MAXDATASIZE-1 , 0,
			(struct sockaddr *)&their_addr, &addr_len));
		} while(aux == -1);
		switch(readbuf[0])
		{
			case '1':
			{
				for(i = 0; i < numfilmes; i++){
					sprintf(writebuf, "%s (%d)\n", locadora[i].titulo, locadora[i].ano);
					mywrite(sockfd, writebuf, their_addr, addr_len);
				}
				mywrite(sockfd, "\0", their_addr, addr_len);
				break;
			}
			case '2':
			{
				if(readbuf[2] < '0' || readbuf[2] > '9'){
					mywrite(sockfd, "Genero Invalido", their_addr, addr_len);
					break;
				}
				sprintf(auxbuf, "%d", locadora[i].ano);
				for(i = 0; i < numfilmes; i++){
					if(locadora[i].idGenero != readbuf[2]-'0')
						continue;
					strcpy(writebuf, locadora[i].titulo);
					sprintf(auxbuf, " (%d)\n", locadora[i].ano);
					strcat(writebuf, auxbuf);
					mywrite(sockfd, writebuf, their_addr, addr_len);
				}
				mywrite(sockfd, "\0", their_addr, addr_len);
				break;
			}
			case '3':
			{
				id = strToInt(&(readbuf[2]))-1;
				strcpy(writebuf, locadora[id].sinopse);
				strcat(writebuf, "\0");
				mywrite(sockfd, writebuf, their_addr, addr_len);
				break;
			}
			case '4':
			{
				id = strToInt(&(readbuf[2]))-1;
				writeFilmInfo(writebuf, locadora, id);
				mywrite(sockfd, writebuf, their_addr, addr_len);
				mywrite(sockfd, "\0", their_addr, addr_len);
				break;
			}
			case '5':
			{
				for(i = 0; i < numfilmes; i++){
					writeFilmInfo(writebuf, locadora, i);
					mywrite(sockfd, writebuf, their_addr, addr_len);
				}
				mywrite(sockfd, "\0", their_addr, addr_len);
				break;
			}
			case '6':
			{
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
				mywrite(sockfd, writebuf, their_addr, addr_len);
				mywrite(sockfd, "\0", their_addr, addr_len);
				break;
			}
			case '7':
			{
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
				mywrite(sockfd, writebuf, their_addr, addr_len);
				mywrite(sockfd, "\0", their_addr, addr_len);
				break;
			}
			default:
			{
				mywrite(sockfd, "Isso não é uma opcao valida.\n", their_addr, addr_len);
				break;
			}
		}
	}

	close(sockfd);
	exit(0);
}
	
int main(int argc, char *argv[])
	{
	if(argc != 2){
		printf("Sintaxe: ./server [nome_do_arquivo_de_bd]\n");
		return 0;
	}

	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int yes=1;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
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
	static struct timeval tm;
	tm.tv_sec = 0;
	tm.tv_usec = 100000;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(tm));

	freeaddrinfo(servinfo); // all done with this structure

	printf("Servidor: esperando conexoes...\n");


	conexao(argv[1], sockfd);

	return 0;
}
