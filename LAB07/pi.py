#/* Disciplina: Programacao Concorrente */
#/* Aluno: Lucas Araujo Carvalho - 119187632 */
#/* Codigo: calculando pi com threads */  

from threading import Thread
import time
import math

#Variaveis globais
N = 1000000
THREADS = 4
pi_est = 0

class Calculadora(Thread):
    def __init__(self, tid):
        super().__init__()
        self.tid = tid
        self.val = 0

    def run(self):
        for i in range(self.tid,N,THREADS):
            self.val += (-1)**i/(2*i + 1)

threads = [Calculadora(tid) for tid in range(THREADS)]
start = time.time()

for thread in threads:
    thread.start()
    # thread.run()

for thread in threads:
    thread.join()

for thread in threads:
    print("Thread {0.tid} = {0.val}".format(thread))
    pi_est += thread.val

print("\nRESULTADOS")
print(f"Tempo: {time.time() - start}")
print(f"Estimativa: {4*pi_est:.8f}")
print(f"Valor Real: {math.pi:.8f}")

# Confere se a diferença entre o valor real de pi e a estimativa é menor que um determinado valor de erro (próximo termo do somatório)
diferenca = abs(math.pi - 4*pi_est)
prox = 4*((-1)**N/(2*N + 1))

if (diferenca <= prox):
    print("\nA estimativa está correta")
