//Referências: https://en.wikipedia.org/wiki/Sleeping_barber_problem
//             https://github.com/Stolichnayer/sleeping_barber/blob/master/sleeping_barber.c

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

#define N_BARBEIROS 1
#define N_CADEIRAS 4
#define N_THREADS N_BARBEIROS + 1
#define TIMESTEP 3
#define CHANCE_NOVO_CLIENTE rand() % 100 < 80

//Variaveis globais
sem_t sem_mutex;        //semafaro para exclusao mutua da variavel sentados
sem_t sem_clientes;     //semafaro que indica se ha clientes sentados
sem_t sem_barbeiros;    //semafaro que indica se ha barbeiros livres
int sentados = 0;

void cortaCabelo() {
    //espera por um tempo para cortar o cabelo
    sleep(TIMESTEP*2);
    printf("Cortou cabelo do cliente\n");
}

void *barbeiro(void *arg) {
    long tid = (long)arg;

    while (1) {
        sem_wait(&sem_clientes);
        sem_wait(&sem_mutex);
        sentados--;
        sem_post(&sem_mutex);
        printf("Barbeiro %ld atendeu um cliente sentado\n", tid);
        sem_post(&sem_barbeiros);
        cortaCabelo();
    }

    pthread_exit(NULL);
}

void *cliente(void *arg) {
    long tid = (long)arg;

    sem_wait(&sem_mutex);
    if (sentados < N_CADEIRAS) {
        sentados++;
        printf("Cliente %ld sentou-se\n", tid);
        sem_post(&sem_clientes);
        sem_post(&sem_mutex);
        sem_wait(&sem_barbeiros);
    } else {
        sem_post(&sem_mutex);
        printf("Cliente %ld foi embora\n", tid);
    }

    pthread_exit(NULL);
}

//A cada timestep, cria um novo cliente
void *cria_cliente(void *arg) {
    long counter = 0;

    while (1) {
        //cria um novo cliente com CHANCE_NOVO_CLIENTE de chance
        if (CHANCE_NOVO_CLIENTE) {
            pthread_t thread;
            pthread_create(&thread, NULL, cliente, (void *)counter);
            counter++;
        }

        //espera por um timestep
        sleep(TIMESTEP);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[N_THREADS];

    //inicializa os semaforos
    sem_init(&sem_mutex, 0, 1);
    sem_init(&sem_barbeiros, 0, 0); 
    sem_init(&sem_clientes, 0, 0);

    //dispara as threads barbeiros
    for (long i = 0; i < N_BARBEIROS; i++) {
        pthread_create(&threads[i], NULL, barbeiro, (void *)i);
    }

    //dispara as threads clientes
    pthread_create(&threads[N_THREADS], NULL, cria_cliente, NULL);

    //espera todas as threads terminarem
    for (long i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}  
