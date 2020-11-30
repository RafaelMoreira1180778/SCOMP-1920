#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{

	fork();
	fork();
	fork();
	printf("SCOMP!\n");

	return 0;
}
