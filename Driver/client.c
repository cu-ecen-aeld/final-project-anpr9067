
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

	
	char ipaddr[15] = "127.0.0.1";
	if(argv[1] == NULL){
		printf("default ipaddr");
	}else{
		//ipaddr = argv[1];
		strcpy(ipaddr, argv[1]);
		printf("%s\n", ipaddr);
		/*if(strcmp(argv[1], "-d") == 0){
			//daemon process
			FLAG=1;
			syslog(LOG_INFO,"daemon mode");
		}else{
			syslog(LOG_INFO, "Invalid argument");
			exit(1);
		}*/
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
	//serv_addr.sin_addr.s_addr = inet_addr("10.0.0.136");
	serv_addr.sin_port = htons(PORT); 

	if(inet_pton(AF_INET, ipaddr, &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
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