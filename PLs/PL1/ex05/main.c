#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{

	int i;	//índice dentro do vetor de pids
	int p[2]; //vetor que guarda os pids dos processos
	int status;

	for (i = 0; i < 2; i++)
	{
		p[i] = fork(); //guarda em p[i] o id do filho
		if (p[i] == 0) //estamos dentro do filho
		{
			sleep(i + 1);
			exit(i + 1);
		}
	}
	for (i = 0; i < 2; i++)
	{
		waitpid(p[i], &status, 0); //espera pelo processo com o pid p[i]
		printf("%d\n", WEXITSTATUS(status)); //retorna o exitstatus do processo p[i]
	}
}
