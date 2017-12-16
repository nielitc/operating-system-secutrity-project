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
#include <string.h>
int daemonfd,i;
char *perm;
char *pathname;
static int (*real_creat) (const char *path, mode_t mode)=NULL;
static int filedesc;
int capid;
char capoid[10];

void die(char *s)
{
    perror(s);
    exit(1);
}

static const char * const SOCKET_PATH = "my_path";
void recv_file_descriptor(int socket_fd, int fd[1])
{
    struct msghdr message;            /*Structure which holds all the information on what to send and recieve*/
    struct iovec iov;             /*Structure holding the I/O buffers*/
    struct cmsghdr *control_message = NULL;   /*control message header*/
    char ctrl_buf[CMSG_SPACE(sizeof(int) * 1)]; //sending fd in control msg
    char data[1];
    int res = -1;

    memset(&message, 0, sizeof(message));
    memset(&iov, 0, sizeof(iov));
    memset(ctrl_buf, 0, sizeof(ctrl_buf));

    data[0] = '\0';
    iov.iov_base = data;
    iov.iov_len = sizeof(data);

    message.msg_name = NULL;            //optional address
    message.msg_namelen = 0;            //address size in bytes
    message.msg_control = ctrl_buf;         //ancillary data
    message.msg_controllen = sizeof(ctrl_buf);    //number of ancillary data
    message.msg_iov = &iov;             //array of I/O buffers
    message.msg_iovlen = 1;             //number of elements in array

    res = recvmsg(socket_fd, &message, 0);

    if (res < 0)
    {
        fprintf(stderr, "recvmsg failed: %s\n", strerror(errno));
    }

    for (control_message = CMSG_FIRSTHDR(&message);control_message != NULL;control_message = CMSG_NXTHDR(&message, control_message))
    {
        //printf("LibC : Got control_message from Capd...!!\n");
        int i = 0;
        int q = (control_message->cmsg_len - CMSG_ALIGN(sizeof(struct cmsghdr))) / sizeof(int);
        //printf("fd count = %d\n", q);

        for (i = 0; i < q; i++)
        {
             if ((control_message->cmsg_level == SOL_SOCKET)&& (control_message->cmsg_type == SCM_RIGHTS))
             {
                  fd[i] = *((int *)CMSG_DATA(control_message) + i);
                  printf("LibC : File Descriptor recieved :  %d\n\n",fd[i]);
            }
        }
    }

     return;
}

/*end of function recieve file descriptor */




// Function to remove all spaces from a given string
void removeSpaces(char *str)
{
    // To keep track of non-space character count
    int count = 0;
    int i;
    // Traverse the given string. If current character
    // is not space, then place it at index 'count++'
    for (i = 0; str[i]; i++)
        if (str[i] != ' ')
            str[count++] = str[i]; // here count is
                                   // incremented
    str[count] = '\0';
}



void client(const char *path,mode_t mode)
{
        int fd=-1, socket_fd = -1,nbytes;
  	char buffer[256],buffer1[256],req[256];
  	struct sockaddr_un address;

	bzero(&address,sizeof(address));
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, SOCKET_PATH);


       	socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
       	//printf("socket_fd:: %d\n",socket_fd);

	if (socket_fd < 0)
  	{
		die("socket");
  	}


	if (connect(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0){

	fprintf(stderr, "connect failed: %s\n", strerror(errno));
	printf("error");
	}




	 /*cap_create request*/

   	printf("Capability Object Creation\n");
    	nbytes=snprintf(buffer,256,"%s","cap_create");
    	printf("Wrapper Sending capability create request : %s\n", buffer);
    	write(socket_fd,buffer,nbytes);


    	nbytes = read(socket_fd, buffer, 256);
    	buffer[nbytes]=0;
      printf("%s\n", buffer);

        nbytes=snprintf(buffer,256,"%s",pathname);
        printf("Wrapper Sending resource specification from xml file : %s\n", buffer);
       // printf("%d\n",nbytes);
       // printf("hellooooooo");
       write(socket_fd,buffer,nbytes);

//printf("hellooooooo");
//memset(&buffer,'0',sizeof(buffer));
    	nbytes = read(socket_fd, buffer, 256);
    	printf("%s\n", buffer);

    	nbytes=snprintf(buffer,256,"%s",perm);
    	printf("Wrapper Sending operation specification from xml file : %s\n\n", buffer);
    	write(socket_fd,buffer,nbytes);
    	printf("%s\n", buffer);





    	nbytes = read(socket_fd, buffer1, 256);

    	if(buffer1[0] == 'c'){
    		for(int k = 0; k < 256; k++){
    			buffer1[k] = buffer1[k + 10];
    		}
    	}
    	printf("%d\n",nbytes);


       	strcpy(capoid,buffer1);
        printf("capoid : %s\n", buffer1);

nbytes=snprintf(buffer,256,"%s %s","creat",capoid);
   printf("Invocation of CREAT\n");
    printf("LibC : Sending the function name and capoid : %s \n", buffer);
    write(socket_fd,buffer,nbytes);

        nbytes = read(socket_fd, buffer, 256);
        buffer[nbytes] = 0;
        printf("%s\n", buffer);






int fdrecv[2];
    	 recv_file_descriptor(socket_fd, fdrecv);

    	 printf("fd recieved is %d\n", fdrecv[0]);

 	   if (fdrecv[0] < 0)
   	 {
     	   fprintf(stderr, "recv_file_descriptor failed\n");
    	}

 	 daemonfd=fdrecv[0];     //copying fd to a global variable
	 printf("fd recieved is %d\n", daemonfd);



  	  close(socket_fd);
  	  return;
}




/*------------------------------------------------------------------------------------------------------------------------------*/
int creat(const char *path, mode_t mode)
{
    char *msg;
    int fd;
    int capmode=1;


    /*------------------------------------------------------------------------------------------------------------*/
     FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char get_file[256];
    char *extract;
    char *word= "permission";
    char *word1="title";
    char *ext= ".xml";
    char *fname = "creat.xml";

    snprintf(get_file,256,"%s",path);
    extract = strtok (get_file, ".");
    //printf("extracted name is %s\n", extract);
    strcat(extract,ext);
    //printf("concated name is %s\n", extract);

    fp = fopen(fname , "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

   while ((read = getline(&line, &len, fp)) != -1) {
        if (strstr(line, word) != NULL){
            removeSpaces(line);
            perm = (char *) malloc (sizeof (char)*20);
   			/* get the second token */
  			strtok(line, "></");
   			strcpy(perm, strtok(NULL , "<"));
  			printf("permission required is %s\n", perm);
            }


        if (strstr(line, word1) != NULL){
            removeSpaces(line);
            pathname = (char *) malloc (sizeof (char)*20);
   			/* get the second token */
  			strtok(line, "></");
   			strcpy(pathname, strtok(NULL , "<"));
  			printf("path required is %s\n", pathname);
            }
    }


 /*---------------------------------------------------------------------------------------------------------------------*/


    if(real_creat==NULL)
    {
        printf("\nWrapping creat() systemcall\n");
        printf("File name : %s\n", path);
        printf("File mode : %o\n\n", mode);

        if(capmode)
        {
            printf("Hey Iam Capability mode.\n\n");
            client(path,mode);
            return daemonfd;
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
}
