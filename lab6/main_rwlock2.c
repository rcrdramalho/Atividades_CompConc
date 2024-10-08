#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list_int.h"
#include "timer.h"

/*Serão comentadas apenas alterações no código pois a main_rwlock já foi analisada em aula*/

#define QTDE_OPS 10000000  
#define QTDE_INI 100       
#define MAX_VALUE 100      

// Lista compartilhada inciada
struct list_node_s* head_p = NULL;  
//qtde de threads no programa
int nthreads;  

// rwlock de exclusão mútua
// pthread_rwlock_t rwlock;  
// rwlock nativo do pthread comentado porque faremos uma implementação própria

// Controle de leitura, fila de escrita e bool para escrita sendo executada
int readers = 0,writers_fila = 0,writers_exe = 0;

// Mutexes e condições
pthread_mutex_t mutex;      // Mutex para exclusão mútua
pthread_cond_t cond_read;   // Condição para leitura
pthread_cond_t cond_write;  // Condição para escrita

// Função que insere os escritores na fila de escrita
void inic_write() { 
    pthread_mutex_lock(&mutex);   
    writers_fila++;                  // Incrementa o contador de escritores na fila
    while (readers > 0 || writers_exe > 0) { // Se há leitores ou escritores executando, a thread fica em espera
        pthread_cond_wait(&cond_write, &mutex);  // Aguarda sinal de que pode escrever
    }
    writers_fila--;                  // Escritor sai da fila
    writers_exe = 1;                 // Indica que há um escritor em execução
    pthread_mutex_unlock(&mutex);   
}

// Função que termina a escrita
void fim_write() {
    pthread_mutex_lock(&mutex);
    writers_exe = 0;                 // Marca que não há mais escritor em execução
    if (writers_fila > 0) { // Se há escritores na fila libera um escritor
        pthread_cond_signal(&cond_write);  // Envia sinal para escritor
    } else {
        // Se não há escritores na fila libera todos os leitores
        pthread_cond_broadcast(&cond_read);  // Acorda todos os leitores
    }
    pthread_mutex_unlock(&mutex);
}

// Função que inicia a leitura
void inic_read() {
    pthread_mutex_lock(&mutex);
    while (writers_fila > 0 || writers_exe > 0) { // Caso hajam escritos na fila ou se um escritor está executando bloqueia 
        pthread_cond_wait(&cond_read, &mutex);  // Aguarda sinal de que pode ler
    }
    readers++;                       // Incrementa o contador de leitores
    pthread_mutex_unlock(&mutex); 
}

// Função que termina a leitura
void fim_read() {
    pthread_mutex_lock(&mutex);
    readers--;                       // Decrementa o contador de leitores
    if (readers == 0) { // Se não há mais leitores libera um escritor
        pthread_cond_signal(&cond_write);  // Sinaliza um escritor
    }
    pthread_mutex_unlock(&mutex);  
}

// Função que será executada pelas threads
void* tarefa(void* arg) {
    long int id = (long int) arg;   
    int op, in = 0, out = 0, read = 0;

    //realiza operacoes de consulta (98%), insercao (1%) e remocao (1%)
    for (long int i = id; i < QTDE_OPS; i += nthreads) {
        op = rand() % 100;  
        if (op < 98) {
            inic_read();                         // Inicia a leitura
            printf("Thread %li iniciou leitura.\n", id);
            Member(i % MAX_VALUE, head_p);    
            fim_read();  
            printf("Thread %li terminou leitura.\n", id);// Termina a leitura
            read++;
        } 
        // 1% das operações serão inserções
        else if (op < 99) {
            inic_write();                        // Inicia a escrita
            printf("Thread %li iniciou escrita.\n", id);
            Insert(i % MAX_VALUE, &head_p);      
            fim_write();                         // Termina a escrita
            printf("Thread %li terminou escrita.\n", id);
            in++;
        } 
        // 1% das operações serão remoções
        else {
            inic_write();                        // Inicia a escrita
            printf("Thread %li iniciou escrita.\n", id);
            Delete(i % MAX_VALUE, &head_p);   
            fim_write();                         // Termina a escrita
            printf("Thread %li terminou escrita.\n", id);
            out++;
        }
    }

    printf("Thread %ld: in=%d out=%d read=%d\n", id, in, out, read);
    pthread_exit(NULL);  // Termina a execução da thread
}

int main(int argc, char* argv[]) {
    pthread_t *tid;     
    double ini, fim, delta; 

    //verifica se o numero de threads foi passado na linha de comando
    if (argc < 2) {
        printf("Digite: %s <numero de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]); 

    // Insere os primeiros elementos na lista
    for (int i = 0; i < QTDE_INI; i++) {
        Insert(i % MAX_VALUE, &head_p);
    }

   //aloca espaco de memoria para o vetor de identificadores de threads no sistema
    tid = malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL) { 
        printf("--ERRO: malloc()\n");
        return 2;
    }

    // Inicializa os mutexes e variáveis de condição
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_read, NULL);
    pthread_cond_init(&cond_write, NULL);

    //tomada de tempo inicial
    GET_TIME(ini);  
    
    //cria as threads
    for (long int i = 0; i < nthreads; i++) {
        if (pthread_create(tid + i, NULL, tarefa, (void*) i)) {
            printf("--ERRO: pthread_create()\n");
            return 3;
        }
    }

    //aguarda as threads terminarem
    for (int i = 0; i < nthreads; i++) {
        if (pthread_join(*(tid + i), NULL)) {
            printf("--ERRO: pthread_join()\n");
            return 4;
        }
    }

    //tomada de tempo final
    GET_TIME(fim);  
    delta = fim - ini;
    printf("Tempo: %lf\n", delta);  

    // Destrói os mutexes e variáveis de condição
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_read);
    pthread_cond_destroy(&cond_write);
    //libera o espaco de memoria do vetor em threads
    free(tid);  
    //libera o espaco de memoria da lista
    Free_list(&head_p);  

    return 0;
    /*main*/
}
