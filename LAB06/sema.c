/* Disciplina: Programacao Concorrente */
/* Prof.: Lucas Araujo Carvalho */
/* Codigo: Comunicação entre threads usando variável compartilhada e sincronização com semáforos */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define N        10
#define PTHREADS 2
#define CTHREADS 4
#define NTHREADS PTHREADS+CTHREADS

// Variaveis globais
sem_t slotCheio;
sem_t slotVazio;
sem_t mutexProd;
sem_t mutexCons;
int buffer[N];

void insere(int tid) {
    // printf("Produtora[%d]: quer inserir.\n", tid);
    sem_wait(&slotVazio);
    // printf("Produtora[%d]: achou posicao vazia no buffer.\n", tid);
    sem_wait(&mutexProd);
    for (int i = 0; i < N; i++) {
      buffer[i] = tid*i;
    }
    printf("Produtora[%d]: inseriu itens no buffer.\n", tid);
    sem_post(&mutexProd);
    for (int i = 0; i < N; i++) sem_post(&slotCheio);
}

int retira(int tid) {
    int item;
    static int out = 0;
    // printf("Consumidora[%d]: quer retirar.\n", tid);
    sem_wait(&slotCheio);
    // printf("Consumidora[%d]: achou posicao cheia no buffer.\n", tid);
    sem_wait(&mutexCons);
    item = buffer[out];
    printf("Consumidora[%d]: retirou %d da posicao %d.\n", tid, item, out);
    out = (out + 1) % N;
    if (out == 0) sem_post(&slotVazio); //se out = 0, consumidoras percorreram todo o buffer
    sem_post(&mutexCons);
    return item;
}

void *consumidor (void *threadid) {
  int item;
  int *tid = (int*) threadid;
  printf("Thread : %d (consumidora) comecou a executar.\n", *tid);
  while(1) {
    item = retira(*tid);
    sleep(1);
  }
  free(threadid);
  pthread_exit(NULL);
}

void *produtor (void *threadid) {
  int *tid = (int *) threadid;
  printf("Thread : %d (produtora) comecou a executar.\n", *tid);
  while(1) {
    insere(*tid);
    sleep(1);
  }
  free(threadid);
  pthread_exit(NULL);
}

//funcao principal
int main(int argc, char *argv[]) {
  pthread_t threads[NTHREADS];
  int *id[NTHREADS]; 

  //inicia os semaforo usados para exclusão mútua e sincronização condicional
  sem_init(&mutexProd, 0, 1); //semaforo binario que garante exclusao mutua para os produtores
  sem_init(&mutexCons, 0, 1); //semaforo binario que garante exclusao mutua para os consumidores
  sem_init(&slotCheio, 0, 0); //semaforo contador que indica o numero de slots cheios
  sem_init(&slotVazio, 0, 1); //semaforo binario que indica quando todos os slots estao vazios

  //inicializa os ids das threads
  for (int t=0; t<NTHREADS; t++) {
    if ((id[t] = malloc(sizeof(int))) == NULL) {
       pthread_exit(NULL); return 1;
    }
    *id[t] = t+1;
  }

  //cria as threads consumidoras
  for(int i=0; i<CTHREADS; i++) {
    if (pthread_create(&threads[i], NULL, consumidor, (void *)id[i])) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }

  //cria as threads produtoras
  for(int i=CTHREADS; i<CTHREADS+PTHREADS; i++) {
    if (pthread_create(&threads[i], NULL, produtor, (void *)id[i])) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }

  //espera todas as threads terminarem
  for (int t=0; t<NTHREADS; t++) {
    if (pthread_join(threads[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    }
  }

  pthread_exit(NULL);
}