#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define MAXCHAR 1000000

int main()
{

    int p, fd[2], i = 0, c;

    char f[MAXCHAR];

    char input[MAXCHAR];

    if (pipe(fd) == -1) //cria o pipe
    {
        perror("Pipe failed");
        return 1;
    }

    p = fork();

    if (p > 0) //estamos no processo pai
    {
        close(fd[0]);

        FILE *fp = fopen("test.txt", "r");

        if (fp == NULL)
        {
            printf("Could not open file");
            return 1;
        }

        while (1)
        {
            c = fgetc(fp);
            if (feof(fp))
            {
                break;
            }
            f[i] = c;
            i++;
        }

        fclose(fp);

        write(fd[1], f, sizeof(f) + 1); //escreve no pipe com o descritor
        close(fd[1]);                   //fecha a escrita
    }
    else if (p == 0) //estamos no processo filho
    {
        close(fd[1]);
        read(fd[0], input, sizeof(input));
        printf("Leu a frase: %s\n", input);
        close(fd[0]); //fecha a escrita
    }

    return 0;
}