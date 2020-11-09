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

#include <pthread.h>
#include "queue.h"

#include <time.h>

#define BUFFER_SIZE 4096
#define PORT 9000

//#define USE_AESD_CHAR_DEVICE 1

int total_bytes=0;
int server_fd, new_socket, system_exit_flag=0;
#ifdef USE_AESD_CHAR_DEVICE
#else
    pthread_mutex_t mutex;
#endif

typedef struct slist_data_s slist_data_t;

struct slist_data_s {
    pthread_t thread_id;
    int thread_flag;
    SLIST_ENTRY(slist_data_s) entries;
};

void sig_handler(int signo){
	system_exit_flag = 1;
	if(signo == SIGINT){
		syslog(LOG_INFO, "Caught signal, exiting");
	}
	if(signo == SIGTERM){
		syslog(LOG_INFO, "Caught signal, exiting");
	}
	printf("before shutdown\n");
	shutdown(server_fd,SHUT_RDWR);
	printf("after shutdown\n");
}

void* thread_handler(void* param){
	slist_data_t* handler_param =  (slist_data_t *) param;
	char *buffer;
    //char *buffer1;
    int recvMsgSize, fd,readln, valrecv, valsend; 
    int byte_size=0;
   // int i=0;
    buffer = (char*) malloc(BUFFER_SIZE*sizeof(char));
	while(1){
		if((int)buffer[byte_size-1] == 10){
			buffer[byte_size] = '\0';
			break;
		}
		valrecv = recv(new_socket , buffer+byte_size, BUFFER_SIZE, 0);
		byte_size = byte_size+valrecv;
		//buffer = (char*)realloc(buffer, (sizeof(char)*BUFFER_SIZE)+1);
	}
    if(valrecv <0){
    	syslog(LOG_INFO, "receive failed");
    }
#ifdef USE_AESD_CHAR_DEVICE
    fd = open("/dev/aesdchar", O_RDWR|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
#else
    //fd = open("/dev/aesdchar", O_RDWR|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
    fd = open("/var/tmp/aesdsocketdata.txt", O_RDWR|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
#endif
    if(fd<0){
    	syslog(LOG_INFO,"Error in opening file");
    }else{
#ifdef USE_AESD_CHAR_DEVICE
#else
    	pthread_mutex_lock(&mutex);
#endif
    	total_bytes += byte_size;
    	recvMsgSize = write(fd, buffer, byte_size);

    	if(recvMsgSize<0){
    		syslog(LOG_INFO, "error in write");
    	}
#ifdef USE_AESD_CHAR_DEVICE
#else    	
    	pthread_mutex_unlock(&mutex);
#endif    
    	byte_size = 0;
        lseek(fd, 0, SEEK_SET);
    	readln = read(fd, buffer, total_bytes);
    	if(readln<0){
    		syslog(LOG_INFO, "error in read");
    	}
    	buffer[total_bytes] = '\0';
    	valsend = send(new_socket, buffer, strlen(buffer), 0);
    	if(valsend <0){
    		syslog(LOG_INFO, "error in send");
		}		    			    	
    }
	close(fd);
	free(buffer);

	handler_param->thread_flag =1;
	return NULL;
}
#ifdef USE_AESD_CHAR_DEVICE
#else
void timer_handler(){

		syslog(LOG_INFO, "Timer handler");
		int fd, writer_ptr;
		time_t t ; 
	    struct tm *tmp ; 
	    char MY_TIME[50]; 
	    char times[50] = "timestamp:";
	    time( &t ); 
	    tmp = localtime( &t ); 
	      
	    // using strftime to display time 
	    strftime(MY_TIME, sizeof(MY_TIME), "%c\n", tmp); 
	    strcat(times, MY_TIME);
	    //printf("%s\n", times ); 
	    //sleep(10);
	    fd = open("/var/tmp/aesdsocketdata.txt", O_RDWR|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
	    if(fd<0){
	    	syslog(LOG_INFO,"Error in opening file");
	    }else{
	    	pthread_mutex_lock(&mutex);
	    	writer_ptr = write(fd, times, strlen(times));
	    	total_bytes += strlen(times);
	    	if(writer_ptr<0){
	    		syslog(LOG_ERR, "error in printing timestamp");
	    	}
	    	close(fd);
	    	pthread_mutex_unlock(&mutex);
	    	syslog(LOG_INFO, "%s\n", times );
	    }

}
#endif

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

	syslog(LOG_INFO,"Error check complete \n");
	openlog("aesdsocket.c", LOG_CONS, LOG_USER);
	syslog(LOG_INFO, "CHECKING SYSLOG IS OPENED FOR SOCKET TESTING");

#ifdef USE_AESD_CHAR_DEVICE
#else
	/*Timer initialization*/
   	timer_t timerid;
#endif

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    syslog(LOG_INFO,"SIgnal check init \n"); 
    struct sockaddr_in cli_address, serv_address;
    unsigned int clntLen;
    int opt = 1; 
    unsigned int myPort;
    //pthread_t timer_thread;
    //int addrlen = sizeof(address); 
    char myIP[16];
	
	slist_data_t *datap=NULL;

    SLIST_HEAD(slisthead, slist_data_s) head;
    SLIST_INIT(&head); 
#ifdef USE_AESD_CHAR_DEVICE
#else
    if (pthread_mutex_init(&mutex, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
#endif

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
    //printf("strcmp value %d\n", strcmp(argv[1], "-d"));
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
    if (listen(server_fd, 3) < 0) 
    { 
        syslog(LOG_INFO,"listen"); 
        exit(EXIT_FAILURE); 
    } 
    syslog(LOG_INFO,"Listening \n");
#ifdef USE_AESD_CHAR_DEVICE
#else
    struct sigevent sev;
    struct itimerspec trigger;
    
    memset(&sev, 0, sizeof(struct sigevent));
    memset(&trigger, 0, sizeof(struct itimerspec));

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_notify_function = &timer_handler;
    
    timer_create(CLOCK_REALTIME, &sev, &timerid);
    trigger.it_value.tv_sec = 10;
    trigger.it_interval.tv_sec = 10;
    timer_settime(timerid, 0, &trigger, NULL);
#endif
	//timer_create(CLOCK_REALTIME, &sev, &timerid);
    /*int timer_th = pthread_create(&timer_thread, NULL, timer_handler, NULL);
	if(timer_th!=0){
		syslog(LOG_ERR, "creating timer thread failed");
	}*/


    while(!system_exit_flag){
    	pthread_t thread_id = 0;

    	syslog(LOG_INFO,"inside While \n");
    	clntLen = sizeof(cli_address);

    	
		//

	    new_socket = accept(server_fd, (struct sockaddr *)&cli_address,  &clntLen);
	    if(system_exit_flag){
			break;
		}
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

		datap = (slist_data_t*)malloc(sizeof(slist_data_t));
		datap->thread_id = thread_id++;
		datap->thread_flag = 0;

		SLIST_INSERT_HEAD(&head, datap, entries);

		int rc = pthread_create(&datap->thread_id, NULL, thread_handler, (void*)datap);

		if(rc!=0){
			printf("Error with thread creation");
			exit(1);
		}

		slist_data_t *temp2 = NULL;
		SLIST_FOREACH_SAFE(datap, &head, entries, temp2){
			if(datap->thread_flag){
					pthread_join(datap->thread_id, NULL);
				}
			}
		syslog(LOG_INFO,"CLOSED ip address: %s\n", myIP);
		
		
	}
#ifdef USE_AESD_CHAR_DEVICE
    if (remove("/dev/aesdchar") ==0){
                syslog(LOG_INFO, "Deleted /dev/aesdchar");
                printf("removed\n");
            }
            else{
                syslog(LOG_INFO, "Unable to delete the file");
                printf("couldnt remove\n");
            }

#else
	if (remove("/var/tmp/aesdsocketdata.txt") ==0){
				syslog(LOG_INFO, "Deleted /var/tmp/aesdsocketdata");
				printf("removed\n");
			}
			else{
				syslog(LOG_INFO, "Unable to delete the file");
				printf("couldnt remove\n");
			}
 printf("Outside main while\n");
    printf("After pthread join\n");
    pthread_mutex_destroy(&mutex);
#endif
    slist_data_t *temp2 = NULL;
    SLIST_FOREACH_SAFE(datap, &head, entries, temp2){
        if(datap->thread_flag){
            pthread_join(datap->thread_id, NULL);
        }
    }
    close(server_fd);
    closelog();
    exit(0);
    //return 0; 
}
