#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

int primo(long int n)
{ /* mpi_primos.c  */
	int i;

	for (i = 3; i < (int)(sqrt(n) + 1); i += 2)
	{
		if (n % i == 0)
			return 0;
	}
	return 1;
}

int main(int argc, char *argv[])
{
	double t_inicial, t_final;
	int cont = 0, total = 0, etiq = 0, origem = 0;
	long int i, n;
	int meu_ranque, num_procs, inicio, salto;
	MPI_Status estado;
	MPI_Request message;
	
	if (argc < 2)
	{
		printf("Valor inválido! Entre com um valor do maior inteiro\n");
		return 0;
	}
	else
	{
		n = strtol(argv[1], (char **)NULL, 10);
	}

	/* Nenhuma função MPI pode ser chamada antes deste ponto */
	MPI_Init(&argc, &argv);

	/* Determina o ranque do processo em relação aos demais */
	MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);

	/* Determina o número de processos que estão ativos */
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	/* Inicia a contagem do tempo */
	t_inicial = MPI_Wtime();

	/* Cada processo verifica um número ímpar */
	/* Soma-se três para garantir que vamos verificar apenas números ímpares */
	inicio = 3 + meu_ranque * 2;
	/* O salto é de 2*num_procs para garantir que cada processo verifique um número ímpar */
	salto = num_procs * 2;

	for (i = inicio; i <= n; i += salto)
	{
		if (primo(i) == 1)
			cont++;
	}

	if (meu_ranque != 0)
	{
		/* É bloqueante e uma versão síncrona da função MPI_Send e bloqueia o destinatário até que a mensagem seja recebida */
		MPI_Ssend(&cont, 1, MPI_INT, 0, etiq, MPI_COMM_WORLD);
	}
	else
	{

		total = cont;

		for (origem = 1; origem < num_procs; origem++)
		{
			/* Operação bloqueante/síncrona  - fica bloqueado até que a mensagem seja recebida*/
			MPI_Recv(&cont, 1, MPI_INT, origem, etiq, MPI_COMM_WORLD, &estado);
			total += cont;
		}

		total += 1; /* Acrescenta o dois, que também é primo */

		/* Finaliza a contagem do tempo */
		t_final = MPI_Wtime();
		printf("Quant. de primos entre 1 e n: %d \n", total);
		printf("Tempo de execucao: %1.3f \n", t_final - t_inicial);
	}

	/* Finaliza o ambiente MPI */
	MPI_Finalize();

	return (0);
}