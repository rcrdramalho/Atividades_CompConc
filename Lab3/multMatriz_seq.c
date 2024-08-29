#include<stdio.h>
#include<stdlib.h>

//#define PRINTA

int main(int argc, char*argv[]) {
    float *matriz1, *matriz2; //matriz que será carregada do arquivo
    int linhas, colunas; //dimensoes da matriz
    long long int tam; //qtde de elementos na matriz
    FILE * descritorArquivo; //descritor do arquivo de entrada
    size_t ret; //retorno da funcao de leitura no arquivo de entrada
   
    //recebe os argumentos de entrada
    if(argc < 2) {
      fprintf(stderr, "Digite: %s <arquivo entrada>\n", argv[0]);
      return 1;
    }
   
    //abre o arquivo para leitura binaria
    descritorArquivo = fopen(argv[1], "rb");
    if(!descritorArquivo) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 2;
    }

    //le as dimensoes da matriz
    ret = fread(&linhas, sizeof(int), 1, descritorArquivo);
    if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
    }
    ret = fread(&colunas, sizeof(int), 1, descritorArquivo);
    if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
    }
    tam = linhas * colunas; //calcula a qtde de elementos da matriz

    //aloca memoria para a matriz
    matriz1 = (float*) malloc(sizeof(float) * tam);
    if(!matriz1) {
      fprintf(stderr, "Erro de alocao da memoria da matriz\n");
      return 3;
    }
    matriz2 = (float*) malloc(sizeof(float) * tam);
    if(!matriz2) {
      fprintf(stderr, "Erro de alocao da memoria da matriz\n");
      return 3;
    }

    //carrega a matriz de elementos do tipo float do arquivo
    ret = fread(matriz1, sizeof(float), tam, descritorArquivo);
    if(ret < tam) {
      fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
      return 4;
    }
    ret = fread(matriz2, sizeof(float), tam, descritorArquivo);
    if(ret < tam) {
      fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
      return 4;
    }

    //imprime a matriz na saida padrao

    #ifdef PRINTA
    printf("Matriz 1:\n");
    for(int i=0; i<linhas; i++) { 
      for(int j=0; j<colunas; j++)
        fprintf(stdout, "%.6f ", matriz1[i*colunas+j]);
      fprintf(stdout, "\n");
    }
    printf("Matriz 2:\n");
    for(int i=0; i<colunas; i++) { 
      for(int j=0; j<linhas; j++)
        fprintf(stdout, "%.6f ", matriz2[i*colunas+j]);
      fprintf(stdout, "\n");
    }
    #endif
    float soma;
    for(int i=0;i<tam;i++){
      for(int j=0;j<tam;j++){
        soma = 0.0;
        for(int k=0;k<tam;k++){
          soma += matriz1[i][k] * matriz2[k][j];
        }
        
      }   
    }

    //finaliza o uso das variaveis
    fclose(descritorArquivo);
    free(matriz1);
    return 0;
}

