#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>


static int (*real_creat) (const char *path, mode_t mode)=NULL;
static int filedesc;

void die(char *s)
{
    perror(s);
    exit(1);
}

static const char * const SOCKET_PATH = "my_path";

int creat(const char *path, mode_t mode)
{
    char *msg;
    int fd;
    int capmode=1;
    int socket_fd = -1;
    if(real_creat==NULL)
    {
        printf("\nWrapping creat() systemcall\n");
        printf("File name : %s\n", path);
        printf("File mode : %o\n\n", mode);

      /*  if(capmode)
        {
            printf("Hey Iam Capability mode.\n\n");
            client(path,mode);
            return socket_fd;
        }
        else
        {
            printf("Iam Normal mode.\n");
            real_creat=dlsym(RTLD_NEXT, "creat");

            if((msg=dlerror())!=NULL)
            {
                fprintf(stderr, "creat : dlcreat failed\n");
            }
            else
                fd=real_creat(path,mode);
        }
      }
      return fd;
}*/
            real_creat=dlsym(RTLD_NEXT, "creat");
            fd=real_creat(path,mode);

        return fd;

      }
}
