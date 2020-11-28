/*
/ @description: Client socket program
/ @date: 12th November 2020
/ @author: Pavan Shiralagi
/ Reference: https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
*/
/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define MAX 20
#define PORT 9000
#define SA struct sockaddr

int main(int argc, char *argv[]) 
{ 
	int sockfd, i; 
	struct sockaddr_in servaddr; 
	for(i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			printf("Daemon\n\r");
			daemon(1, 1);
		}	
	}
	// socket create and varification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
	printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(PORT); 

	// connect the client socket to server socket 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
		} 
		else
			printf("connected to the server..\n"); 

	printf("Before while..\n"); 
	while(1)
	{

		printf("inside while\n");
		char buffer[1024];
		int valsend = send(sockfd , "hello\n" , strlen("hello\n") , 0 ); 
		printf("Hello message sent: %d\n", valsend);
		if(valsend == -1){
			printf("Error in send\n");
		} 
		int valread = read( sockfd , buffer, 1024); 
		if(valread == -1){
			printf("connection failed\n");
		}
		printf("buffer: %s\n",buffer );
	}
 
	close(sockfd); 
}*/

#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <signal.h>

#define PORT 9000 


int main(int argc, char const *argv[]) 
{ 

	int FLAG=0;
	if(argv[1] == NULL){
		FLAG = 0;
		//without daemon
	}else{
		if(strcmp(argv[1], "-d") == 0){
			//daemon process
			FLAG=1;
			syslog(LOG_INFO,"daemon mode");
		}else{
			syslog(LOG_INFO, "Invalid argument");
			exit(1);
		}
	}

	int sock = 0, valread; 
	int valsend, fd, readln;

	struct sockaddr_in serv_addr; 
	//char *hello = "Hello from client\n"; 
	char buffer[1024] = {0}; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 

	if(inet_pton(AF_INET, INADDR_ANY, &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 

	if(FLAG == 1){
    	syslog(LOG_INFO,"starting daemon");
    	pid_t pid;//, sid;
    	pid = fork();
    	if(pid<0){
    		syslog(LOG_ERR, "Error in creating child");
    		exit(1);
    	}
        /* If we got a good PID, then
           we can exit the parent process. */
        if (pid > 0) {
                syslog(LOG_INFO, "Child created");
                exit(0);
        }
        /* Change the file mode mask */
        umask(0);
        /* Open any logs here */               
        /* Create a new SID for the child process */
        //sid = ;
        if (setsid() < 0) {
                /* Log the failure */
        		syslog(LOG_ERR, "Error in creating SID for child process");
                exit(EXIT_FAILURE);
        }
        /* Change the current working directory */
        if ((chdir("/")) < 0) {
                syslog(LOG_ERR, "Error in changing current directory");
                exit(EXIT_FAILURE);
        }
        
        /* Close out the standard file descriptors */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
   	
   	fd = open("/var/tmp/accdata.txt", O_RDWR|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
    	if(fd<0){
        	printf("Error in opening file");
    	}
    while(1){

    	
    	// Convert IPv4 and IPv6 addresses from text to binary form 
        readln = read(fd, buffer, 1);
        if(readln<0){
        	printf("Error in read\n");
        }
        printf("read messgae: %s  \n", buffer);
		valsend = send(sock , buffer , 1 , 0 ); 
		printf("message sent\n");
		if(valsend == -1){
			printf("Error in send\n");
		} 
		valread = read( sock , buffer, 1024); 
		if(valread == -1){
			printf("connection failed\n");
		}
		printf("%s\n",buffer ); 
		
	}
	close(fd);
	return 0; 
} 