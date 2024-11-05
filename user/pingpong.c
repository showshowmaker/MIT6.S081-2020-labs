#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int pid;
    int pp[2],pc[2];
    char buf[2];
    pipe(pp);
    pipe(pc);
    pid = fork();
    if (pid < 0){
        fprintf(2,"error in fork\n");
        exit(1);
    }
    else if (pid == 0){
        close(pp[1]);
        read(pp[0],buf,1);
        close(pp[0]);
        fprintf(1,"%d: received ping\n",getpid());

        close(pc[0]);
        write(pc[1],"a",1);
        close(pc[1]);
    }
    else{
        close(pp[0]);
        write(pp[1],"a",1);
        close(pp[1]);

        close(pc[1]);
        read(pc[0],buf,1);
        close(pc[0]);
        fprintf(1,"%d: received pong\n",getpid());
    }
    exit(0);
}