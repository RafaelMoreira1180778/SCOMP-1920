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

struct invenTOry
{
    int TOtal;
};

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

    struct invenTOry global;
    global.TOtal = 0;

    int i, a, f = 5, c = 0, h = 100, t = 10;

    initiatePipes(fd);

    for (i = 0; i < CHILDS; i++)
    {
        pids[i] = fork();
        if (pids[i] == 0)
        {
            if (i == 0) //M1
            {
                close(fd[i][0]);
                while (c != PIECES)
                {
                    c += 5;
                    write(fd[i][1], &f, sizeof(f));
                    printf("M1 CUT 5 PIECES, SENDING TO M2...\n");
                }
                close(fd[i][1]);
                exit(0);
            }
            if (i == 1) //M2
            {
                close((fd[i - 1][1]));
                close((fd[i][0]));
                while (c != PIECES)
                {
                    read(fd[i - 1][0], &f, sizeof(int));
                    c += f;
                    write(fd[i][1], &f, sizeof(f));
                    printf("M2 folded 5 PIECES, SENDING TO M3...\n");
                }
                close(fd[i][1]);
                close(fd[i - 1][0]);
                exit(0);
            }
            if (i == 2) //M3
            {
                close((fd[i - 1][1]));
                close((fd[i][0]));
                while (c != PIECES)
                {
                    read(fd[i - 1][1], &f, sizeof(int));
                    c += f;
                    if (c % 10 == 0) //10 em 10
                    {
                        write(fd[i][1], &t, sizeof(t)); //envia 10
                        printf("M3 WELDED 10 PIECES, SENDING TO M4...\n");
                    }
                }
                close(fd[i][1]);
                close(fd[i - 1][0]);
                exit(0);
            }
            if (i == 3) //M4
            {
                close((fd[i - 1][1]));
                close((fd[i][0]));
                while (c != PIECES)
                {
                    read(fd[i - 1][1], &f, sizeof(f));
                    c += f;
                    if (c % 100 == 0) //100 em 100
                    {
                        write(fd[i][1], &h, sizeof(h)); //envia 100
                        printf("M4 PACKED 100 PIECES, SENDING TO A1...\n");
                    }
                }
                close(fd[i][1]);
                close(fd[i - 1][0]);
                exit(0);
            }
        }
    }

    close(fd[CHILDS - 1][1]);      //fd[3][1] M4
    while (global.TOtal != PIECES) //A1
    {
        read(fd[CHILDS - 1][0], &a, sizeof(int));
        printf("M4 SENT 100 PIECES, ADDING TO INVENTORY...\n");
        global.TOtal += a;
        printf("INVENTORY UPDATED: %d\n", global.TOtal);
    }
    close(fd[CHILDS - 1][0]); //fd[3][0] M4

    for (i = 0; i < CHILDS; i++) //pai espera por TOdos os processos
    {
        waitpid(pids[i], &status, 0);
    }

    return 0;
}