#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ARRAY_SIZE 2000
#define PROCESSOS_FILHOS 10

int main()
{
    int pids[PROCESSOS_FILHOS];
    int numbers[ARRAY_SIZE]; /* array to lookup */
    int n;                   /* the number to find */
    int i, j;
    int status; //status preciso para o exit status e waitpid

    /* intializes RNG (srand():stdlib.h; time(): time.h) */
    srand(time(NULL));

    /* initialize array with random numbers (rand(): stdlib.h) */
    for (i = 0; i < ARRAY_SIZE; i++)
    {
        //numbers[i] = rand() % 10000;
        numbers[i] = 7;
    }

    /* initialize n */
    n = rand() % 10000;

    printf("------------------\nn = %d\n------------------\n", n);

    for (i = 0; i < PROCESSOS_FILHOS; i++)
    {
        pids[i] = fork(); //a = pid do filho
        if (pids[i] == 0) //estamos dentro do processo filho
        {
            for (j = i * 200; j < (i + 1) * 200; j++)
            {
                if (numbers[j] == n)
                {
                    if (i > 0)
                    {
                        exit((j / (200 * i)) - 1);
                    }
                    else
                    {
                        exit(j); //exit com a posição de n no vetor numbers
                    }
                }
                else
                {
                    exit(255); //se n não for encontrado exit com o valor de saída 255
                }
            }
        }
    }

    for (i = 0; i < PROCESSOS_FILHOS; i++)
    {
        waitpid(pids[i], &status, 0);
        int exit = WEXITSTATUS(status);
        if (exit != 255)
        {
            printf("posiçao %d\n------------------\n", (+1) * (200 * i));
        }
        else
        {
            printf("posiçao %d\n------------------\n", exit);
        }
    }

    return 0;
}