#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void prime(int readfd){
    int myprime=0;
    int wrote=0;
    int forked=0;
    int p[2];
    int cur;
    //int pid=getpid();
    
    while(read(readfd,&cur,4)>0){
        if(myprime==0){
            myprime=cur;
            //fprintf(1,"pid %d,prime %d\n",pid,myprime);
            fprintf(1,"prime %d\n",myprime);
            continue;
        }
        if(cur%myprime!=0){
           if(forked==0){
               pipe(p);
               int pid=fork();
               if(pid==0){
                   close(p[1]);
                   prime(p[0]);
                   break;
               }
               else if(pid<0){
                   fprintf(2,"fork failed\n");
                   exit(1);
               }
               else{
                   forked=1;
                   wrote=1;
                   write(p[1],&cur,4);
                   close(p[0]);
               }
           }
           else{
               write(p[1],&cur,4);
           }
        }
    }
    if(wrote==1){
        close(p[1]);
    }
    close(readfd);
    wait(0);
}

int
main(int argc, char *argv[])
{
    int p[2];
    pipe(p);
    for(int i=2;i<=35;i++){
        write(p[1],&i,4);
    }
    close(p[1]);
    int pid=fork();
    if(pid<0){
        exit(1);
    }
    else if(pid==0){
        prime(p[0]);
    }
    else{
        wait(0);
        close(p[0]);
    }
    exit(0);
}
