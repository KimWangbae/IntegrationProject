#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void term_status(int status)
{
    if(WIFEXITED(status))
    {
        printf("(exit)status: 0x%x\n", WEXITSTATUS(status));
    }
    else if(WTERMSIG(status))
    {
        printf("(signal)status: 0x%x\n", status & 0x7f);
    }
}

void my_sig(int signo)
{
    int status;

    printf("signo = %d\n", signo);

    wait(&status);
    term_status(status);
}

int main(void)
{
    int i;
    pid_t pid;

    signal(SIGCHLD, my_sig); //즉각 반응 할 수 있게 행동지침 내림

    if((pid = fork()) > 0)
    {
        for(i=0; i<10000; i++)
        {
            usleep(50000);
            printf("%d\n", i + 1);
        }
    }
    else if(!pid)
    {
        sleep(5); //5초 후에 SIGCHLD가 동작
    }
    else
    {
        perror("fork() ");
        exit(-1);
    }

    return 0;
}
