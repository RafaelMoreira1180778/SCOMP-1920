#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    int a = 0, b, c, d;
    b = (int)fork();
    c = (int)getpid(); /* getpid(), getppid(): unistd.h*/
    d = (int)getppid();
    a = a + 5;
    //if(b>0) wait(NULL);
    printf("\na=%d, b=%d, c=%d, d=%d\n", a, b, c, d);

    return 0;
}