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


int main(void){
	int file;
	char *filename = "/dev/i2c-1";
	if ((file = open(filename, O_RDWR)) < 0) {
        /* ERROR HANDLING: you can check errno to see what went wrong */
    perror("Failed to open the i2c bus");
    exit(1);
    }
     
	int addr = 0x48;     // The I2C address of the device
    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }

    unsigned char buf[10] = {0};
     
    // for (int i = 0; i<4; i++) {
    //     // Using I2C Read
    //     if (read(file,buf,2) != 2) {
    //         /* ERROR HANDLING: i2c transaction failed */
    //         printf("Failed to read from the i2c bus: %s.\n", strerror(errno));
    //             printf("\n\n");
    //     } else {
    //         /* Device specific stuff here */
    //     }
    // }
}