#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h" // Tive que compilar o código com -lrt para que funcionasse

float *matriz1, *matriz2, *matriz3; // Variáveis das matrizes
int linhas, colunas, nthreads; // Dimensões das matrizes e quantidade de threads

void *multiplicaMatriz(void *tid) {
    long int id = (long int) tid; // id de cada thread criada
    int ini, fim, bloco; // Variáveis e inicio e parada do código

    bloco = linhas / nthreads; //Tamanho do bloco a ser calculado 
    ini = id * bloco; // Inicio do trabalho da thread
    fim = (id == nthreads - 1) ? linhas : ini + bloco; // Parada

    //Multiplica as matrizes
    for (int i = ini; i < fim; i++) {
        for (int j = 0; j < linhas; j++) {
            float soma = 0.0;
            for (int k = 0; k < colunas; k++) {
                soma += matriz1[i * colunas + k] * matriz2[k * linhas + j];
            }
            matriz3[i * linhas + j] = soma;
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    FILE *descritorArquivo; 
    size_t ret;
    double inicio, fim, delta;

    if (argc < 4) {
        fprintf(stderr, "Digite: %s <arquivo entrada> <arquivo saida> <numero de threads>\n", argv[0]);
        return 1;
    }


    // Inicia o contador
    GET_TIME(inicio);

    // Abre arquivo com as matrizes de entrada
    descritorArquivo = fopen(argv[1], "rb");
    if (!descritorArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 2;
    }

    nthreads = atoi(argv[3]);

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

    // Calcula tamanho das matrizes
    long long int tam = linhas * colunas;

    // Aloca espaço para as matrizes
    matriz1 = (float*) malloc(sizeof(float) * tam);
    matriz2 = (float*) malloc(sizeof(float) * tam);
    matriz3 = (float*) malloc(sizeof(float) * linhas * linhas);
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

    pthread_t *threads = (pthread_t*) malloc(nthreads * sizeof(pthread_t));
    if (threads == NULL) {
        fprintf(stderr, "Erro ao alocar memória para threads\n");
        return 6;
    }

    for (long int i = 0; i < nthreads; i++) {
        if (pthread_create(&threads[i], NULL, multiplicaMatriz, (void*) i)) {
            fprintf(stderr, "Erro na criação da thread %ld\n", i);
            return 7;
        }
    }

    // Inicia threads que calcularão os produtos internos para cada célula
    for (long int i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cria o arquivo de escrita para saída
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

    free(matriz1);
    free(matriz2);
    free(matriz3);
    free(threads);
    fclose(descritorArquivo);
    fclose(descritorArquivo2);

    // Finaliza contador e registra o tempo de execução
    GET_TIME(fim);
    delta = fim-inicio;
    printf("Tempo de execução: %.26f segundos.\n", delta);
    return 0;
}
