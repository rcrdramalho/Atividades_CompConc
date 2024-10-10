#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 1000 // Tamanho do buffer
#define MAX_CHARS 1000 // Tamanho máximo do buffer2

// Variáveis globais
char buffer1[N]; // Buffer para a thread 1
char buffer2[MAX_CHARS]; // Buffer para a thread 2
int count1 = 0; // Contador de caracteres lidos na thread 1
int count2 = 0; // Contador de caracteres processados na thread 2
sem_t sem1, sem2; // Semáforos para coordenação

// Função para calcular o comprimento da string
int str_len(const char *str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// Função executada pela thread 1
void *t1(void *arg) {
    FILE *descritorArquivo = (FILE *)arg;
    while (fgets(buffer1, N, descritorArquivo) != NULL) {
        for (int i = 0; buffer1[i] != '\0'; i++) {
            count1++;
        }
        sem_post(&sem1); // Notifica a thread 2
    }
    sem_post(&sem1); // Sinaliza que não há mais dados
    pthread_exit(NULL);
}

// Função executada pela thread 2
void *t2(void *arg) {
    
    sem_wait(&sem1); // Espera por dados da thread 1
    int prox = 1;
    int lidos = 0;
    int flag = 0;
    for (int i = 0; i < count1; i++) {
        buffer2[count2] = buffer1[i];
        lidos++;
        if (lidos == prox) {
            printf("caracter %c prox=%i lidos=%i i=%i\n", buffer2[count2], prox, lidos, i);
            count2++;
            buffer2[count2] = '\n'; // Adiciona nova linha
            prox = prox+2;
            if(prox > 21) flag = 1;
            if(flag){
                prox=10;
            }
            lidos=0;
        }
        count2++;
    }
    sem_post(&sem2); // Notifica a thread 3
    
    buffer2[count2] = '\0'; // Termina a string
    sem_post(&sem2); // Sinaliza que não há mais dados
    pthread_exit(NULL);
}

// Função executada pela thread 3
void *t3(void *arg) {
    while (1) {
        sem_wait(&sem2); // Espera por dados da thread 2
        if (count2 == 0) break; // Verifica se não há mais dados
        for (int i = 0; i < count2; i++) {
            putchar(buffer2[i]); // Imprime os dados
        }
        count2 = 0; // Reseta o contador após imprimir
    }
    pthread_exit(NULL);
}

// Função principal
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    // Inicializa os semáforos
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 0);

    // Abre o arquivo para leitura
    FILE *descritorArquivo = fopen(argv[1], "r");
    if (!descritorArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 2;
    }

    pthread_t tid[3];

    // Cria as três threads
    pthread_create(&tid[0], NULL, t1, (void *)descritorArquivo);
    pthread_create(&tid[1], NULL, t2, NULL);
    pthread_create(&tid[2], NULL, t3, NULL);

    // Espera todas as threads terminarem
    for (int t = 0; t < 3; t++) {
        pthread_join(tid[t], NULL);
    }

    // Fecha o arquivo
    fclose(descritorArquivo);
    
    // Destrói os semáforos
    sem_destroy(&sem1);
    sem_destroy(&sem2);

    return 0;
}
