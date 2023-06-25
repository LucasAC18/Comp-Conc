#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_BARBEIROS 2
#define N_CADEIRAS 6
#define N_THREADS N_BARBEIROS + 1
#define TIMESTEP 3
#define TEMPO_CORTE TIMESTEP * 2
#define CHANCE_NOVO_CLIENTE rand() % 100 < 80

typedef struct {
    int clientes[N_CADEIRAS];
    int primeiro;
    int ultimo;
    int sentados;
} Cadeiras;

//Variaveis globais
const char* arquivo_de_log = "log.txt"; // arquivo de log
sem_t sem_mutex;        //semafaro para exclusao mutua da variavel sentados
sem_t sem_clientes;     //semafaro que indica se ha clientes sentados
sem_t sem_barbeiros;    //semafaro que indica se ha barbeiros livres
Cadeiras cadeiras;      //cadeiras de espera

//Aqui, a fila de espera é implementada como um array circular
void sentar(Cadeiras* c, int cliente) {
    if (c->sentados == N_CADEIRAS) {
        printf("Fila cheia, impossível sentar.\n");
        return;
    }
    c->ultimo = (c->ultimo + 1) % N_CADEIRAS;
    c->clientes[c->ultimo] = cliente;
    c->sentados++;
}

int levantar(Cadeiras* c) {
    //if (c->primeiro == c->ultimo + 1) {
    if (c->sentados == 0) {
        printf("Fila vazia, impossível levantar.\n");
        return -1;
    }
    int cliente = c->clientes[c->primeiro];
    c->primeiro = (c->primeiro + 1) % N_CADEIRAS;
    c->sentados--;
    return cliente;
}

void mostrar_fila(Cadeiras* c) {
    if (c->sentados == 0) {
        printf("Fila vazia.\n");
        return;
    }
    printf("Clientes na fila: ");
    int contagem = 0;
    int i = c->primeiro;
    while (contagem < c->sentados) {
        printf("%d ", c->clientes[i]);
        i = (i + 1) % N_CADEIRAS;
        contagem++;
    }
    printf("\n");
}

//Escreve uma mensagem no arquivo de log
void escreve_log(const char* mensagem, ...) {
    FILE* arquivo = fopen(arquivo_de_log, "a");

    if (arquivo != NULL) {
        va_list args;
        va_start(args, mensagem);
        vfprintf(arquivo, mensagem, args);
        va_end(args);
        fclose(arquivo);
    } else {
        printf("Falha ao abrir o arquivo de logs.\n");
    }
}

void corta_cabelo() {
    //espera por um tempo para cortar o cabelo
    printf("Cortando cabelo...\n");
    sleep(TEMPO_CORTE);
    printf("Terminou de cortar\n");   
}

void *barbeiro(void *arg) {
    long tid = (long)arg;

    while (1) {
        sem_wait(&sem_clientes);
        sem_wait(&sem_mutex);
        int cliente_saindo = levantar(&cadeiras);
        sem_post(&sem_mutex);
        printf("Barbeiro %ld atendeu o cliente %d\n", tid, cliente_saindo);
        escreve_log("B %d 1\n", cliente_saindo);
        sem_post(&sem_barbeiros);
        corta_cabelo();
    }

    pthread_exit(NULL);
}

void *cliente(void *arg) {
    long tid = (long)arg;

    sem_wait(&sem_mutex);
    if (cadeiras.sentados < N_CADEIRAS) {
        sentar(&cadeiras, tid);
        printf("Cliente %ld sentou-se\n", tid);
        escreve_log("C %ld 1\n", tid);
        mostrar_fila(&cadeiras);
        sem_post(&sem_clientes);
        sem_post(&sem_mutex);
        sem_wait(&sem_barbeiros);
    } else {
        sem_post(&sem_mutex);
        printf("Cliente %ld foi embora\n", tid);
        escreve_log("C %ld 0\n", tid);
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
        sleep(TIMESTEP/2);
    }
    pthread_exit(NULL);
}

int main() {
    //remove o arquivo de log e cria um novo vazio
    remove(arquivo_de_log);
    FILE* arquivo = fopen(arquivo_de_log, "w");
    fclose(arquivo);

    //inicializa as cadeiras
    cadeiras.primeiro = 0;
    cadeiras.ultimo = -1;
    cadeiras.sentados = 0;

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
