#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int
main(int argc, char *argv[])
{
    char line[512],c[1];
    int n,xargc;
    char *xargv[MAXARG],*p;
    for(int i=1;i<argc;i++){
        xargv[i-1]=argv[i];
    }
    xargc=argc-1;
            
    if(argc<2){
        exit(0);
    }
    p=line;
    while((n=read(0,c,1))>0){
        if(c[0]=='\n'){
            *p='\0';
            p=line;
            xargv[xargc]=p;
            int pid=fork();
            if(pid<0){
                fprintf(2,"fork error\n");
                exit(1);
            }
            else if(pid==0){
                // fprintf(1,"exec %s ",xargv[0]);
                // for(int i=1;i<xargc;i++){
                //     fprintf(1,"%s ",xargv[i]);
                // }
                // fprintf(1,"\n");
                exec(xargv[0],xargv);
                fprintf(2,"exec error\n");
                exit(1);
            }
            else{
                wait(0);
            }
        }
        else{
            *p++=c[0];
        }
    }

    exit(0);
}