#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
	int i;
	int status;
	int p[4]; //vetor que vai guardar os 4 pids dos processos filho gerados.

	for (i = 0; i < 4; i++) //queremos gerar 4 processos filhos apenas
	{
		p[i] = fork(); //criação de um processo filho
		if (p[i] == 0) //estamos dentro do processo filho
		{
			sleep(1); //sleep de 1 segundo
			exit(i);  //exit com o índice sequencial de geração de processo
		}
	}

	for (i = 0; i < 4; i++) //percorrer os 4 pids guardados
	{
		if (p[i] % 2 == 0) //se o pid for um número par queremos esperar por ele
		{
			waitpid(p[i], &status, 0);										 //o pai espera pelo processo filho
			printf("pid: %d / exitstatus: %d\n", p[i], WEXITSTATUS(status)); //imprime o pid do processo filho e o seu código de exit
		}
	}
	printf("This is the end.\n");
}