//Referências: https://en.wikipedia.org/wiki/Sleeping_barber_problem

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N_CLIENTES 20
#define N_BARBEIROS 1
#define N_CADEIRAS 4
#define N_THREADS N_CLIENTES + N_BARBEIROS

//Variaveis globais
sem_t sem_mutex;        //semafaro para exclusao mutua da variavel sentados
sem_t sem_clientes;     //semafaro que indica se ha clientes sentados
sem_t sem_barbeiros;    //semafaro que indica se ha barbeiros livres
int sentados = 0;

void cortaCabelo() {
    sleep(1);
    printf("Cortou cabelo do cliente\n");
}

void *barbeiro(void *arg) {
    while (1) {
        sem_wait(&sem_clientes);
        sem_wait(&sem_mutex);
        sentados--;
        printf("Barbeiro atendeu um cliente sentado\n");
        sem_post(&sem_barbeiros);
        sem_post(&sem_mutex);
        cortaCabelo();
    }
}

void *cliente(void *arg) {
    sem_wait(&sem_mutex);
    if (sentados < N_CADEIRAS) {
        sentados++;
        printf("Cliente sentou-se\n");
        sem_post(&sem_clientes);
        sem_post(&sem_mutex);
        sem_wait(&sem_barbeiros);
    } else {
        sem_post(&sem_mutex);
        printf("Cliente foi embora\n");
    }
}

int main() {
    pthread_t threads[N_THREADS];

    //inicializa os semaforos
    sem_init(&sem_mutex, 0, 1);
    sem_init(&sem_barbeiros, 0, 0); 
    sem_init(&sem_clientes, 0, 0);

    //dispara as threads barbeiros
    for (int i = N_CLIENTES; i < N_THREADS; i++) {
        pthread_create(&threads[i], NULL, barbeiro, NULL);
    }

    //dispara as threads clientes
    for (int i = 0; i < N_CLIENTES; i++) {
        pthread_create(&threads[i], NULL, cliente, NULL);
    }

    //espera todas as threads terminarem
    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}  