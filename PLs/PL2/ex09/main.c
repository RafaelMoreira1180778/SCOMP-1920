#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CHILDS 10
#define ARRAY_SIZE 50000

typedef struct
{
    int customer_code;
    int product_code;
    int quantity;
} sales;

int main()
{
    int fd[2];
    int pids[CHILDS];
    int i, j, status;
    int p_code;

    ssize_t rc;

    sales array[ARRAY_SIZE];
    int products[ARRAY_SIZE];

    srand(time(NULL) + getpid());

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        array[i].customer_code = rand() % 100;
        array[i].product_code = rand() % 50;
        array[i].quantity = rand() % 100;
    }

    if (pipe(fd) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    for (i = 0; i < CHILDS; i++)
    {
        pids[i] = fork();
        if (pids[i] == 0)
        {
            for (j = 5000 * i; j < (5000 * i) + 5000; j++)
            {
                if (array[j].quantity > 20)
                {
                    write(fd[1], &array[j].product_code, sizeof(array[j].product_code));
                }
            }
            exit(0);
        }
    }

    close(fd[1]);
    while ((rc = read(fd[0], &p_code, sizeof p_code)) > 0)
    {
        products[i] = p_code;
        i++;
    }

    for (i = 0; i < CHILDS; i++)
    {
        waitpid(pids[i], &status, 0);
    }

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        if (products[i] > 0)
        {
            printf("P_CODE: %d\n", products[i]);
        }
    }

    return 0;
}
