#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 1000

int main()
{
	int numbers[ARRAY_SIZE]; /* array to lookup */
	int n;					 /* the number to find */
	int i;

	int status;			  //status preciso para o exit status e waitpid
	int nH1 = 0, nH2 = 0; //variáveis que guardam o número de vezes que n surge
	int c1 = 0, c2 = 0;   //contadores para cada uma das partes (por questoes de controlo)

	/* intializes RNG (srand():stdlib.h; time(): time.h) */
	srand(time(NULL));

	/* initialize array with random numbers (rand(): stdlib.h) */
	for (i = 0; i < ARRAY_SIZE; i++)
		numbers[i] = rand() % 10000;

	/* initialize n */
	n = rand() % 10000;

	int a = (int)fork(); //a == pid do processo filho

	if (a == 0) //estamos dentro do processo filho
	{
		for (i = ARRAY_SIZE / 2; i < ARRAY_SIZE; i++) //o processo filho analisa de metade do vetor ate ao fim
		{
			++c2; //aumentar o numero de posiçoes lidas nesta metade
			if (numbers[i] == n)
			{
				++nH2; //aumenta o contador de aparencias nesta metade do vetor
			}
		}
		printf("lidos %d numeros\n%d surgiu %d vez na segunda metade do vetor\n", c2, n, nH2);
		exit(nH2); //exit do processo filho com o numero de aparencias de n nesta metade do vetor
	}
	else
	{
		if (a > 0) //estamos dentro do processo pai
		{
			for (i = 0; i < ARRAY_SIZE / 2; i++) //o processo pai le de 0 até metade do vetor
			{
				++c1; //aumenta o numero de posiçoes lidas nesta metade
				if (numbers[i] == n)
				{
					++nH1; //aumenta o contador de aparencias nesta metade do vetor
				}
			}
			printf("lidos %d numeros\n%d surgiu %d vez na segunda primeira do vetor\n", c1, n, nH1);

			waitpid(a, &status, 0);											 //o processo pai espera pelo processo filho terminar para efetuar a soma do total
			printf("total de ocorrencias: %d\n", nH1 + WEXITSTATUS(status)); //soma = contador do Pai + exit status do filho
		}
	}
}