/*
* NAME : aesdscoket.c
* REFERENCE : https://www.geeksforgeeks.org/socket-programming-cc/
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define PORT 9000

void sig_handler(int signo){
    if(signo == SIGINT){
        syslog(LOG_INFO, "Caught signal, exiting");
        if (remove("/var/tmp/aesdsocketdata.txt") ==0){
            syslog(LOG_INFO, "Deleted /var/tmp/aesdsocketdata");
        }
        else{
            syslog(LOG_INFO, "Unable to delete the file");
        }
        exit(1);
    }
    if(signo == SIGTERM){
        syslog(LOG_INFO, "Caught signal, exiting");
        if (remove("/var/tmp/aesdsocketdata.txt") ==0){
            syslog(LOG_INFO, "Deleted /var/tmp/aesdsocketdata");
        }
        else{
            syslog(LOG_INFO, "Unable to delete the file");
        }
        exit(1);
    }
}



int data_integrity(char buffer[]){
    //printf("Inside function\n");
    char* token = strtok(buffer, ", ");
    char * ip[3];
    int i=0;
    while(token != NULL){
        ip[i] = token;
        token = strtok(NULL, ", ");
        i++;
    }

    if((int)ip[0][0] != 88){
        return -1;
    }else{
        char* token1 = strtok(ip[0], ":");
        char * ip1[2];
        int j=0;
        while(token1 != NULL){
            ip1[j] = token1;
            token1 = strtok(NULL, ":");
            j++;
        }
        int X = atoi(ip1[1]);
        if(X <-32767 || X >32767){
            return -1;
        }else
            return 0;
    }

    if((int)ip[1][0] != 89){
        return -1;
    }else{
        char* token2 = strtok(ip[1], ":");
        char * ip2[2];
        int k=0;
        while(token2 != NULL){
            ip2[k] = token2;
            token2 = strtok(NULL, ":");
            k++;
        }
        int Y = atoi(ip2[1]);
        if(Y<-32767 || Y>32767){
            return -1;
        }else
            return 0;
    }

    if((int)ip[2][0] != 90){
        return -1;
    }
    else{
        char* token3 = strtok(ip[2], ":");
        char * ip3[2];
        int l=0;
        while(token3 != NULL){
            ip3[l] = token3;
            token3 = strtok(NULL, ":");
            l++;
        }
        int Z = atoi(ip3[1]);
        if(Z<-32767 || Z>32767){
            return -1;
        }else
            return 0;
    }
    return 0;
}

int main(int argc, char const *argv []){

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

    openlog("aesdsocket.c", LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "CHECKING SYSLOG IS OPENED FOR SOCKET TESTING");
    
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
    
    int server_fd, new_socket, valrecv, valsend; 
    struct sockaddr_in cli_address, serv_address;
    unsigned int clntLen;
    int opt = 1; 
    unsigned int myPort;
    //int addrlen = sizeof(address); 
    char myIP[16];
    char buffer[1024];
    //char acc_val[1024];
    char *buff = "received\n";
    int recvMsgSize, fd;      

    syslog(LOG_INFO,"setsockopt created \n");
    memset(&serv_address, 0, sizeof(serv_address));
    serv_address.sin_family = AF_INET; 
    serv_address.sin_addr.s_addr = INADDR_ANY; 
    serv_address.sin_port = htons( PORT );
    syslog(LOG_INFO,"After memset \n"); 

    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        syslog(LOG_INFO,"socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    syslog(LOG_INFO,"Socket created \n");
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) 
    { 
        syslog(LOG_INFO,"setsockopt"); 
        exit(EXIT_FAILURE); 
    }
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&serv_address,  
                                 sizeof(serv_address))<0) 
    { 
        syslog(LOG_INFO,"bind failed"); 
        exit(EXIT_FAILURE); 
    }
    syslog(LOG_INFO,"Daemon process \n");
    // creating daemon
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
    syslog(LOG_INFO,"after daemon \n");
    if (listen(server_fd, 10) < 0) 
    { 
        syslog(LOG_INFO,"listen"); 
        exit(EXIT_FAILURE); 
    } 
    syslog(LOG_INFO,"Listening \n");
    clntLen = sizeof(cli_address);
    
    new_socket = accept(server_fd, (struct sockaddr *)&cli_address,  &clntLen);
    
    syslog(LOG_INFO,"%d new_socket\n", new_socket);
    if (new_socket==-1) 
    { 
        syslog(LOG_INFO,"accept"); 
        exit(EXIT_FAILURE);
    }
    syslog(LOG_INFO,"Connection accepted");

    bzero(&cli_address, sizeof(cli_address));

    getsockname(new_socket, (struct sockaddr *) &cli_address, &clntLen);
    inet_ntop(AF_INET, &cli_address.sin_addr, myIP, sizeof(myIP));
    myPort = ntohs(cli_address.sin_port);

    syslog(LOG_INFO,"connected to ip address: %s\n", myIP);
    syslog(LOG_INFO,"connected to Local port : %u\n", myPort);

    while(1){

        valrecv = recv(new_socket , buffer, BUFFER_SIZE, 0);
       
        if(valrecv <0){
            syslog(LOG_INFO, "receive failed");
        }

        int val = data_integrity(buffer);
        if(val<0){
            buff = "Courrupt data sent\n";
        }else{
            buff = "Valid data recieved\n";
        }
        printf("outside function\n");
        fd = open("/var/tmp/aesdsocketdata.txt", O_RDWR|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
        if(fd<0){
            syslog(LOG_INFO,"Error in opening file");
        }
        
        recvMsgSize = write(fd, buffer, valrecv);
        if(recvMsgSize<0){
            syslog(LOG_INFO, "error in write\n");
        }
        syslog(LOG_INFO,"write done: %d\n", recvMsgSize);
        valsend = send(new_socket, buff, strlen(buff), 0);
        if(valsend <0){
            syslog(LOG_INFO, "error in send\n");
        }       
        close(fd);
    }
    close(new_socket);
    
    closelog();
    return 0; 
}