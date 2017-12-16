#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/socket.h>
#include<arpa/inet.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>

#define MIN 100000
#define MAX 1000000
static const char * const SOCKET_PATH = "my_path";
//static const char * const filename = "rohith.txt";
int socket_fd;
int num=0;
mode_t mod;
mode_t mode;
char path[256],request[256],request1[256];
char *pathname[256];
char resperm[256];
char *mode1[256];
char *perm[256];
char ambient_perm[256];
char dir_perm[256];
//char *ambient_perm1;
//char *permissions[256];
    char new_permissions[256];
char new[10];
int capoid_index;
int capoid_index1;
int i,capguid_index,capid_index,getindex,array2[10],capguid[10];
char capoid[256];
int fd_final;
char capoid_recvd[256];
char buf[20];
int permissions (char *);
int min(char a, char b);




void die(char *socket_fd)
{
    perror(socket_fd);
    exit(1);
}

struct GCO
{
	char path1[256];
	char ambient_perm1[256];
	int uuid;
	int count;
}gc[520],*m,*n;


int getrand(int min,int max)
{
	return(min+rand()%(max-min));
}


/* send fd by socket(SERVER CODE)*/

static int send_fd(int socket, int fd)
{

/*Structure which holds all the information on what to send and recieve*/
	struct msghdr msg = { 0 };
	char buf[CMSG_SPACE(sizeof(fd))];
	memset(buf, '\0', sizeof(buf));
	char m_buffer[256];


/*Structure holding the I/O buffers*/
	struct iovec io = { .iov_base = m_buffer, .iov_len = sizeof(m_buffer) };
	msg.msg_iov = &io;
	msg.msg_iovlen = 1;
	msg.msg_control = buf;
	msg.msg_controllen = sizeof(buf);

/*Structure holding control message header*/

	struct cmsghdr * cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(sizeof(fd));

	*((int *) CMSG_DATA(cmsg)) = fd;

	msg.msg_controllen = cmsg->cmsg_len;

	if (sendmsg(socket, &msg, 0) < 0)
	die("Sending");
	return sendmsg(socket, &msg, 0);
}


int sending(){						/*------------SERVER------------*/
/*structure for sockaddress*/
	struct sockaddr_un address;
	int socket_fd = -1;
	int client = -1;
   	FILE *fp1 =NULL;
   	 char buffer[256];
   	 int nbytes;

/*create a socket*/

        socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	//printf("socket_fd=%d\n",socket_fd);

	if (socket_fd<0){
		die("socket");
	}

/*starting a clean address structure*/
	unlink(SOCKET_PATH);
	bzero(&address,sizeof(address));
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, SOCKET_PATH);


/*Socket binding*/
	if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0){

	    printf("bind() failed\n");
	    die("bind");
	}

/*Listening Connenction*/
	if (listen(socket_fd, 5) < 0){

	    printf("listen() failed\n");
	    die("listen");
	}


 /* Socket Accept*/


 	client = accept(socket_fd, NULL, NULL);

        printf("client() accepts socket_desc from wrapper: %d\n", client);
        fp1=fopen("daemonfile.txt","a");
        fprintf(fp1, "client() accept %d\n", client);
        fclose(fp1);
             // return socket_fd;
	connection_handler(client);
close(socket_fd);
return socket_fd;

}



int connection_handler(int socket_fd){

    int nbytes;
    char buffer[256],pathname[256],pathname1[256];
    char buffer1[256];
    int i,j;
    int len,len_amb,random=0;
char cwd[1024];

     /*list of calls */
    const char *call_name[] = {
        "cap_create",
        "creat",
     //   "open",
      //  "socket",
       // "delete",
        //"bind",
    };
    /*list completion */

    /*recevig the capability request*/
     memset(buffer, 0, 256);
//printf("socketfd : %d",socket_fd);
    nbytes = read(socket_fd, buffer, 256);
    buffer[nbytes] = 0;
    printf("%s",buffer);
    strcpy(request,buffer);
    printf("\nRequest received : %s\n", request);

    for(i=0;i<1;i++)
       if(strcmp(request,call_name[i])==0 && strcmp(request,call_name[0])==0)
    {
        printf("Call is found in the array at position %d %s\n", i,call_name[0]);
        write(socket_fd, buffer, nbytes);
    }



        printf("for test: %s\n",buffer);
        nbytes = read(socket_fd, buffer, 256);
        printf("%d\n",nbytes);
        buffer[nbytes] = 0;
        printf("%s\n",buffer);
        strcpy(pathname,buffer);
        printf("Resource specification : %s\n", pathname);
//printf("%s\n",buffer);
//memset(&buffer,'0',sizeof(buffer));
        write(socket_fd, buffer, nbytes);
    nbytes = read(socket_fd, buffer, 256);
    buffer[nbytes] = 0;
    strcpy(resperm,buffer);
    printf("Resource permission : %s\n", resperm);

	if (getcwd(cwd, sizeof(cwd)) != NULL)
        fprintf(stdout, "Current working dir: %s\n", cwd);
    /*    char *p;
       p= strtok(cwd,"/,0");
        while (p!= NULL)
 {
    printf ("%s\n",p);
   p = strtok (NULL, "/,0");
 }


       */
       const char ch = '/';
   	char *ret;

  	 ret = strrchr(cwd, ch);
  // printf("String after |%c| is - |%s|\n", ch, ret);
   	printf("PARENT DIRECTORY is :%s\n", ret);
/*To get previlleges of the user process*/

     	strcpy(ambient_perm,permissions(cwd));
	printf("%s\n",ambient_perm);

	if(dir_perm>=ambient_perm){

printf("FILE CAN BE CREATED IN THIS DIR....\n\n\n\n");
	}




	FILE *f=fopen("ro.txt","a");
	fprintf(f,"%s",ambient_perm);

	fclose(f);

	len_amb=strlen(ambient_perm);
	printf("Ambient_permissions length:%d\n",len_amb);
	len=strlen(resperm);
	printf("xml_permissions length:%d\n",len);



/*Check for SUBSET =? */

	 if(len==len_amb)
        {

            for(j=0;j<len;j++)
            {
                 // printf("%s\n",&ambient_perm[j]);
          	//printf("%s\n",&resperm[j]);
                new[j] = min(ambient_perm[j],resperm[j]);
             }

           printf("%s\n",new);
            printf("Capability Object creation  \n");
            printf("Resource name : %s\n", pathname);
            strcpy(gc[num].path1,pathname);
            //printf("num:%d\n",num);
            strcpy(gc[num].ambient_perm1, new);
            printf("Permission : %s\n", gc[num].ambient_perm1);
            FILE *fp1=fopen("mydaemonfile.txt","a");
            fprintf(fp1, "Creating Capability permissions for Global Capability Objects: %s\n\n", new);
            fclose(fp1);
        }
        else
        {
            printf("Object Cannot be created\n");
        }


        /*-----------------uuid --------------------------*/
        srand(time(NULL));	          /*srand(time(NULL)) should be run exactly once to intialise the PRNG*/
        random=getrand(MIN,MAX);
        gc[num].uuid=random;
       	printf("UUID : %d\n", gc[num].uuid);
           //printf("UUID : %d\n", random);
        gc[num].count=1;
        printf("Reference count : %d\n\n", gc[num].count);
	printf("hiii%s\n",&gc[num]);

            printf("num:%d\n",num);
        //printf("address is %p\n", &gc[num]);
      	m=&gc[0];
      	n=&gc[520];/*Contents int a[i],,, No. of process (num)*/
     		printf("address is %p %p\n", n, m);
     		capoid_index=n-m;
     		printf("capoid is : %d\n", capoid_index);

 	buffer1[nbytes]=0;
 	nbytes = snprintf(buffer1, 256, "%d", capoid_index);
        printf("Got the Operation spec .. sending back the CAPOID %d\n", capoid_index);
        write(socket_fd, buffer1, nbytes);
         printf("buffer :%s\n",buffer1);

         nbytes=read(socket_fd,buffer1,256);
         printf("%s\n",buffer1);

/*
         if(buffer1[0] == 'c'){
    		for(int k = 0; k < 256; k++){
    			buffer1[k] = buffer1[k + 6];
    		}
    	}
*/

          //printf("hi : %s\n",buffer1);
          strcpy(capoid_recvd,buffer1);
        //  printf("%s\n",capoid_recvd);
          printf("capoid test:%d\n",capoid_index);
       //   buf=itoa(capoid_index);
               // itoa (capoid_index,buf,10);
             // char *  itoa ( int value, char * str, int base );
             sprintf(buf, "%d", capoid_index);


          // printf("atoi:%s\n",buf);
             //nbytes=read(&buf,buf,20);
             //printf("nbytes: %d\n",nbytes);
          /*   for(j=0;j<=3;j++){
 	 		 if(capoid_recvd[j] == buf[j]){


       		fd_final=creat(gc[num].path1,gc[num].ambient_perm1);



    	}else
    	printf("hi_testing\n");

 	}	printf("Daemon Agrees to CREAT the file...");
 		printf("File to be created : %s\n", gc[num].path1);
 		printf("Permissions allowed : %s\n", gc[num].ambient_perm1);
 		printf("Authorization status : Sucess\n");
 			printf("final fd: %d\n",fd_final);

 			 			//if(gc[num].ambient_perm1==){//}
*/





return 0;
}


int min(char a, char b){
    if(a<=b)
        return a;
    else
	return b;
}

 int permissions(char *file){
    struct stat st;
    char *modeval = malloc(sizeof(char) * 9 + 1);
    if(stat(file, &st) == 0){
	//printf("hi");
        mode_t perm = st.st_mode;
        modeval[0] = (perm & S_IRUSR) ? 'r' : '-';
        modeval[1] = (perm & S_IWUSR) ? 'w' : '-';
        modeval[2] = (perm & S_IXUSR) ? 'x' : '-';
        modeval[3] = (perm & S_IRGRP) ? 'r' : '-';
        modeval[4] = (perm & S_IWGRP) ? 'w' : '-';
        modeval[5] = (perm & S_IXGRP) ? 'x' : '-';
        modeval[6] = (perm & S_IROTH) ? 'r' : '-';
        modeval[7] = (perm & S_IWOTH) ? 'w' : '-';
        modeval[8] = (perm & S_IXOTH) ? 'x' : '-';
        modeval[9] = '\0';
        return modeval;
    }
    else{
        return strerror(errno);
    	}

    }





/*daemon function */

int daemo(){
    pid_t pid = 0;
    pid_t sid = 0;

    int i = 0;
    pid = fork();		// fork a new child process

    if (pid < 0)
    {
        printf("fork failed!\n");
        exit(1);
    }

    if (pid > 0)		// its the parent process
    {
        printf("pid of child process %d \n", pid);
        exit(0); 		//terminate the parent process succesfully
    }

    umask(0);			//umasking the file mode

    sid = setsid();		//set new session
    if(sid < 0)
    {
        exit(1);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    FILE *fp1 = fopen ("mydaemonfile.txt", "a");
    fprintf(fp1,"%s\n", "logging details");
    fclose(fp1);

    	while(1)
	{
    	sending();
	//printf("file descriptor:",fd);
	}
    return 0;
}


/*Main fuction */

int main(void)
{	 char buffer[256];
   	 int nbytes;
   	 	int client = -1;

	//daemo();
	sending();

	connection_handler(client);
	int fd = creat(pathname,S_IWUSR|S_IRGRP|S_IROTH);
	 printf("CHECK UP\n");
   	 nbytes=snprintf(buffer,256,"%s","Checking: fd passing is correct");
    	 write(fd,buffer,nbytes);

       	nbytes = read(fd, buffer, 256);
    	printf("%s\n", buffer);

        buffer[nbytes] = 0;


	printf("Sending fd:%d \n",fd);
	if (fd < 0)
	    printf("Failed to creat file %s for reading\n", pathname);


        send_fd(client, fd);

    return 0;
}
