#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int id;
	char titulo[30];
	int ano;
	int genero;
	char sinopse[400];
	int num_exemplares;
} filme;



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

	int i, j;
	for(i = m; i < m+n; i++){
		locadora[i].id = i+1;
		printf("Insira o titulo do filme %d:\n", i+1);
		scanf("%c", &aux);
		while(aux == '\n' || aux == ' ')
			scanf("%c", &aux);
		
		j = 0;
		while(aux != '\n' && j < 29){
			locadora[i].titulo[j] = aux;
			j++;
			scanf("%c", &aux);
		}
		locadora[i].titulo[j] = '\0';
		
		
		printf("Insira o ano do filme:\n");
		scanf(" %d", &(locadora[i].ano));
		
		printf("Insira o indice do gênero do filme (ACAO=1, DRAMA=2, COMEDIA=3, AVENTURA=4, TERROR=5, SUSPENSE=6, DOCUMENTARIO=7:\n");
		scanf(" %d", &(locadora[i].genero));
		

		printf("Insira a sinopse do filme: (MAX 400 caracteres)\n");
		scanf("%c", &aux);
		while(aux == '\n' || aux == ' ')
			scanf("%c", &aux);
		j = 0;
		while(aux != '\n' && j < 400){
			locadora[i].sinopse[j] = aux;
			j++;
			scanf("%c", &aux);
		}
		locadora[i].sinopse[j] = '\0';
		
		printf("Insira o número de exemplares disponiveis do filme:\n");
		scanf(" %d", &(locadora[i].num_exemplares));
		
		printf("%s (%d) - Genero %d - %d exemplares - Sinopse:%s\n", locadora[i].titulo, locadora[i].ano, locadora[i].genero, locadora[i].num_exemplares, locadora[i].sinopse);
	}
	

	fp=fopen(argv[1], "w+b");
	fwrite(&n, sizeof(int), 1, fp);
	fwrite(locadora, sizeof(filme), m+n, fp);
	fclose(fp);
	
	return 0;
}