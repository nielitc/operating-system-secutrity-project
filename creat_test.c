#include<stdio.h>
#include<fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){

		int fd,fd2;
		if(argc!=2)
		{
	  		printf("Usage : ./creat_test <file name>\n");
	    	exit(1);
		}
    pid_t pid = getpid();
    printf("\npid is %d\n",pid);
      		
    fd=creat(argv[1],S_IRWXU|S_IRGRP|S_IROTH);
   	
    printf("fd  = %d\n",fd);
    open(argv[1],O_WRONLY|O_APPEND,S_IRWXU);
   // printf("fd2  = %d\n",fd2);
    

    return 0;
}
