#/* Disciplina: Programacao Concorrente */
#/* Prof.: Silvana Rossetto */
#/* Módulo 4 - Laboratório: 7 */
#/* Codigo: calculando pi com threads */  

from threading import Thread
import time
import math

#Variaveis globais
N = 10000000
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

# Confere se a diferença entre o valor real de pi e a estimativa é menor que um determinado valor de erro
if (abs(math.pi - 4*pi_est) <= 1/N):
    print("\nA estimativa está correta")