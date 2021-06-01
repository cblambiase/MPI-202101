#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
 
//#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 100000      // trabalho final com o valores 10.000, 100.000, 1.000.000
#define ARRAY_NUMBER 1000      // número de arrays
 
int compare (const void *a, const void *b)
{
  return ( *(int*)a - *(int*)b );
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


int (*saco)[ARRAY_NUMBER] = malloc(ARRAY_SIZE * sizeof *saco);
 
int vetor_recebido[ARRAY_SIZE];
int ponteiro,linha_saco;
int i,j,np;

double inteiro[10];
double t1,t2;
t1 = MPI_Wtime();  // inicia a contagem do tempo
inteiro[1]=t1;


ponteiro=0;
	
if (my_rank==0) {
	for (j=0 ; j<ARRAY_NUMBER; j++)
	{
		for (i=0 ; i<ARRAY_SIZE; i++){              /* init array with worst case for sorting */
			saco[j][i] = (ARRAY_SIZE+ARRAY_SIZE*j)-i;
			//printf("\n j=%d e i=%d do processo %d",j,i,my_rank);
		}
	}

	#ifdef DEBUG
	printf("\nVetor unsorted: ");
	for (j=0 ; j<ARRAY_NUMBER; j++)
	{
		for (i=0 ; i<ARRAY_SIZE; i++){              /* imprime todo o saco de trabalho */
			printf("[%03d] ", saco[j][i]);
		}
		printf("linha %d \n",j);
	}
	#endif

	// os outros processos escravos
	//int vetor_ordem[ARRAY_SIZE];
	#ifdef DEBUG
	printf("\nEntrou IF do Processo %d: ",my_rank);
	#endif	
	while(1){

	   //linha_saco=vetor_recebido[ARRAY_SIZE];
	   
		if(ponteiro<ARRAY_NUMBER){
			//for(i=0;i<ARRAY_SIZE;i++) vetor_recebido[i] = saco[ponteiro][i];
			qsort(saco[ponteiro],ARRAY_SIZE,sizeof(int),compare);
			ponteiro++;
		} else{
		   printf("\n Fim do processo %d \n",my_rank);
		   break;
		}
	}
}

if (my_rank==0){
	#ifdef DEBUG
	printf("\n");
	for (j=0 ; j<ARRAY_NUMBER; j++)
	{
		for (i=0 ; i<ARRAY_SIZE; i++){              /* imprime todo o saco de trabalho */
			printf("[%03d] ", saco[j][i]);
		}
		printf("linha %d \n",j);
	}
	#endif
	
	t1=inteiro[1];
        t2 = MPI_Wtime(); // termina a contagem do tempo
        printf(" \nTempo gasto pela mensagem da Raiz até o processo %d: %f\n",my_rank, t2-t1);
        t1=inteiro[2];
        printf(" Tempo gasto pela mensagem desde o processo anterior até o processo %d: %f\n",my_rank, t2-t1);
}
free(saco);   
MPI_Finalize();
}
