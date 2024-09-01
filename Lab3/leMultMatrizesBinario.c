/* Programa que le um arquivo binario com dois valores inteiros (indicando as dimensoes de uma matriz) 
 * e uma sequencia com os valores da matriz (em float)
 * Entrada: nome do arquivo de entrada
 * Saida: valores da matriz escritos no formato texto (com 6 casas decimais) na saida padrao 
 * */

#include<stdio.h>
#include<stdlib.h>

int main(int argc, char*argv[]) {
   float *matriz1, *matriz2, *matriz3; //matrizes que seram carregada do arquivo
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
      fprintf(stderr, "Erro de alocao da memoria da matriz1\n");
      return 3;
   }

   matriz2 = (float*) malloc(sizeof(float) * tam);
   if(!matriz2) {
      fprintf(stderr, "Erro de alocao da memoria da matriz2\n");
      return 3;
   }

   matriz3 = (float*) malloc(sizeof(float) * tam);
   if(!matriz3) {
      fprintf(stderr, "Erro de alocao da memoria da matriz3\n");
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

   ret = fread(matriz3, sizeof(float), tam, descritorArquivo);
   if(ret < tam) {
      fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
      return 4;
   }

   //imprime a matriz na saida padrao
   printf("Matriz 1 (%dx%d):\n", linhas, colunas);
   for (int i = 0; i < linhas; i++) {
      for (int j = 0; j < colunas; j++)
         printf("%.6f ", matriz1[i * colunas + j]);
      printf("\n");
   }

   printf("Matriz 2 (%dx%d):\n", colunas, linhas);
   for (int i = 0; i < colunas; i++) {
      for (int j = 0; j < linhas; j++)
         printf("%.6f ", matriz2[i * linhas + j]);
      printf("\n");
   }

   printf("Matriz Resultante (%dx%d):\n", linhas, linhas);
   for (int i = 0; i < linhas; i++) {
      for (int j = 0; j < linhas; j++)
         printf("%.6f ", matriz3[i * linhas + j]);
      printf("\n");
   }

   //finaliza o uso das variaveis
   fclose(descritorArquivo);
   free(matriz1);
   free(matriz2);
   free(matriz3);
   return 0;
}

