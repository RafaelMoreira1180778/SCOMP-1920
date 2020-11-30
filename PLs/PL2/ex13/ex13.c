#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define CHILDS 4
#define PIECES 1000

void initiatePipes(int fd[CHILDS][2])
{
    int i;
    for (i = 0; i < CHILDS; i++)
    {
        if (pipe(fd[i]) == -1)
        {
            perror("Broken Pipe");
        }
    }
}

int main()
{
    int pids[CHILDS], status;
    int fd[CHILDS][2];

    int i, globalInventory = 0, f = 5, c = 0, a;

    initiatePipes(fd);

    for (i = 0; i < 4; i++)
    {
        pids[i] = fork();
        if (pids[i] == 0)
        {
            if (i == 0)
            {
                close(fd[i][0]);
                while (c != PIECES)
                {
                    c += 5;
                    write(fd[i][1], &f, sizeof(f));
                    printf("peças cortadas: %d | Máquina M1\n", c);
                }
                close(fd[i][1]);
                exit(0);
            }
            if (i == 1)
            {
                close((fd[i - 1][1]));
                close((fd[i][0]));
                while (c != PIECES)
                {
                    read(fd[i - 1][0], &f, sizeof(f));
                    c += f;
                    write(fd[i][1], &f, sizeof(f));
                    printf("peças dobradas: %d | Máquina M2\n", c);
                }
                close(fd[i][1]);
                close(fd[i - 1][0]);
                exit(0);
            }
            if (i == 2)
            {
                close((fd[i - 1][1]));
                close((fd[i][0]));
                int dez = 10;
                while (c != PIECES)
                {
                    read(fd[i - 1][1], &f, sizeof(f));
                    c += f;
                    if (c % 10 == 0)
                    {
                        write(fd[i][1], &dez, sizeof(dez));
                        printf("peças soldadas: %d | Máquina M3\n", c);
                    }
                }
                close(fd[i][1]);
                close(fd[i - 1][0]);
                exit(0);
            }
            if (i == 3)
            {
                close((fd[i - 1][1]));
                close((fd[i][0]));
                int cem = 100;
                while (c != PIECES)
                {
                    read(fd[i - 1][1], &f, sizeof(f));
                    c += f;
                    if (c % 100 == 0)
                    {
                        write(fd[i][1], &cem, sizeof(cem));
                        printf("peças empacotadas: %d | Máquina M4\n", c);
                    }
                }
                close(fd[i][1]);
                close(fd[i - 1][0]);
                exit(0);
            }
        }
    }
    close(fd[3][1]);
    while (globalInventory != PIECES)
    {
        read(fd[3][0], &a, sizeof(a));
        globalInventory += a;
        printf("Inventário: %d\n", globalInventory);
    }
    close(fd[3][0]);

    for (i = 0; i < 4; i++)
    {
        waitpid(pids[i], &status, 0);
    }
}