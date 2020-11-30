#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(){
	int recvMsgSize;
	char *buffer = "X:999123, Y:4563, Z:-111\n";;
	while(1){
		int fd;
		fd = open("/var/tmp/accdata.txt", O_RDWR|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
        if(fd<0){
            printf("Error in opening file");
        }
        //total_bytes += byte_size;
        recvMsgSize = write(fd, buffer, strlen(buffer));
        if(recvMsgSize<0){
            printf("Error in write\n");
        }
        printf("write done: %d\n", recvMsgSize);   
        close(fd);
	}
	return 0;
}