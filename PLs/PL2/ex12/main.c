#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CHILDS 5
#define NUM_PRODUCTS 10

typedef struct
{
    char name[20];
    int price;

} product;

int generateRandomNumber(int a)
{
    srand(time(NULL) + a);
    return rand() % 50 + 1;
}

//Retirado de: https://codereview.stackexchange.com/questions/29198/random-string-generator-in-c
char *generateRandomName(size_t length)
{

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *randomString = NULL;

    if (length)
    {
        randomString = malloc(sizeof(char) * (length + 1));

        if (randomString)
        {
            for (int n = 0; n < length; n++)
            {
                int key = rand() % (int)(sizeof(charset) - 1);
                randomString[n] = charset[key];
            }

            randomString[length] = '\0';
        }
    }

    return randomString;
}

int main()
{
    product p, m;
    product array[NUM_PRODUCTS];

    int i, r;
    pid_t pid;
    int fd1[CHILDS][2], fd2[2];

    for (i = 0; i < NUM_PRODUCTS; i++)
    {
        strcpy(p.name, generateRandomName(5));
        p.price = generateRandomNumber(i);
        array[i] = p;
    }

    for (i = 0; i < CHILDS; i++)
    {
        if (pipe(fd1[i]) == -1)
        {
            perror("Pipe1 failed");
            return 1;
        }
    }

    if (pipe(fd2) == -1)
    {
        perror("Pipe2 failed");
        return 1;
    }

    for (i = 0; i < CHILDS; i++)
    {
        pid = fork();
        if (pid == -1)
        {
            perror("Fork failed");
            exit(-1);
        }
        else if (pid == 0)
        {
            break;
        }
    }

    if (pid > 0)
    {
        close(fd2[1]);
        for (i = 0; i < CHILDS; i++)
        {
            read(fd2[0], &r, sizeof(int));
            m = array[r];
            write(fd1[r][1], &m, sizeof(m));
        }
        for (i = 0; i < CHILDS; i++)
        {
            wait(NULL);
        }
        close(fd2[0]);
    }
    else
    {
        close(fd2[0]);
        write(fd2[1], &i, sizeof(int));
        read(fd1[i][0], &m, sizeof(m));
        printf("%s costs %d --- %d\n", m.name, m.price, i);
        exit(0);
    }

    return 0;
}
