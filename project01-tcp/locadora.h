typedef struct {
	int id;
	char titulo[30];
	int ano;
	int genero;
	char sinopse[1000];
	int num_exemplares;
} filme;

enum { ACAO = 1, DRAMA, COMEDIA, AVENTURA, TERROR, SUSPENSE, DOCUMENTARIO } genero;