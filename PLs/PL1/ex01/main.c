#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
	int x = 1;
	pid_t p = fork(); /*pid_t: sys/types.h; fork(): unistd.h*/
	if (p == 0)
	{ // Filho
		x = x + 1;
		printf("1. x = %d\n", x);
	}
	else
	{ // Pai
		x = x - 1;
		printf("2. x = %d\n", x);
	}
	printf("3. %d; x = %d\n", p, x);

	return 0;
}
