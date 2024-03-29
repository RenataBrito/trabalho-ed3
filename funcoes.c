/* George Gantus - 10691988 */
/* Renata Brito -  10373663 */

#include "funcoes.h"


void binarioNaTela1(char *nomeArquivoBinario) {

	/* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar. */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela1): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *) malloc(fl);
	fread(mb, 1, fl, fs);

	cs = 0;
	for(i = 0; i < fl; i++) {
		cs += (unsigned long) mb[i];
	}
	printf("%lf\n", (cs / (double) 100));
	free(mb);
	fclose(fs);
}

void scan_quote_string(char *str) {

	/*
	*	Use essa função para ler um campo string delimitado entre aspas (").
	*	Chame ela na hora que for ler tal campo. Por exemplo:
	*
	*	A entrada está da seguinte forma:
	*		nomeDoCampo "MARIA DA SILVA"
	*
	*	Para ler isso para as strings já alocadas str1 e str2 do seu programa, você faz:
	*		scanf("%s", str1); // Vai salvar nomeDoCampo em str1
	*		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2 (sem as aspas)
	*
	*/

	char R;

	while((R = getchar()) != EOF && isspace(R)); // ignorar espaços, \r, \n...

	if(R == 'N' || R == 'n') { // campo NULO
		getchar(); getchar(); getchar(); // ignorar o "ULO" de NULO.
		strcpy(str, ""); // copia string vazia
	} else if(R == '\"') {
		if(scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
			strcpy(str, "");
		}
		getchar(); // ignorar aspas fechando
	} else if(R != EOF){ // vc tá tentando ler uma string que não tá entre aspas! Fazer leitura normal %s então...
		str[0] = R;
		scanf("%s", &str[1]);
	} else { // EOF
		strcpy(str, "");
	}
}

void lerAtePipe(char *campo,FILE *file){
    char c;
    int contador = 0;
    fread(&c,1,1,file);
    while(c != '|'){
        campo[contador] = c;
        contador ++;
        fread(&c,1,1,file);
    }
    campo[contador] = '\0';

}

int CsvtoBin(char* nome_arq_csv,char* nomeArqGe, struct NoCidades **inicio){
    // Ponteiros para os arquivos
    FILE* csv;
    FILE* bin;
    csv = fopen(nome_arq_csv,"rb"); // abre o arquivo csv
    bin = fopen(nomeArqGe,"wb"); // cria o arquivo binário
    if(!csv){
        printf("Falha no carregamento do arquivo.");
        return ERRO; // caso tenha ocorrido algum erro com o arquivo, retorna 0
    }
    if(!bin){
        printf("Falha no carregamento do arquivo.");
        return ERRO; // caso tenha ocorrido algum erro com o arquivo, retorna 0
    }
    struct Cabecalho cabecalho;
    struct Dados dado;
    //Dados para criar o cabecalho
    strcpy(cabecalho.dataUltimaCompactacao,"##########");
    cabecalho.numeroArestas = 0;
    cabecalho.numeroVertices = 0;
    cabecalho.status = '0';

    fwrite(&cabecalho.status,1,1,bin);
    fwrite(&cabecalho.numeroVertices,sizeof(int),1,bin);//OBS sizeof(int) é de 4bits por padrao do GCC
    fwrite(&cabecalho.numeroArestas,sizeof(int),1,bin);
    fwrite(cabecalho.dataUltimaCompactacao,10,1,bin);
    //Fim do cabecalho

    char delimitador = '|';
    char lixo = '#';
    char limpalinha[100];
    int a,b,c;
    char aux;
    int cntaux;
    dado.distancia = -1;//Esta linha e a de baixo servem para tratar os casos onde a leitura seja nula ja que iniciamos o campo com um valor conhecido
    dado.estadoDestino[0] = dado.estadoOrigem[0] = dado.cidadeDestino[0] = dado.cidadeOrigem[0] = dado.tempoViagem[0] = '\0';
    fscanf(csv,"%[^\n]%*c",limpalinha);

    while(fscanf(csv,"%[^,]%*c %[^,]%*c %d%*c %[^,]%*c %[^,]%*c",dado.estadoOrigem,dado.estadoDestino,&dado.distancia,dado.cidadeOrigem,dado.cidadeDestino) != EOF){
        cntaux = 0;
        fscanf(csv,"%c",&aux);
        if(aux == '\n'){
            dado.tempoViagem[0] = '\0';
        }else{
            while (aux != '\n'){
                dado.tempoViagem[cntaux]= aux;
                fscanf(csv,"%c",&aux);
                cntaux++;
            }
            dado.tempoViagem[cntaux]='\0';
        }
        if(dado.estadoDestino[0] == '\0'){
            dado.estadoDestino[1] = '#';
        }
        if(dado.estadoOrigem[0]== '\0'){
            dado.estadoOrigem[1] = '#';
        }
        //escrita dos campos de tamanho fixo
        fwrite(dado.estadoOrigem,2,1,bin);//escreve os dois primeiros bytes de dado.estado origem arquivo binario
        fwrite(dado.estadoDestino,2,1,bin);
        fwrite(&dado.distancia,sizeof(int),1,bin);

        //campos de tamanho varaivel

        /*Não se realizou o tratamento de truncamento pq na especificação do trabalho foi
          dito que não era necessário pois o arquivo de dados garantia regularidade */

        a = strlen(dado.cidadeOrigem);
        b = strlen(dado.cidadeDestino);
        c = strlen(dado.tempoViagem);
        fwrite(dado.cidadeOrigem,a,1,bin);
        fwrite(&delimitador,1,1,bin);
        fwrite(dado.cidadeDestino,b,1,bin);
        fwrite(&delimitador,1,1,bin);
        fwrite(dado.tempoViagem,c,1,bin);
        fwrite(&delimitador,1,1,bin);
        for(int i = 0; i < 77 - (a+b+c+3);i++){//escreve lixo no resto dos espaços livres
            fwrite(&lixo,1,1,bin);
        }
        if(!buscaCidade(dado.cidadeOrigem,*inicio)){
            cabecalho.numeroVertices = insereCidade(dado.cidadeOrigem,inicio);
        }
        if(!buscaCidade(dado.cidadeDestino,*inicio)){
            cabecalho.numeroVertices = insereCidade(dado.cidadeDestino,inicio);
        }
       cabecalho.numeroArestas++;
    }
    /*Atualizar o Cabecalho*/
    fseek(bin,0,SEEK_SET);
    cabecalho.status = '1';
    escreveCabecalho(cabecalho,bin);
    fclose(bin);
    fclose(csv);
    binarioNaTela1(nomeArqGe);
    return 0;

}

int print_reg(char* nome_arq){
    FILE *file;
    file = fopen(nome_arq,"rb"); // abre o arquivo gerado para leitura
    if(!file){
        printf("Falha no processamento do arquivo.");
        return ERRO; // caso tenha ocorrido algum erro com o arquivo, retorna 0
    }
    char letra;
    fread(&letra,1,1,file);
    if(letra == '0'){
        printf("Falha no processamento do arquivo.");
        return ERRO;
    }
    struct Dados registro;
    int contador = 0;
    int rrn = 0;
    fseek(file,5,SEEK_SET);
    int numeroReg;

    fseek(file,5,SEEK_SET);
    fread(&numeroReg,4,1,file);
    if(numeroReg == 0){
        printf("Registro inexistente.");
    }

    fseek(file, (rrn*TAMREGISTRO)+19, SEEK_SET);//Pular o cabeçalho
    registro.estadoOrigem[2] = registro.estadoDestino[2] = '\0';
    while(contador < numeroReg){
        lerRegistro(file,&registro);
        rrn++;
        fseek(file,(rrn*TAMREGISTRO)+19,SEEK_SET);
        if(registro.estadoOrigem[0] != '*'){//so imprime se o campo nao estiver apagado e so conta como aresta se o registro nao estiver apagado
            printf("%d %s %s %d %s %s %s \n",contador,registro.estadoOrigem,registro.estadoDestino,registro.distancia,registro.cidadeOrigem,registro.cidadeDestino,registro.tempoViagem);
            contador++;
        }

    }
    fclose(file);
    return 1;
}
int buscaCidade(char *cidade,struct NoCidades *inicio){
    if(inicio == NULL){
        return 0;
    }
    while(inicio != NULL){
        if(!strcmp(inicio->cidade,cidade)){
            return 1;
        }
        inicio = inicio->prox;
    }
    return 0;

}
int insereCidade(char *cidade,struct NoCidades **inicio){
    struct NoCidades *aux = *(inicio);
    struct NoCidades *elem = calloc(1,sizeof(struct NoCidades));
    int contador = 2;
    strcpy(elem->cidade,cidade);
    if(cidade == NULL){
        return 0;
    }
    if(aux == NULL){
       *inicio = elem;
       return 1;
    }
    while(aux->prox != NULL){
        aux = aux->prox;
        contador++;
    }
    aux->prox = elem;
    return contador;
}
struct Dados buscaPorRRN(char *nomeArquivo,int RRN){
    FILE *file;
    int numreg;
    struct Dados resultado;
    file = fopen(nomeArquivo,"rb");
    fseek(file,5,SEEK_SET);
    fread(&numreg,4,1,file);
    resultado.distancia = -1;
    if(RRN > numreg-1){
        fclose(file);
        return resultado;
    }
    fseek(file,(RRN*TAMREGISTRO)+19,SEEK_SET);
    fread(resultado.estadoOrigem,2,1,file);
    fread(resultado.estadoDestino,2,1,file);
    resultado.estadoOrigem[2] = resultado.estadoDestino[2] = '\0';
    fread(&resultado.distancia,4,1,file);
    lerAtePipe(resultado.cidadeOrigem,file);
    lerAtePipe(resultado.cidadeDestino,file);
    lerAtePipe(resultado.tempoViagem,file);
    fclose(file);
    return resultado;
}
void printaRegistro(struct Dados r, int RRN){
    printf("%d %s %s %d %s %s %s",RRN,r.estadoOrigem,r.estadoDestino,r.distancia,r.cidadeOrigem,r.cidadeDestino,r.tempoViagem);
}
struct Cabecalho leCabecalho(FILE *file){
    struct Cabecalho r;
    r.dataUltimaCompactacao[10] = '\0';
    rewind(file); // volta a posicao do cursor para o começo do arquivo
    fread(&r.status,1,1,file);
    fread(&r.numeroVertices,4,1,file);
    fread(&r.numeroArestas,4,1,file);
    fread(&r.dataUltimaCompactacao,10,1,file);
    rewind(file);
    return r;
}
int procuraRegistro(char *campo,char *nomeArq,char *valor, int dist,int opr){
    FILE *file;
    struct Cabecalho c;
    struct Dados reg;
    reg.estadoDestino[2] = reg.estadoOrigem[2] = '\0';
    file = fopen(nomeArq,"rb+");
    if(!file){
        printf("Falha no processamento do arquivo.");
        return ERRO;
    }
    c = leCabecalho(file);
    char status = '0';
    char remover = '*';
    int RRN = 0;
    char verifica;
    if(c.status == '0'){
        printf("Falha no processamento do arquivo.");
        return ERRO;
    }
    fwrite(&status,1,1,file);
    fseek(file,(RRN*TAMREGISTRO)+19,SEEK_SET);
    int contador = 0;
    int limite = c.numeroArestas;
    while(contador < limite){
        fread(&verifica,1,1,file);
        if(verifica == '*'){
            RRN++;
            fseek(file,(RRN*TAMREGISTRO)+19,SEEK_SET);
            continue;
        }
        fseek(file,(RRN*TAMREGISTRO)+19,SEEK_SET);//voltar para o começo do registro
        fread(reg.estadoOrigem,2,1,file);
        fread(reg.estadoDestino,2,1,file);
        fread(&reg.distancia,4,1,file);
        lerAtePipe(reg.cidadeOrigem,file);
        lerAtePipe(reg.cidadeDestino,file);
        lerAtePipe(reg.tempoViagem,file);
        if(!strcmp(campo,"estadoOrigem")){
            if(!strcmp(reg.estadoOrigem,valor)){
                if(opr == 1){
                    printf("%d %s %s %d %s %s %s \n",RRN,reg.estadoOrigem, reg.estadoDestino,reg.distancia,reg.cidadeOrigem,reg.cidadeDestino,reg.tempoViagem);
                }else{
                    fseek(file,(RRN*TAMREGISTRO)+19,SEEK_SET);
                    fwrite(&remover,1,1,file);
                    c.numeroArestas--;
                }
            }
        }else if(!strcmp(campo,"estadoDestino")){
            if(!strcmp(reg.estadoDestino,valor)){
                if(opr == 1){
                    printf("%d %s %s %d %s %s %s \n",RRN,reg.estadoOrigem, reg.estadoDestino,reg.distancia,reg.cidadeOrigem,reg.cidadeDestino,reg.tempoViagem);
                }else{
                    fseek(file,(RRN*TAMREGISTRO)+19,SEEK_SET);
                    fwrite(&remover,1,1,file);
                    c.numeroArestas--;
                }
            }
        }else if(!strcmp(campo,"distancia")){
            if(reg.distancia == dist){
                if(opr == 1){
                    printf("%d %s %s %d %s %s %s \n",RRN,reg.estadoOrigem, reg.estadoDestino,reg.distancia,reg.cidadeOrigem,reg.cidadeDestino,reg.tempoViagem);
                }else{
                    fseek(file,(RRN*TAMREGISTRO)+19,SEEK_SET);
                    fwrite(&remover,1,1,file);
                    c.numeroArestas--;
                }
            }
        }else if (!strcmp(campo,"cidadeOrigem")){
            if(!strcmp(reg.cidadeOrigem,valor)){
                if(opr == 1){
                    printf("%s %s %d %s %s %s \n",reg.estadoOrigem, reg.estadoDestino,reg.distancia,reg.cidadeOrigem,reg.cidadeDestino,reg.tempoViagem);
                }else{
                    fseek(file,(RRN*TAMREGISTRO)+19,SEEK_SET);
                    fwrite(&remover,1,1,file);
                    c.numeroArestas--;
                }
            }
        }else if (!strcmp(campo,"cidadeDestino")){
            if(!strcmp(reg.cidadeDestino,valor)){
                if(opr == 1){
                    printf("%s %s %d %s %s %s \n",reg.estadoOrigem, reg.estadoDestino,reg.distancia,reg.cidadeOrigem,reg.cidadeDestino,reg.tempoViagem);
                }else{
                    fseek(file,(RRN*TAMREGISTRO)+19,SEEK_SET);
                    fwrite(&remover,1,1,file);
                    c.numeroArestas--;
                }
            }
        }else if (!strcmp(campo,"tempoViagem")){
            if(!strcmp(reg.tempoViagem,valor)){
                if(opr == 1){
                    printf("%s %s %d %s %s %s \n",reg.estadoOrigem, reg.estadoDestino,reg.distancia,reg.cidadeOrigem,reg.cidadeDestino,reg.tempoViagem);
                }else{
                    fseek(file,(RRN*TAMREGISTRO)+19,SEEK_SET);
                    fwrite(&remover,1,1,file);
                    c.numeroArestas--;
                }
            }
        }else{
            return ERRO;
        }
        RRN++;
        fseek(file,(RRN*TAMREGISTRO)+19,SEEK_SET);
        contador++;
    }
    rewind(file);
    c.status = '1';
    fwrite(&c.status,1,1,file);
    fwrite(&c.numeroVertices,4,1,file);
    fwrite(&c.numeroArestas,4,1,file);
    fwrite(&c.dataUltimaCompactacao,1,1,file);
    fclose(file);
    return 1;
}
int adicionaRegistro(struct Dados dado,char *nomeArq){
    FILE *file;
    file = fopen(nomeArq,"rb+");
    if(!file){
        printf("Falha no processamento do arquivo.");
        return ERRO;
    }
    struct Cabecalho cab;
    cab = leCabecalho(file);
    if(cab.status == '0'){
        printf("Falha no processamento do arquivo.");
        return ERRO;
    }
    char abriu = '0';
    fwrite(&abriu,1,1,file);
    fseek(file,0,SEEK_END);
    insereRegistroBin(dado,file,0);
    fseek(file,0,SEEK_SET);
    cab.numeroArestas++;
    cab.status = '1';
    escreveCabecalho(cab,file);
    fseek(file,5,SEEK_SET);
    fclose(file);
    return 1;
}
int atualizaArestas(char *nomeArq, struct NoCidades **inicio){
    struct Cabecalho c;
    c.status = '0';
    FILE *file;
    file = fopen(nomeArq,"rb+");
    c = leCabecalho(file);
    //verificar status
    fwrite(&c.status,1,1,file);
    int RRN = 0;
    int contador = 0;
    struct Dados registro;
    registro.estadoOrigem[2] = registro.estadoDestino[2] = '\0';
    fseek(file,(contador*TAMREGISTRO)+19,SEEK_SET);
    while(RRN < c.numeroArestas){
        contador++;
        lerRegistro(file,&registro);
        fseek(file,(contador*TAMREGISTRO)+19,SEEK_SET);
        if(registro.estadoOrigem[0] == '*'){;
            continue;
        }
        if(!buscaCidade(registro.cidadeOrigem,*inicio)){
            c.numeroVertices = insereCidade(registro.cidadeOrigem,inicio);
        }
        if(!buscaCidade(registro.cidadeDestino,*inicio)){
            c.numeroVertices = insereCidade(registro.cidadeDestino,inicio);
        }
        RRN++;
    }
    fseek(file,0,SEEK_SET);
    c.status = '1';
    escreveCabecalho(c,file);
    fclose(file);
    return 0;
}
int attRegistroPorRRN(char *nomeArq,int rrn,char *campo, char *valor,int distancia){
    FILE *file;
    file = fopen(nomeArq,"rb+");
    if(!file){
        printf("Falha no processamento do arquivo.");
        return ERRO;
    }
    struct Cabecalho ca;
    ca = leCabecalho(file);
    if(ca.status == '0'){
        printf("Falha no processamento do arquivo.");
        return ERRO;
    }
    if(rrn > ca.numeroArestas-1){
        return 1;
    }
    ca.status = '0';
    fwrite(&ca.status,1,1,file);
    struct Dados reg;
    reg.estadoOrigem[2] = reg.estadoDestino[2] = '\0';
    fseek(file,(rrn*TAMREGISTRO)+19,SEEK_SET);
    lerRegistro(file,&reg);
    fseek(file,(rrn*TAMREGISTRO)+19,SEEK_SET);
    if(!strcmp(campo,"estadoOrigem")){
        strcpy(reg.estadoOrigem,valor);
    }else if(!strcmp(campo,"estadoDestino")){
        strcpy(reg.estadoDestino,valor);
    }else if(!strcmp(campo,"distancia")){
        reg.distancia = distancia;
    }else if (!strcmp(campo,"cidadeOrigem")){
        strcpy(reg.cidadeOrigem,valor);
    }else if (!strcmp(campo,"cidadeDestino")){
        strcpy(reg.cidadeDestino,valor);
    }else if (!strcmp(campo,"tempoViagem")){
        strcpy(reg.tempoViagem,valor);
    }else{
        return ERRO;
    }
    insereRegistroBin(reg,file,1);
    fseek(file,0,SEEK_SET);
    ca.status = '1';
    fwrite(&ca.status,1,1,file);
    fclose(file);
    return 1;
}
void lerRegistro(FILE *file,struct Dados *r){
    struct Dados registro;
    fread(registro.estadoOrigem,2,1,file);
    fread(registro.estadoDestino,2,1,file);
    fread(&registro.distancia,4,1,file);
    //Campos de tamanhoVariavel ------>Ler ate o delimitador char a char
    lerAtePipe(registro.cidadeOrigem,file);
    lerAtePipe(registro.cidadeDestino,file);
    lerAtePipe(registro.tempoViagem,file);
    *r = registro;
}
void insereRegistroBin(struct Dados dado,FILE *file,int update){
    int a,b,c;
    char lixo = '#';
    char delimitador = '|';
    fwrite(dado.estadoOrigem,2,1,file);//escreve os dois primeiros bytes de dado.estado origem arquivo binario
    fwrite(dado.estadoDestino,2,1,file);
    fwrite(&dado.distancia,sizeof(int),1,file);
    a = strlen(dado.cidadeOrigem);
    b = strlen(dado.cidadeDestino);
    c = strlen(dado.tempoViagem);
    fwrite(dado.cidadeOrigem,a,1,file);
    fwrite(&delimitador,1,1,file);
    fwrite(dado.cidadeDestino,b,1,file);
    fwrite(&delimitador,1,1,file);
    fwrite(dado.tempoViagem,c,1,file);
    fwrite(&delimitador,1,1,file);
    if(update == 0){
        for(int i = 0; i < 77 - (a+b+c+3);i++){//escreve lixo no resto dos espaços livres
            fwrite(&lixo,1,1,file);
        }
    }
}
int comprimirArquivo(char *nomeArq,char *Comprimido){
    FILE *file = fopen(nomeArq,"rb");
    FILE *filecomp = fopen(Comprimido,"wb");
    int rrn = 0;
    int contador = 0;
    struct Dados aux;
    if(!file){
        printf("Falha no carregamento do arquivo.");
        return ERRO;
    }
    struct Cabecalho c;
    c = leCabecalho(file);
    if(c.status == '0'){
        printf("Falha no carregamento do arquivo.");
        return ERRO;
    }
    c.status = '1';
    strcpy(c.dataUltimaCompactacao,"01/11/2019");
    escreveCabecalho(c,filecomp);
    fseek(file,(rrn*TAMREGISTRO)+19,SEEK_SET);
    while(rrn < c.numeroArestas){
        lerRegistro(file,&aux);
        if(aux.estadoOrigem[0] != '*'){
            insereRegistroBin(aux,filecomp,0);
            rrn++;
        }
        contador++;
        fseek(file,(contador*TAMREGISTRO)+19,SEEK_SET);
    }
    fseek(filecomp,0,SEEK_SET);
    c.status ='1';
    fwrite(&c.status,1,1,filecomp);
    fclose(file);
    fclose(filecomp);
    return 1;
}
void escreveCabecalho(struct Cabecalho c,FILE *file){
    fwrite(&c.status,1,1,file);
    fwrite(&c.numeroVertices,sizeof(int),1,file);//OBS sizeof(int) é de 4bits por padrao do GCC
    fwrite(&c.numeroArestas,sizeof(int),1,file);
    fwrite(c.dataUltimaCompactacao,10,1,file);
}