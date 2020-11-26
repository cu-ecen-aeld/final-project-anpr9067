// Client side C/C++ program to demonstrate Socket programming 
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
#include <pthread.h>

#define PORT 9000 

pthread_mutex_t mutex;

/*int send_char(){
	
    return 0;
}*/


int main(int argc, char const *argv[]) 
{ 

	printf("inside main\n");
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
	printf("after daemon check\n");
	if (pthread_mutex_init(&mutex, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return -1; 
    } 

    printf("pthread_mutex_init\n");
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	//char *hello = "Hello from client\n"; 
	char buffer1[1024] = {0}; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	printf("socket\n");
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 

	printf("deamon start\n");
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
    printf("daemin end\n");
    while(1){
    	printf("inside while 1\n");
	    int readln, valsend;
		char *buffer = (char*)malloc(sizeof(char));
		int fd = open("/var/tmp/accdata.txt", O_RDWR|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
	    if(fd<0){
	    	printf("file error\n");	
	    	syslog(LOG_INFO,"Error in opening file");
	    	return -1;
	    }
	    printf("opened file");
	    pthread_mutex_lock(&mutex);
	    //while((int)buffer[0]!=10){
	    	//printf("inside while");
	    	readln = read(fd, buffer, 1);
	    	if(readln<0){
	    		syslog(LOG_INFO, "error in read");
	    	}
	    	valsend = send(sock, buffer, 1, 0);
	    	if(valsend <0){
				syslog(LOG_INFO, "error in send");
			}
		//}//
		//printf("outside while");
		//send(sock , hello , strlen(hello) , 0 ); 
		//printf("Hello message sent\n"); 
		valread = read( sock , buffer1, 1024); 
		if(valread == -1){
			printf("connection failed");
		}
		//printf("%s\n",buffer );
		if(strcmp(buffer1, "received") != 0){
			printf("Data error\n");
		}

	    pthread_mutex_unlock(&mutex);
	    close(fd);
	}
	return 0; 
} 
