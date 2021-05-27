#include <string.h>
#include <stdio.h>
#include "mpi.h"
 
//#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 10000      // trabalho final com o valores 10.000, 100.000, 1.000.000
#define ARRAY_NUMBER 100      // número de arrays
 
void bs(int n, int * vetor)
{
    int c=0, d, troca, trocou =1;

    while (c < (n-1) & trocou )
        {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1])
                {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
                }
        c++;
        }
} 
 
main(int argc, char** argv)
{
int my_rank;  /* Identificador do processo */
int proc_n;   /* Número de processos */
int source;   /* Identificador do proc.origem */
int dest;     /* Identificador do proc. destino */
int tag = 50; /* Tag para as mensagens */


MPI_Status status; /* Status de retorno */
 
MPI_Init (&argc , & argv);
 
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
 
int saco[ARRAY_NUMBER][ARRAY_SIZE]; 
int vetor_recebido[ARRAY_SIZE+1];
int ponteiro,linha_saco;
int i,j,np;

	ponteiro=0;
	
if (my_rank == 0){//o primeiro cria a mensagem		
	
	np=proc_n;
	
	printf("\n Entrou no IF do processo %d",my_rank);
	
	for (j=0 ; j<ARRAY_NUMBER; j++)
	{
		for (i=0 ; i<ARRAY_SIZE; i++){              /* init array with worst case for sorting */
			saco[j][i] = (ARRAY_SIZE+ARRAY_SIZE*j)-i;
			//printf("\n j=%d e i=%d do processo %d",j,i,my_rank);
		}
	}
	while(1){
	   MPI_Recv (vetor_recebido, ARRAY_SIZE+1, MPI_INT , MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	   
	   if (status.MPI_TAG==42){ //mensagem pedindo trabalho
		   dest=status.MPI_SOURCE;
		   if(ponteiro<ARRAY_NUMBER){
			   
			   for(i=0;i<ARRAY_SIZE;i++) vetor_recebido[i] = saco[ponteiro][i];			   
			   tag=43;
			   /*Na última posição do vetor com tamanho ARRAY_SIZE+1 colocamos a linha origem */				
			   vetor_recebido[ARRAY_SIZE]=ponteiro;
				MPI_Send (vetor_recebido, ARRAY_SIZE+1, MPI_INT,dest, tag, MPI_COMM_WORLD);	   
				ponteiro++;
			} else {
				//envia mensagem de Kill process
				tag=1; // manda o processo morrer
				MPI_Send (vetor_recebido, ARRAY_SIZE+1, MPI_INT,dest, tag, MPI_COMM_WORLD);	   				
				np--;
				printf("\n Processos restantes %d ",np);
				// se terminaram os processos BREAK
				if (np==1) break;
			}
	   } else if (status.MPI_TAG==43){
		   linha_saco = vetor_recebido[ARRAY_SIZE]; // processo está devolvendo trabalho de qual linha?
		   for(i=0;i<ARRAY_SIZE;i++) saco[linha_saco][i] = vetor_recebido[i];
			
			#ifdef DEBUG
			printf("\n TODOS Elementos do Vetor Ordenado: ");
			for (i=0 ; i<=ARRAY_SIZE; i++)              /* print sorted array */
			printf("[%03d] ", vetor_recebido[i]);
			#endif		   
	   }
	   
	} 
} 
else if (my_rank>0) {
	// os outros processos escravos
	//int vetor_ordem[ARRAY_SIZE];
			#ifdef DEBUG
			printf("\nEntrou IF do Processo %d: ",my_rank);
			#endif	
	while(1){
	   dest=0;
	   tag=42;
	   MPI_Send (vetor_recebido, ARRAY_SIZE+1, MPI_INT, dest, tag, MPI_COMM_WORLD);	 
	   MPI_Recv (vetor_recebido, ARRAY_SIZE+1, MPI_INT , MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	   if (status.MPI_TAG==43){
			#ifdef DEBUG
			printf("\nVetor unsorted: ");
			for (i=0 ; i<=ARRAY_SIZE; i++)              /* print unsorted array */
			printf("[%03d] ", vetor_recebido[i]);
			#endif
		   //linha_saco=vetor_recebido[ARRAY_SIZE];
		   bs(ARRAY_SIZE,vetor_recebido);
		   tag=43;
		   MPI_Send (vetor_recebido, ARRAY_SIZE+1, MPI_INT, dest, tag, MPI_COMM_WORLD);
	   } else if (status.MPI_TAG==1) {
		   printf("\n Fim do processo %d \n",my_rank);
		   break;
	   }
	}
}

if (my_rank==0){
	printf("\n");
	for (j=0 ; j<ARRAY_NUMBER; j++)
	{
		for (i=0 ; i<ARRAY_SIZE; i++){              /* imprime todo o saco de trabalho */
			printf("[%03d] ", saco[j][i]);
		}
		printf("linha %d \n",j);
	}
}
   
MPI_Finalize();
}
