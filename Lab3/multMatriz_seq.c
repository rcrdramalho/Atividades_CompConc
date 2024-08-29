#include <stdio.h>
#include <stdlib.h>

#define PRINTA

int main(int argc, char* argv[]) {
    float *matriz1, *matriz2, *matriz3; // Matrizes
    int M, N; // Dimensões das matrizes
    long long int tam1, tam2; // Quantidade de elementos nas matrizes
    FILE *descritorArquivo; // Descritor do arquivo de entrada
    size_t ret; // Retorno da função de leitura no arquivo de entrada

    // Recebe os argumentos de entrada
    if (argc < 2) {
        fprintf(stderr, "Digite: %s <arquivo entrada>\n", argv[0]);
        return 1;
    }

    // Abre o arquivo para leitura binária
    descritorArquivo = fopen(argv[1], "rb");
    if (!descritorArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 2;
    }

    // Lê as dimensões das matrizes
    ret = fread(&M, sizeof(int), 1, descritorArquivo);
    if (!ret) {
        fprintf(stderr, "Erro de leitura das dimensões da matriz 1 do arquivo\n");
        return 3;
    }
    ret = fread(&N, sizeof(int), 1, descritorArquivo);
    if (!ret) {
        fprintf(stderr, "Erro de leitura das dimensões da matriz 1 do arquivo\n");
        return 3;
    }

    // Calcula as dimensões da segunda matriz e a matriz resultante
    int M2 = N; // Como a segunda matriz é NxM
    int N2 = M; // E a primeira matriz é MxN
    tam1 = M * N; // Número de elementos na matriz 1
    tam2 = N * M; // Número de elementos na matriz 2
    long long int tam3 = M * M; // Número de elementos na matriz 3

    // Aloca memória para as matrizes
    matriz1 = (float*) malloc(sizeof(float) * tam1);
    if (!matriz1) {
        fprintf(stderr, "Erro de alocação da memória da matriz 1\n");
        return 4;
    }
    matriz2 = (float*) malloc(sizeof(float) * tam2);
    if (!matriz2) {
        fprintf(stderr, "Erro de alocação da memória da matriz 2\n");
        free(matriz1);
        return 4;
    }
    matriz3 = (float*) malloc(sizeof(float) * tam3);
    if (!matriz3) {
        fprintf(stderr, "Erro de alocação da memória da matriz 3\n");
        free(matriz1);
        free(matriz2);
        return 4;
    }

    // Carrega as matrizes do arquivo
    ret = fread(matriz1, sizeof(float), tam1, descritorArquivo);
    if (ret < tam1) {
        fprintf(stderr, "Erro de leitura dos elementos da matriz 1\n");
        free(matriz1);
        free(matriz2);
        free(matriz3);
        fclose(descritorArquivo);
        return 5;
    }
    ret = fread(matriz2, sizeof(float), tam2, descritorArquivo);
    if (ret < tam2) {
        fprintf(stderr, "Erro de leitura dos elementos da matriz 2\n");
        free(matriz1);
        free(matriz2);
        free(matriz3);
        fclose(descritorArquivo);
        return 5;
    }

    // Fecha o arquivo
    fclose(descritorArquivo);

    // Inicializa a matriz resultante com zeros
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            matriz3[i * M + j] = 0.0;
        }
    }

    // Multiplica as matrizes
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            float soma = 0.0;
            for (int k = 0; k < N; k++) {
                soma += matriz1[i * N + k] * matriz2[k * M + j];
            }
            matriz3[i * M + j] = soma;
        }
    }

    // Imprime as matrizes
    #ifdef PRINTA
    printf("Matriz 1 (%dx%d):\n", M, N);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++)
            printf("%.6f ", matriz1[i * N + j]);
        printf("\n");
    }

    printf("Matriz 2 (%dx%d):\n", N, M);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++)
            printf("%.6f ", matriz2[i * M + j]);
        printf("\n");
    }

    printf("Matriz Resultante (%dx%d):\n", M, M);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++)
            printf("%.6f ", matriz3[i * M + j]);
        printf("\n");
    }
    #endif

    // Finaliza o uso das variáveis
    free(matriz1);
    free(matriz2);
    free(matriz3);

    return 0;
}
