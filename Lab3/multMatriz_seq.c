#include <stdio.h>
#include <stdlib.h>
#include "timer.h" //Tive que compilar o código com -lrt para que funcionasse


int main(int argc, char* argv[]) {
    float *matriz1, *matriz2, *matriz3; // Matrizes
    int linhas, colunas; // Dimensões das matrizes
    long long int tam; // Quantidade de elementos nas matrizes
    FILE *descritorArquivo; // Descritor do arquivo de entrada
    size_t ret; // Retorno da função de leitura no arquivo de entrada
    double inicio, fim, delta; // Contadores

    // Recebe os argumentos de entrada
    if (argc < 2) {
        fprintf(stderr, "Digite: %s <arquivo entrada> <arquivo saida>\n", argv[0]);
        return 1;
    }

    // Inicia o contador para registrar tempo de execução do programa
    GET_TIME(inicio);
    // Abre o arquivo para ler as matrizes
    descritorArquivo = fopen(argv[1], "rb");
    if (!descritorArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 2;
    }

    // Lê as dimensões das matrizes
    ret = fread(&linhas, sizeof(int), 1, descritorArquivo);
    if (!ret) {
        fprintf(stderr, "Erro de leitura das dimensões da matriz 1 do arquivo\n");
        return 3;
    }
    ret = fread(&colunas, sizeof(int), 1, descritorArquivo);
    if (!ret) {
        fprintf(stderr, "Erro de leitura das dimensões da matriz 1 do arquivo\n");
        return 3;
    }

    // Calcula o tamanho das matrizes
    tam = linhas * colunas;

    // Aloca memória para as matrizes
    matriz1 = (float*) malloc(sizeof(float) * tam);
    if (!matriz1) {
        fprintf(stderr, "Erro de alocação da memória da matriz 1\n");
        return 4;
    }
    matriz2 = (float*) malloc(sizeof(float) * tam);
    if (!matriz2) {
        fprintf(stderr, "Erro de alocação da memória da matriz 2\n");
        return 4;
    }
    matriz3 = (float*) malloc(sizeof(float) * tam);
    if (!matriz3) {
        fprintf(stderr, "Erro de alocação da memória da matriz 3\n");
        return 4;
    }

    // Carrega as matrizes do arquivo
    ret = fread(matriz1, sizeof(float), tam, descritorArquivo);
    if (ret < tam) {
        fprintf(stderr, "Erro de leitura dos elementos da matriz 1\n");
        return 5;
    }
    ret = fread(matriz2, sizeof(float), tam, descritorArquivo);
    if (ret < tam) {
        fprintf(stderr, "Erro de leitura dos elementos da matriz 2\n");
        return 5;
    }

    // Multiplica as matrizes
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < linhas; j++) {
            float soma = 0.0;
            for (int k = 0; k < colunas; k++) {
                soma += matriz1[i * colunas + k] * matriz2[k * linhas + j];
            }
            matriz3[i * linhas + j] = soma;
        }
    }

    // Cria arquivo de escrita para escrever as 3 matrizes
    FILE *descritorArquivo2;
    descritorArquivo2 = fopen(argv[2], "wb");
    ret = fwrite(&linhas, sizeof(int), 1, descritorArquivo2);
    ret = fwrite(&colunas, sizeof(int), 1, descritorArquivo2);
    ret = fwrite(matriz1, sizeof(float), tam, descritorArquivo2);
    if(ret < tam) {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      return 4;
     }
    ret = fwrite(matriz2, sizeof(float), tam, descritorArquivo2);
    if(ret < tam) {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      return 4;
    }
    ret = fwrite(matriz3, sizeof(float), tam, descritorArquivo2);
    if(ret < tam) {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      return 4;
    }

    // Finaliza o uso das variáveis
    free(matriz1);
    free(matriz2);
    free(matriz3);
    fclose(descritorArquivo);
    fclose(descritorArquivo2);

    // Finaliza o contador
    GET_TIME(fim);
    delta = fim-inicio;
    printf("Tempo de execução: %.26f segundos.\n", delta);
    return 0;
}
