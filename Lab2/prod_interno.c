#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h> 

// Variáveis globais
long int n;
float *vet1, *vet2;
int nthreads;

// Função executada pelas threads
void *ProdutoInterno(void *tid) {
   long int id = (long int) tid; 
   int ini, fim, bloco; 
   float produto_local = 0.0;
   float *ret; 

   bloco = n / nthreads; 
   ini = id * bloco; 
   fim = (id == nthreads - 1) ? n : ini + bloco;

   for(int i = ini; i < fim; i++) {
      produto_local += vet1[i] * vet2[i];
   }

   ret = malloc(sizeof(float));
   if (ret != NULL) *ret = produto_local;
   else printf("--ERRO: malloc() thread\n");

   pthread_exit((void*) ret);
}

int main(int argc, char *argv[]) {
   FILE *arq; 
   size_t ret;
   double prod_ori; 
   float produto_global, *prod_ret_thread, vat_relativa;
   pthread_t *tid_sistema;

   if(argc < 3) { 
      printf("Use: %s <arquivo de entrada> <numero de threads> \n", argv[0]); 
      exit(-1); 
   }

   arq = fopen(argv[1], "rb");
   if(arq == NULL) { 
      printf("--ERRO: fopen()\n"); 
      exit(-1); 
   }

   ret = fread(&n, sizeof(long int), 1, arq);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes do vetor no arquivo \n");
      return 3;
   }

   vet1 = malloc(sizeof(float) * n);
   vet2 = malloc(sizeof(float) * n);
   if(vet1 == NULL || vet2 == NULL) { 
      printf("--ERRO: malloc()\n"); 
      exit(-1); 
   }

   ret = fread(vet1, sizeof(float), n, arq);
   if(ret < n) {
      fprintf(stderr, "Erro de leitura dos elementos do vetor1\n");
      return 4;
   }

   ret = fread(vet2, sizeof(float), n, arq);
   if(ret < n) {
      fprintf(stderr, "Erro de leitura dos elementos do vetor2\n");
      return 4;
   }

   nthreads = atoi(argv[2]);
   if(nthreads > n) nthreads = n;

   tid_sistema = malloc(sizeof(pthread_t) * nthreads);
   if(tid_sistema == NULL) { 
      printf("--ERRO: malloc()\n"); 
      exit(-1); 
   }

   for(long int i = 0; i < nthreads; i++) {
      if (pthread_create(&tid_sistema[i], NULL, ProdutoInterno, (void*) i)) {
         printf("--ERRO: pthread_create()\n"); 
         exit(-1);
      }
   }

   produto_global = 0.0;
   for(int i = 0; i < nthreads; i++) {
      if (pthread_join(tid_sistema[i], (void *) &prod_ret_thread)) {
         printf("--ERRO: pthread_join()\n"); 
         exit(-1);
      }
      produto_global += *prod_ret_thread;
      free(prod_ret_thread);
   }

   ret = fread(&prod_ori, sizeof(double), 1, arq); 
   if (ret != 1) {
      fprintf(stderr, "Erro de leitura do produto esperado\n");
      return 5;
   }

   vat_relativa = ((prod_ori - produto_global) / prod_ori);
   vat_relativa = vat_relativa < 0 ? vat_relativa*-1 : vat_relativa;

   printf("Produto esperado: %.26lf\n", prod_ori);
   printf("Produto obtido:   %.26f\n", produto_global);
   printf("Variacao relativa: %.26f\n", vat_relativa);


   free(vet1);
   free(vet2);
   free(tid_sistema);
   fclose(arq);

   return 0;
}
