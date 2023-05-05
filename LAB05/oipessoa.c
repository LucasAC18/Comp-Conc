/* Disciplina: Computacao Concorrente */
/* Aluno: Lucas Araujo Carvalho - 119187632 */
/* Laboratorio: 5 */
/* Atividade: 4 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS  5

/* Variaveis globais */
int cumprimentadas = 0, sentados = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond1;
pthread_cond_t x_cond2;

/* Thread A: imprime Oi <pessoa>! */
void *A (void *string) {
  char *nome = (char *)string;

  printf("Oi %s!\n", nome);

  pthread_mutex_lock(&x_mutex);
  cumprimentadas++;
  if (cumprimentadas==2) {
      pthread_cond_signal(&x_cond1);
  }
  pthread_mutex_unlock(&x_mutex);

  pthread_exit(NULL);
}

/* Thread B: imprime "Sentem-se por favor." quando todas as pessoas tiverem sido cumprimentadas */
void *B (void *t) {

  pthread_mutex_lock(&x_mutex);
  if (cumprimentadas < 2) { 
     pthread_cond_wait(&x_cond1, &x_mutex);
  }
  sentados++;
  pthread_mutex_unlock(&x_mutex);

  printf("Sentem-se por favor.\n");
  pthread_cond_broadcast(&x_cond2);

  pthread_exit(NULL);
}

/* Thread C: imprime "Até mais <pessoa>!" após o pedido para todos se sentarem */
void *C (void *string) {
  char *nome = (char *)string;

  pthread_mutex_lock(&x_mutex);
  if(sentados == 0) {
     pthread_cond_wait(&x_cond2, &x_mutex);
  }
  pthread_mutex_unlock(&x_mutex); 

  printf("Até mais %s!\n", nome);

  pthread_exit(NULL);
}

/* Funcao principal */
int main(int argc, char *argv[]) {
  int i; 
  pthread_t threads[NTHREADS];
  char *nome1 = "João";
  char *nome2 = "Maria";

  /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
  pthread_mutex_init(&x_mutex, NULL);
  pthread_cond_init (&x_cond1, NULL);
  pthread_cond_init (&x_cond2, NULL);

  /* Cria as threads */
  pthread_create(&threads[0], NULL, A, (void *)nome1);
  pthread_create(&threads[1], NULL, A, (void *)nome2);
  pthread_create(&threads[2], NULL, B, NULL);
  pthread_create(&threads[3], NULL, C, (void *)nome1);
  pthread_create(&threads[4], NULL, C, (void *)nome2);

  /* Espera todas as threads completarem */
  for (i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  printf ("\nFIM\n");

  /* Desaloca variaveis e termina */
  pthread_mutex_destroy(&x_mutex);
  pthread_cond_destroy(&x_cond1);
  pthread_cond_destroy(&x_cond2);
}
