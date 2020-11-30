#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define N_PROCESSOS 10

int main(void)
{

	int i, j;
	int p[N_PROCESSOS]; //vetor que guarda os pids dos 10 processos filhos
	int status;

	for (i = 0; i < N_PROCESSOS; i++)
	{
		p[i] = fork(); //guarda em p[i] o id do filho
		if (p[i] == 0) //estamos dentro do filho
		{
			//i = 0 -> (100*0) + 1 = 1 ... ((0+1) * 100 = 100)
			//i = 1 -> (100*1) + 1 = 101 ... ((1+1) * 100 = 200)
			//i = 2 -> (100*2) + 1 = 201 ... ((2+1) * 100 = 300)
			for (j = (100 * i) + 1; j < ((i + 1) * 100) + 1; j++)
			{
				printf("%d\n", j);
			}
			exit(0);
		}
	}
	for (i = 0; i < 10; i++)
	{
		waitpid(p[i], &status, 0); //espera pelo processo com o pid p[i]
	}

	printf("Todos os numeros foram impressos.\n"); //processo pai espera por TODOS os processos filho

	return 0;
}
