#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char* path,char* file){

    //fprintf(1,"find: path %s,file %s\n",path,file);
    char filename[32];
    char buf[512];
    int fd;
    struct dirent de;
    struct stat st;

    char *p;
    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--);
    p++;
    memmove(filename,p,strlen(p)+1);
   //memmove(prefix,path,)


    if((fd=open(path,0))<0){
        fprintf(2,"find: cannot open %s\n",path);
        return;
    }

    if(fstat(fd,&st)<0){
        fprintf(2,"find: cannot stat %s\n",path);
        close(fd);
        return;
    }
    //fprintf(1,"path %s,filename %s\n",path,filename);
    switch(st.type){
    case T_FILE:
      //fprintf(1,"filename %s\n",filename);
      if(strcmp(filename,file)==0){
        fprintf(1,"%s\n",path);
      }
      break;
    case T_DIR:
      strcpy(buf, path);
      p = buf+strlen(buf);
      *p++ = '/';
      p[DIRSIZ]='\0';
      while(read(fd,&de,sizeof(de))==sizeof(de)){
        if(strcmp(de.name,".")==0 || strcmp(de.name,"..")==0 || strcmp(de.name,"")==0){
          //fprintf(1,"skip\n");
          continue;
        }
        memmove(p,de.name,DIRSIZ);
        //fprintf(1,"%s\n",de.name);
        find(buf,file);
      }
      break;
    }
    close(fd);

}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    exit(0);
  }
  find(argv[1],argv[2]);
  exit(0);
}