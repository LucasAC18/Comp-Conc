#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#define THREADCOUNT 2
#define ARRAYSIZE 10000

int arr[ARRAYSIZE];

//Thread 0 percorre as posições pares e 1, as ímpares.
void *multiplica_dois(void *thread_id) {
  for (long i = (long) thread_id; i < ARRAYSIZE; i += 2) {
    arr[i] *= 2;
  }
  pthread_exit(NULL);
}

int main() {
  pthread_t threads[THREADCOUNT];
  bool estaCorreto = true;

  //Inicialização do vetor. O valor em cada posição é seu próprio índice.
  for (int i = 0; i < ARRAYSIZE; i++) {
    arr[i] = i;
  }

  //Cria as duas threads. Quando criadas elas executam a função "multiplica_dois" com seu índice i como argumento.
  for (long i = 0; i < THREADCOUNT; i++) {
    pthread_create(&threads[i], NULL, multiplica_dois, (void *) i);
  }

  //O fluxo da main espera ambas as threads finalizarem sua execução. 
  for (int i = 0; i < THREADCOUNT; i++) {
    pthread_join(threads[i], NULL);
  }

  //Verifica se o vetor foi de fato multiplicado por 2.
  for (int i = 0; i < ARRAYSIZE; i++) {
    if (arr[i] != i*2) {
      estaCorreto = false;
    } 
  }

  //Diz se o vetor foi multiplicado corretamente ou não
  if (estaCorreto) {
    printf("Multiplicou corretamente!\n");
  } else {
    printf("Vetor final está incorreto\n");
  }

  return 0;
}