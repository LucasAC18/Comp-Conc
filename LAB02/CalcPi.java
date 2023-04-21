import java.util.Scanner;

/* Disciplina: Computacao Concorrente */
/* Aluno: Lucas Araujo Carvalho */
/* Laboratório: 3 */
/* Codigo: Calcula a série que estima valor de pi */


//--PASSO 1: cria uma classe que estende a classe Thread 
class SumPi extends Thread {
   private double sum;
   private int index;
   private int threadCount;
   private int n;
   
   //--construtor
   public SumPi(int id, int count, int n) { 
      this.sum = 0;
      this.index = id;
      this.threadCount = count;
      this.n = n; 
   }

   //--metodo executado pela thread
   public void run() {
      for (int i = index; i < n; i += threadCount) {
          sum += Math.pow(-1, i) * (1.0/(2*i+1));
      }
   }

   public double getSum() {
       return this.sum;
   }
}

//--classe do metodo main
class CalcPi {

   public static void main (String[] args) {
      //--reserva espaço para um vetor de threads
      int nThreads = 1;
      int nTerms = 1;
      double sumTotal = 0;
      Scanner myScanner = new Scanner(System.in);

      //--pega inputs do usuário para as variáveis
      System.out.println("Threads count: ");
      nThreads = myScanner.nextInt();
      System.out.println("Terms count: ");
      nTerms = myScanner.nextInt();
      myScanner.close();
      
      //--cria array onde serão guardadas as classes das threads
      SumPi[] threads = new SumPi[nThreads];

      //--calculando a série de maneira sequencial para comparação posterior
      double sumSeq = 0;
      for (int i = 0; i < nTerms; i++) {
         sumSeq += Math.pow(-1, i) * (1.0/(2*i+1));
      }

      //--PASSO 2: criar instancias da classe SumPi
      for (int i=0; i<threads.length; i++) {
         threads[i] = new SumPi(i, threads.length, nTerms);
      }

      //--PASSO 3: iniciar a thread
      for (int i=0; i<threads.length; i++) {
         threads[i].start();
      }

      //--PASSO 4: esperar pelo termino das threads
      for (int i=0; i<threads.length; i++) {
            try { threads[i].join(); } 
            catch (InterruptedException e) { return; }
      }

      for (int i=0; i<nThreads; i++) {
          sumTotal += threads[i].getSum();
      }

      System.out.println(String.format("Estimated value (%d Threads): %.8f", nThreads, 4*sumTotal));
      System.out.println(String.format("Estimated value (Sequential): %.8f", 4*sumSeq));

      //--PASSO 5: conferindo se a conta bateu. Como estamos tratando com valores de ponto flutuante, usamos uma margem de erro
      double erro = 4*sumSeq - 4*sumTotal;
      if (erro <= Math.pow(10,-8)) {
         System.out.println("Está correto");
      }
   }
}