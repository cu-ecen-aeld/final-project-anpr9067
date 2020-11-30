
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

void sig_handler(int signo){
    if(signo == SIGINT){
        syslog(LOG_INFO, "Caught signal, exiting");
        exit(1);
    }
    if(signo == SIGTERM){
        syslog(LOG_INFO, "Caught signal, exiting");
        exit(1);
    }
}

int get_set_value(int fd, int sock){
	int readln, valsend;
	char *buffer = (char*)malloc(sizeof(char));
	char acc_val[1024];
	int i=0;
	
	while((int)buffer[0] != 10){
		readln = read(fd, buffer, 1);
		if(readln <0){
			printf("error in read\n");
			return -1;
		}
		acc_val[i] = buffer[0];
		i++;
	}

	valsend = send(sock , acc_val , i , 0 ); 
	
	if(valsend == -1){
		printf("Error in send\n");
		return -1;
	} 
	return 0;
}

int main(int argc, char const *argv[]) 
{ 

	
	char ipaddr[15] = "127.0.0.1";
	if(argv[1] == NULL){
		printf("default ipaddr");
	}else{
		strcpy(ipaddr, argv[1]);
		printf("%s\n", ipaddr);
	}

	struct sigaction sa;
    sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; /* Restart functions if
                                 interrupted by handler */
    if (sigaction(SIGINT, &sa, NULL) == -1){
        syslog(LOG_ERR, "Error in siginit");
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1){
        syslog(LOG_ERR, "Error in siginit");
    }
    syslog(LOG_INFO,"SIgnal check init \n");

	int sock = 0, valread; 
	int fd;//valsend, fd;//, readln;

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

		int val = get_set_value(fd, sock);
		if(val < 0){
			printf("error\n");
		}
		valread = read(sock , buffer, 1024); 
		if(valread == -1){
			printf("connection failed\n");
		}
		printf("%s\n",buffer); 
		
	}
	close(fd);
	return 0; 
} 