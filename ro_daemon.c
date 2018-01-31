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
#define R "READ"
#define W "WRITE"
#define X "EXECUTE"
FILE *fp1;
FILE *fp2;
static const char * const SOCKET_PATH = "my_path";
//static const char * const filename = "rohith.txt";
int socket_fd;
int client = -1;
int num=0,res;
mode_t mod;
mode_t mode;
char path[256],path_new[256],mode_new[256],request[256],request1[5];
char *pathname[256],*pathname1[256];
char resperm[256];
char *mode1[256];
char *perm[256];
char ambient_perm[256];
char resperm[256];
char dir_perm[256];
int temp(const char *path);
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
char getperm[256];
int permissions (char *);
int min(char a, char b);
int capid[256];
int oflag;
int remap();
int flag1,p,s;
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

static int send_fd(int socket_fd, int fd)
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

	if (sendmsg(socket_fd, &msg, 0) < 0)
	die("Sending");
	return sendmsg(socket_fd, &msg, 0);
}


void sending(){						/*------------SERVER------------*/
/*structure for sockaddress*/
	struct sockaddr_un address;
	int socket_fd = -1;

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
return 0;

}



int connection_handler(int socket_fd){
  //char buffer[256];
     	// int nbytes;
     	 	int client = -1;

    int nbytes;
    char buffer[256],pathname[256],pathname1[256];
    char buffer1[256];
    int i,j;
    int len,len1,len_amb,random=0;
char cwd[1024];
char first_request[256],request2[256],request1[256];
char creation_permission[256];
     /*list of calls */
    const char *call_name[] = {
        "cap_create",
        "creat",
        "open",
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
//    printf("%s",buffer);
    strcpy(request,buffer);
    printf("\nRequest received : %s\n\n", request);

    for(i=0;i<2;i++)
       if(strcmp(request,call_name[i])==0 && strcmp(request,call_name[0])==0)
    {
        printf("Call is found in the array at position %d %s\n\n", i,call_name[0]);
        write(socket_fd, buffer, nbytes);
    }



    nbytes = read(socket_fd, buffer, 256);
    buffer[nbytes] = 0;
//    printf("%s",buffer);
    strcpy(request1,buffer);
    printf("\nRequest received : %s\n\n", request1);
    buffer[nbytes] = 0;

     if((request1[0]=='c') &&  (request1[1]=='r') &&(request1[2]=='e') &&(request1[3]=='a') &&(request1[4]=='t')){
      //  printf("for test: %s\n",buffer);

      nbytes=snprintf(buffer, 256, "Capd : Request received for '%s' !! Waiting for path and permission", request);
      write(socket_fd, buffer, nbytes);

      //printf("hi hw r u \n");
      memset(request1, 0, 5);
      nbytes = read(socket_fd, buffer, 256);
      buffer[nbytes] = 0;
      strcpy(pathname,buffer);
      printf("Resource description : %s\n", pathname);


      nbytes = snprintf(buffer, 256, "CapD : Got the Pathname '%s'....waiting for operation spec", pathname1);
      write(socket_fd, buffer, nbytes);

        //
        // nbytes = read(socket_fd, buffer, 256);
        // buffer[nbytes] = 0;
        // strcpy(pathname,buffer);
        // printf("Resource specification : %s\n", pathname);




//printf("%s\n",buffer);
//memset(&buffer,'0',sizeof(buffer));
        write(socket_fd, buffer, nbytes);
    nbytes = read(socket_fd, buffer, 256);
    buffer[nbytes] = 0;
    strcpy(resperm,buffer);
    printf("Resource permission : %s\n\n", resperm);



    /* ------------------------CREAT--------------------------------------*/

	if (getcwd(cwd, sizeof(cwd)) != NULL)
        fprintf(stdout, "Current working dir: %s\n\n", cwd);

        const char ch = '/';
   	char *ret;

  	 ret = strrchr(cwd, ch);
  // printf("String after |%c| is - |%s|\n", ch, ret);
   	printf("PARENT DIRECTORY is :%s\n\n", ret);
/*To get previlleges of the user process*/

     	strcpy(ambient_perm,permissions(cwd));
	printf("Ambient Permissions: %s\n\n",ambient_perm);

	FILE *f=fopen("ro.txt","a");
	fprintf(f,"%s",ambient_perm);

	fclose(f);

	len_amb=strlen(ambient_perm);
	printf("Ambient permission's length:%d\n",len_amb);
	len=strlen(resperm);
	printf("Resource permission's length:%d\n\n",len);




/*Check for SUBSET =? */

	 if(len==len_amb)
        {

            for(j=0;j<len;j++)
            {
                 // printf("%s\n",&ambient_perm[j]);
          	//printf("%s\n",&resperm[j]);
                new[j] = min(ambient_perm[j],resperm[j]);
             }

            printf("Minimum Previllege: %s\n\n",new);
            printf("Capability Object creation  \n\n");
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
	      //printf("hiii%s\n",&gc[num]);

          //  printf("num:%d\n",num);
        //printf("address is %p\n", &gc[num]);
      	m=&gc[0];
      	n=&gc[num];/*Contents int a[i],,, No. of process (num)*/
     		//printf("address is %p %p\n", n, m);
     		capoid_index=n-m;
     		//printf("capoid is : %d\n", capoid_index);
/*mapping starts*/
  for (i = 0; i <=num; i++ ) {
    capguid[i] = capoid_index;
	}

	for (i = 0; i <=num; i++){
    if (capoid_index == capguid[i]){
      capguid_index = i;
    }
  }

  //printf("capguid is : %d\n", capguid_index);
	for (i = 0; i <=num; i++ ) {
		capid[0]=capguid_index;
	}

	for (i = 0; i <=num; i++){
  	if (capguid_index == capid[i]){
      capid_index = i;
    }
  }
	//printf("capid is : %d\n\n", capid_index);
	res=capid_index;

  /* mapping ends */
 	      buffer[nbytes]=0;
 	      nbytes = snprintf(buffer, 256, "%d", res);
        printf("Got the Operation spec .. sending back the CAPOID %d\n\n", capoid_index);
        write(socket_fd, buffer, nbytes);
          printf("capoid is : %d\n", res);

        //printf("buffer :%s\n",buffer);
        buffer[nbytes] = 0;
        nbytes=read(socket_fd,buffer,256);
                buffer[nbytes] = 0;
        strcpy(first_request,buffer);
        //printf("%s\n",buffer);
          printf("FIRST MESSAGE FROM CLIENT: %s\n",first_request);
          write(socket_fd, buffer, nbytes);
      //

          nbytes = read(socket_fd, buffer, 256);
          buffer[nbytes] = 0;
          strcpy(path_new,buffer);          //strcpy(gc[num].path1, buffer);
          printf("File to be created: %s\n", path_new);
        //  printf("new buffer : %s\n", buffer);
          write(socket_fd, buffer, nbytes);

//              nbytes = snprintf(buffer, 256, "CapD : Got the path....waiting for file flag");
  //            write(socket_fd, buffer, nbytes);
          buffer[nbytes] = 0;
        //buffer[nbytes] = 0;
      //  printf("buffer testing only: %s\n",buffer);

            nbytes = read(socket_fd, buffer, 256);
            buffer[nbytes] = 0;
            strcpy(creation_permission,buffer);
            write(socket_fd, buffer, nbytes);

          // strcpy(gc[num].path1, buffer);
            printf("Permissions with which file is created: %s\n", creation_permission);
          //  printf("File path : %s\n", gc[num].path1);
      //    printf("HI Iam going to creat the file !!!!!11\n\n");

          printf("CapD : Going to creat the file !!!!!\n\n");
          int fd = creat(path_new,S_IWUSR|S_IRGRP|S_IROTH);
          printf("File Descriptor(fd) :%d\n",fd);
          // printf("CHECK UP\n");
             //nbytes=snprintf(buffer,256,"%s","Checking: fd passing is correct");
             //  write(fd,buffer,nbytes);

                //nbytes = read(fd, buffer, 256);
             //  printf("%s\n", buffer);


remap();
  //printf("%c\n", gc[p].ambient_perm1[2]);
  //printf("%s\n", gc.ambient_perm1);
	/*communicate to user*/
	//for(i=0;i<strlen(gc[p].ambient_perm1);i++){
  //  if(gc[p].ambient_perm1[1]=='r' && gc[p].ambient_perm1[2]=='w')
	//	  flag1=1;
	//	}
	//printf("flag is %d\n", flag1);
	/*if(flag1==1){
    //printf("CapD : User requested file is open with following permissions \t %s\t %s\t, to continue (Y/n)", R,W);
   	nbytes = snprintf(buffer, 512, "CapD : User program is trying to open the following files with the following permissions : \n\n\t %s\t %s,%s\n\n to continue press 'Y', else 'N'", path,R,W);
   	write(socket_fd, buffer, nbytes);
  }
  else{
   // nbytes = snprintf(buffer, 512, "CapD : User requested file is open with following permissions \t\n %s\t\n, to continue (Y/n)", R);
   	write(socket_fd, buffer, nbytes);
  }
*/
          printf("Sending file descriptor:%d \n",fd);
          if (fd < 0)
              printf("Failed to creat file %s for reading\n", pathname);
	else{
//printf("skt,fd:%d,%d\n",socket_fd,fd);
                send_fd(socket_fd, fd);
             }

}
else if((request1[0]=='o') &&  (request1[1]=='p') &&(request1[2]=='e') &&(request1[3]=='n')){

/*----------------------------------- OPEN -----------------------------------------------*/
        nbytes=snprintf(buffer, 256, "Capd : Request received for '%s' !! Waiting for path and permission", request);
        write(socket_fd, buffer, nbytes);

    //printf("hi hw r u \n");
    memset(request1, 0, 5);
    nbytes = read(socket_fd, buffer, 256);
    buffer[nbytes] = 0;
    strcpy(pathname1,buffer);
    printf("Resource description : %s\n", pathname1);

        nbytes = snprintf(buffer, 256, "CapD : Got the Pathname '%s'....waiting for operation spec", pathname1);
        write(socket_fd, buffer, nbytes);

    nbytes = read(socket_fd, buffer, 256);
    buffer[nbytes] = 0;
    strcpy(resperm,buffer);
    printf("Resource description : %s, %s\n", pathname1,resperm);

		/* start of mapping*/

        temp(pathname1);
        fp1=fopen("mydaemonfile.txt","a");
        fprintf(fp1, "FSTAT: PERMISSION OBTAINED FROM FSTAT: %s\n", getperm);
        fclose(fp1);

        len=strlen(resperm);
        len1=strlen(getperm);
        //printf("%d\t%d\n", len,len1);
        if(len==len1)
        {
            for(j=0;j<len;j++)
            {
                new[j]=min(getperm[j],resperm[j]);
                //printf("%d\n", new[j]);
            }
            //printf("new is %c\n", new[2]);
            /*converting to mode */


           mode = (((new[1] == 'r') * 4 | (new[2] == 'w') * 2 | (new[3] == 'x')) << 6) |
              (((new[4] == 'r') * 4 | (new[5] == 'w') * 2 | (new[6] == 'x')) << 3) |
              (((new[7] == 'r') * 4 | (new[8] == 'w') * 2 | (new[9] == 'x')));

            //printf("mode accordng to mod is %o\n", mode);
            /*end of code */

            printf("Capability Object creation  \n");
            printf("Resource name : %s\n", pathname1);
            strcpy(gc[num].ambient_perm1, new);
            printf("Permission : %s\n", gc[num].ambient_perm1);
            fp1=fopen("mydaemonfile.txt","a");
            fprintf(fp1, "Creating Capability permissions for Global Capability Objects: %s\n\n", new);
            fclose(fp1);
        }
        else
        {
            printf("Object Cannot be created\n");
        }

        srand(time(NULL));
        random=getrand(MIN,MAX);
        gc[num].uuid=random;
        printf("UUID : %d\n", gc[num].uuid);
        gc[num].count=1;
        printf("Reference count : %d\n\n", gc[num].count);



        //printf("address is %p\n", &gc[num]);
      	m=&gc[0];
      	n=&gc[num];
     		//printf("address is %p %p\n", n, m);
     		capoid_index=n-m;
     		//printf("capoid is : %d\n", capoid_index);

     		/*mapping starts*/
      	for (i = 0; i <=num; i++ ) {
    		capguid[i] = capoid_index;
			}


				for (i = 0; i <=num; i++){
    			if (capoid_index == capguid[i]){
        		capguid_index = i;
     		}
		}
		printf("capguid is : %d\n", capguid_index);


		for (i = 0; i <=num; i++ ) {
			capid[0]=capguid_index;
		}


		for (i = 0; i <=num; i++){
    		if (capguid_index == capid[i]){
        		capid_index = i;
     		}

		}
		printf("capid is : %d\n\n", capid_index);
		res=capid_index;
		/* mapping ends */

		/*continuing the request reply*/
        nbytes = snprintf(buffer, 256, "%d", res);
        printf("Got the Operation spec .. sending back the CAPID %d\n", res);
        write(socket_fd, buffer, nbytes);

    nbytes = read(socket_fd, buffer, 256);
    buffer[nbytes] = 0;
    getindex=atoi(buffer);


        nbytes = snprintf(buffer, 256, "CapD : Got the CAPID....waiting for the call to be made");
        write(socket_fd, buffer, nbytes);


    nbytes = read(socket_fd, buffer, 256);
    buffer[nbytes] = 0;
    strcpy(request2,buffer);
    printf("Request received : %s %d\n", request2,capid_index);
  //  for(j=0;j<5;j++)
  //  if(strcmp(request2,call_name[j])==0 && strcmp(request2,call_name[1])==0 && strcmp(request2,call_name[0])!=0)
//   {
  //      printf("Call is found in the array at position %d %s\n", j,call_name[1]);
  //     nbytes=snprintf(buffer, 256, "Capd : Request received for '%s' !! Waiting for path and permission", request2);
      write(socket_fd, buffer, nbytes);
//
    buffer[nbytes] = 0;

    nbytes = read(socket_fd, buffer, 256);
    buffer[nbytes] = 0;
    strcpy(path,buffer);
    strcpy(gc[num].path1, buffer);
    printf("FIRST MESSAGE FROM CLIENT: %s\n", path);
    printf("File path : %s\n", gc[num].path1);

        nbytes = snprintf(buffer, 256, "CapD : Got the path....waiting for file flag");
        write(socket_fd, buffer, nbytes);

    nbytes = read(socket_fd, buffer, 256);
    buffer[nbytes] = 0;
    oflag=atoi(buffer);
    printf("File flag : %d\n", oflag);


        nbytes = snprintf(buffer, 256, "CapD : Got the flag....waiting for mode");
        write(socket_fd, buffer, nbytes);

  	nbytes = read(socket_fd, buffer, 256);
    buffer[nbytes] = 0;
    mode=atoi(buffer);
    printf("File mode : %o\n", mode);
    //snprintf(mode1,10,"%d",mode);

        nbytes = snprintf(buffer, 256, "CapD : Got the mode....waiting for operation permissions");
        write(socket_fd, buffer, nbytes);

    nbytes = read(socket_fd, buffer, 256);
    buffer[nbytes] = 0;
    strcpy(perm,buffer);
    //printf("FOURTH MESSAGE FROM CLIENT: %s\n", perm);
    printf("Capid : %d\n\n", getindex);


    	/*new code strats*/

    	 /*new code authorization*/



        /*calling the remap function */
		remap();
        //printf("%c\n", gc[p].getperm1[2]);
        //printf("%s\n", gc.getperm1);
		/*communicate to user*/
		for(i=0;i<strlen(gc[p].ambient_perm1);i++)
			{
				if(gc[p].ambient_perm1[1]=='r' && gc[p].ambient_perm1[2]=='w')
				flag1=1;
			}
			printf("flag is %d\n", flag1);


   			// if(flag1==1)
   			// {
   			// 		//printf("CapD : User requested file is open with following permissions \t %s\t %s\t, to continue (Y/n)", R,W);
   			// 		 //nbytes = snprintf(buffer, 512, "CapD : User program is trying to open the following files with the following permissions : \n\n\t %s\t %s,%s\n\n to continue press 'Y', else 'N'", path,R,W);
   			// 		 write(socket_fd, buffer, nbytes);
   			// }
   			// else
   			// {
   			// 		nbytes = snprintf(buffer, 512, "CapD : User requested file is open with following permissions \t\n %s\t\n, to continue (Y/n)", R);
   			// 		write(socket_fd, buffer, nbytes);
   			// }

		/*ends*/

 //	}
  printf("CapD : Going to open the file %s !!!!!\n\n",gc[num].path1);
  int fd = open(gc[num].path1,O_WRONLY|O_APPEND,S_IRWXU);

  printf("File Descriptor(fd) :%d\n",fd);
  if (fd < 0)
      printf("Failed to open file %s for reading\n",gc[num].path1 );
else{
       //printf("skt,fd:%d,%d\n",socket_fd,fd);
        send_fd(socket_fd, fd);
     }
  //send_fd(socket_fd, fd);
   	fp1=fopen("mydaemonfile.txt","a");
   	fprintf(fp1,"MESSAGE (PATH) FROM CLIENT: %s\n", path);
 	fprintf(fp1,"MESSAGE (FLAG) FROM CLIENT: %d\n", oflag);
 	fprintf(fp1,"MESSAGE (MODE) FROM CLIENT: %o\n", mode);
        fprintf(fp1,"MESSAGE (PERMISSION) FROM CLIENT: %s\n", perm);
 	fclose(fp1);
	close(socket_fd);
 	//return 0;

}
return 0;
}



int remap(){
	/* mapping starts */
	s= capid[getindex];
	//printf("mapping of capid to capguid is %d\n", s);
	p=capguid[s];
	//printf("mapping capguid to capoid is %d\n", p);
	printf("Capability objects accessed : %s, %s, %d, %d\n", gc[p].path1,gc[p].ambient_perm1,gc[p].uuid,gc[p].count);
	/*end of code*/

	//if(getindex==index2 && array2[getindex]==index1 && array1[index1]==capoid){
	//printf("Mapping Successful\n\n");
	return 0;
}



int min(char a, char b){
    if(a<=b)
        return a;
    else
	return b;
}
int temp(const char *path)
{
	struct stat filestat;
	if(stat(path,&filestat)<0)
		return 1;
	strcpy(getperm,"");

	//printf((S_ISDIR(filestat.st_mode)) ? "d" : "-");
	strcat(getperm,(S_ISDIR(filestat.st_mode)) ? "d" : "-");
	//printf((filestat.st_mode & S_IRUSR) ? "r" : "-");
	strcat(getperm,(filestat.st_mode & S_IRUSR) ? "r" : "-");
	//printf((filestat.st_mode & S_IWUSR) ? "w" : "-");
	strcat(getperm,(filestat.st_mode & S_IWUSR) ? "w" : "-");
	//printf((filestat.st_mode & S_IXUSR) ? "x" : "-");
	strcat(getperm,(filestat.st_mode & S_IXUSR) ? "x" : "-");
	//printf((filestat.st_mode & S_IRGRP) ? "r" : "-");
	strcat(getperm,(filestat.st_mode & S_IRGRP) ? "r" : "-");
	//printf((filestat.st_mode & S_IWGRP) ? "w" : "-");
	strcat(getperm,(filestat.st_mode & S_IWGRP) ? "w" : "-");
	//printf((filestat.st_mode & S_IXGRP) ? "x" : "-");
	strcat(getperm,(filestat.st_mode & S_IXGRP) ? "x" : "-");
	//printf((filestat.st_mode & S_IROTH) ? "r" : "-");
	strcat(getperm,(filestat.st_mode & S_IROTH) ? "r" : "-");
	//printf((filestat.st_mode & S_IWOTH) ? "w" : "-");
	strcat(getperm,(filestat.st_mode & S_IWOTH) ? "w" : "-");
	//printf((filestat.st_mode & S_IXOTH) ? "x" : "-");
	strcat(getperm,(filestat.st_mode & S_IXOTH) ? "x" : "-");
	//printf("\n");
	//strcpy(gc.getperm1, getperm);
	printf("fstat output : %s\n\n",getperm);
	//printf("fstat: Permission bits of %s is %s\n", path,gc[p].getperm1);
}

/*end of fstat*/
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
{

	//daemo();
	sending();



    return 0;
}
