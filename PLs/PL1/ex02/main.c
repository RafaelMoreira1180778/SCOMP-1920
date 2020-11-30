#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
	printf("I'm..\n");
	if (fork() != 0)
	{						 // Pai
		wait(NULL);			 // Pai espera que o 1º filho termine
		printf("..the..\n"); // O pai esperou que o 1º filho terminasse para escrever
		if (fork() != 0)
		{						 // Pai
			wait(NULL);			 // Pai espera que o 2º filho termine
			printf("father!\n"); // O pai esperou que o 2º filho terminasse para escrever
		}
		else
		{ // 2º Filho criado
			printf("I'll never join you!\n");
			exit(2); // 2º Filho termina
		}
	}
	else
	{ // 1º Filho criado
		printf("I'll never join you!\n");
		exit(1); // 1º Filho termina
	}

	printf("I'll never join you!\n"); // Com os dois filhos terminados, apenas o pai escreve

	return 0;
}
