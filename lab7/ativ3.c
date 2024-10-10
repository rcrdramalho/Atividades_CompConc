#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 1000 // Tamanho do buffer
#define MAX_CHARS 1000 // Tamanho máximo do buffer2

/*Código implementado com base no código semaf2.c da aula*/

//variáveis globais
char buffer1[N]; //buffer para a thread 1
char buffer2[MAX_CHARS]; //buffer para a thread 2
int count1 = 0; //contador de caracteres lidos na thread 1
int count2 = 0; //contador de caracteres processados na thread 2
sem_t sem1, sem2; //semáforos para coordenação

//função para calcular o comprimento da string
int str_len(const char *str) { //incluí para não usar a biblioteca string.h
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

//função executada pela thread 1
void *t1(void *arg) {
    FILE *descritorArquivo = (FILE *)arg; //lê o arquivo de entrada
    while (fgets(buffer1, N, descritorArquivo) != NULL) {
        for (int i = 0; buffer1[i] != '\0'; i++) {
            count1++; //conta quantos caracters foram inseridos no buffer1
        }
    }
    sem_post(&sem1); //envia o  sinal para a thread 2 indicando que todos os caracteres foram lidos
    pthread_exit(NULL);
}

//função executada pela thread 2
void *t2(void *arg) {
    
    sem_wait(&sem1); // Espera por dados da thread 1
    int prox = 1; //proximo posicao a ser inserido o \n
    int lidos = 0; //quantidade de caracteres lidos apos o ultimo \n
    int flag = 0; //flag para saber quando chegou o limite de caracteres
    for (int i = 0; i < count1; i++) {
        buffer2[count2] = buffer1[i]; //copia os dados do buffer de entrada no buffer de saida
        lidos++; //lido
        if (lidos == prox) {
            count2++;
            buffer2[count2] = '\n'; //adiciona nova linha
            prox = prox+2; //atualiza a proxima posicao a ser inserido o \n
            if(prox > 21) flag = 1; //caso o limite seja alcancado para o crescimento
            if(flag){
                prox=10;
            }
            lidos=0; //reseta a linha de lidos
        }
        count2++;
    }
    
    buffer2[count2] = '\0'; //termina a string
    sem_post(&sem2); //sinaliza que não há mais dados
    pthread_exit(NULL);
}

//função executada pela thread 3
void *t3(void *arg) {
    while (1) {
        sem_wait(&sem2); //espera por dados da thread 2
        if (count2 == 0) break; //verifica se não há mais dados
        for (int i = 0; i < count2; i++) {
            putchar(buffer2[i]); //imprime os dados
        }
        count2 = 0; //reseta o contador após imprimir
    }
    pthread_exit(NULL);
}

//função principal
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    //inicializa os semaforos
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 0);

    //abre o arquivo para leitura
    FILE *descritorArquivo = fopen(argv[1], "r");
    if (!descritorArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 2;
    }

    pthread_t tid[3];

    //cria as tres threads
    pthread_create(&tid[0], NULL, t1, (void *)descritorArquivo);
    pthread_create(&tid[1], NULL, t2, NULL);
    pthread_create(&tid[2], NULL, t3, NULL);

    //espera todas as threads terminarem
    for (int t = 0; t < 3; t++) {
        pthread_join(tid[t], NULL);
    }

    //fecha o arquivo
    fclose(descritorArquivo);
    
    //destroi os semaforos
    sem_destroy(&sem1);
    sem_destroy(&sem2);

    return 0;
}
