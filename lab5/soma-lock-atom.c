/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Comunicação entre threads usando variável compartilhada e exclusao mutua com bloqueio */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define QNT_MULTIPLOS 20 //Define que queremos 20 
#define MULTIPLO 10 //Define o número o qual queremos imprimir os múltiplos

long int soma = 0; //variavel compartilhada entre as threads
pthread_mutex_t mutex,mutex_parada; //variavel de lock para exclusao mutua
pthread_cond_t cond; //variavel condicional para que "extra" libere "ExecutaTarefa"
pthread_cond_t cond_2;  //variavel condicional para que "ExecutaTarefa" libere "extra"
int contador,parada; //Contadores
int nthreads; //qtde de threads (passada linha de comando)

//funcao executada pelas threads
void *ExecutaTarefa (void *arg) {
    long int id = (long int) arg; //Identificador da thread
    printf("Thread : %ld esta executando...\n", id);

    for (int i=0; i<100000; i++) {
        //--entrada na SC
        pthread_mutex_lock(&mutex);
        //--SC (seção critica)
        if(contador<QNT_MULTIPLOS){
        if(!(soma%MULTIPLO)){ //Caso o contador seja menor que o número de multipos e a soma esteja em um múltiplo, para
            pthread_cond_wait(&cond,&mutex); //Espera que extra imprima o próximo múltiplo para continuar
            pthread_cond_signal(&cond_2); //Libera extra para execução
        }
        }
        soma++; //Incrementa soma
        //--saida da SC
        pthread_mutex_unlock(&mutex);
    }
    printf("Thread : %ld terminou!\n", id);

    pthread_mutex_lock(&mutex_parada); //Seção crítica com outra variável mutex para que em casa de muitas threads não aja sobreescrita em parada
    //Caso alguém queira rodar com 10000 threads
    parada++; //Incrementa a condição de parada de extra
    pthread_mutex_unlock(&mutex_parada); //Fim da seção crítica após incremento de parada
    pthread_exit(NULL);
}

//funcao executada pela thread de log
void *extra (void *args) {
    printf("Extra : esta executando...\n");
    while(contador<QNT_MULTIPLOS){ //Enquanto todos o múltiplos não forem impresso
        //--entrada na SC 
        pthread_mutex_lock(&mutex);
        //--SC (seção critica)
        if (!(soma%MULTIPLO)){//Imprime se 'soma' for multiplo
        printf("soma = %ld \n", soma);
        contador++; //Incrementa contador condicional
        pthread_cond_signal(&cond); //Libera execução de ExecutaTarefa
        pthread_cond_wait(&cond_2,&mutex); //Espera que ExecutaTarefa encontre outro múltipo para continuar
        pthread_mutex_unlock(&mutex);
        //--saida da SC
        }
    }
    while(parada<nthreads){ //Enquanto ainda houverem threads trabalhando
        pthread_cond_broadcast(&cond); //Sinaliza para que elas não sejam bloqueadas
    }
    printf("Extra : terminou!\n");
    pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char *argv[]) {
    /*Não adicionarei comentários pois a função main não sofreu alteração (apenas os destroy ao fim do código)*/
    
    pthread_t *tid; //identificadores das threads no sistema

    //--le e avalia os parametros de entrada
    if(argc<2) {
        printf("Digite: %s <numero de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]);

    //--aloca as estruturas
    tid = (pthread_t*) malloc(sizeof(pthread_t)*(nthreads+1));
    if(tid==NULL) {puts("ERRO--malloc"); return 2;}

    //--inicilaiza o mutex (lock de exclusao mutua)
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond,NULL);
    pthread_cond_init(&cond_2,NULL);

    //--cria as threads
    for(long int t=0; t<nthreads; t++) {
        if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
        printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }

    //--cria thread de log
    if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
        printf("--ERRO: pthread_create()\n"); exit(-1);
    }

    //--espera todas as threads terminarem
    for (int t=0; t<nthreads+1; t++) {
        if (pthread_join(tid[t], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1); 
        } 
    } 

    //--finaliza o mutex
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    pthread_cond_destroy(&cond_2);

    printf("Valor de 'soma' = %ld\n", soma);

    return 0;
}
