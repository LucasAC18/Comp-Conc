#include <stdio.h>

#define N_CADEIRAS 6

int main() {
    const char* arquivo_de_log = "log.txt";
    FILE* arquivo = fopen(arquivo_de_log, "r");
    if (arquivo == NULL) {
        printf("Falha ao abrir o arquivo.\n");
        return 1;
    }

    char letra;
    int numero1, numero2;
    int tot_atendimentos = 0, tot_clientes_sentaram = 0, tot_clientes_sairam = 0;
    int ultimo_cliente = 0, sentados = 0;

    while (fscanf(arquivo, "%c %d %d\n", &letra, &numero1, &numero2) == 3) {
        if (letra == 'B') {
            sentados--;
            int tmp = ultimo_cliente;
            ultimo_cliente = numero1;
            if (tmp > numero1) {
              printf("Atendimentos fora de ordem!\n");
              break;
            }
            tot_atendimentos++;
        } else if (letra == 'C' && numero2 == 1) {
            sentados++;
            if (sentados > N_CADEIRAS) {
                printf("Mais clientes sentados do que cadeiras!\n");
                break;
            }
            tot_clientes_sentaram++;
        } else if (letra == 'C' && numero2 == 0) {
            tot_clientes_sairam++;
        }
    }

    fclose(arquivo);

    printf("Total de clientes atendidos: %d\n", tot_atendimentos);
    printf("Total de clientes que sentaram: %d\n", tot_clientes_sentaram);
    printf("Total de clientes que saíram: %d\n", tot_clientes_sairam);

    return 0;
}
