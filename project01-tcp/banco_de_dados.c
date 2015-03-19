#include <stdio.h>
#include <stdlib.h>

#include "locadora.h"

int main(int argc, char *argv[]){
	if(argc <= 1){
		printf("Sintaxe: ./banco_de_dados [nome_do_arquivo]\n");
		return 0;
	}
	FILE *fp;
	fp=fopen(argv[1], "r+");
	
	filme locadora [1000];
	printf("Insira o numero de filmes:\n");
	int n, m = 0;
	
	if(fp != NULL){
		fread(&m, sizeof(int), 1, fp);
		fread(locadora, sizeof(filme), m, fp);
		fclose(fp);
	}
	
	scanf("%d", &n);
	char aux;
	scanf("%c", &aux);
	int i, j;
	for(i = m; i < m+n; i++){
		locadora[i].id = i+1;
		printf("Insira o titulo do filme %d:\n", i+1);
		fgets(locadora[i].titulo, 30, stdin);
		
		printf("Insira o ano do filme:\n");
		scanf(" %d", &(locadora[i].ano));
		
		printf("Insira o indice do gênero do filme (ACAO=1, DRAMA=2, COMEDIA=3, AVENTURA=4, TERROR=5, SUSPENSE=6, DOCUMENTARIO=7:\n");
		scanf(" %d", &(locadora[i].genero));
		
		scanf("%c", &aux);
		printf("Insira a sinopse do filme: (MAX 1000 caracteres)\n");
		fgets(locadora[i].sinopse, 1000, stdin);
		
		printf("Insira o número de exemplares disponiveis do filme:\n");
		scanf(" %d", &(locadora[i].num_exemplares));
	}
	

	fp=fopen(argv[1], "w+b");
	fwrite(&n, sizeof(int), 1, fp);
	fwrite(locadora, sizeof(filme), m+n, fp);
	fclose(fp);
	
	return 0;
}