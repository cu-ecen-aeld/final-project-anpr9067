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
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(void){
	int file;
	char *filename = "/dev/i2c-2";
	if ((file = open(filename, O_RDWR)) < 0) {
        /* ERROR HANDLING: you can check errno to see what went wrong */
    perror("Failed to open the i2c bus");
    printf("Failed to open the i2c bus.\n");
    exit(1);
    }
     
	int addr = 0x18;     // The I2C slave address of the device 
    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }
    printf("Passed\n");
    /*__u8 reg = 0x28; 
  	__s32 res;
  	//char buf[10]; 
  	res = i2c_smbus_read_word_data(file, reg);
  	if (res < 0) {
    	
    	printf("failed");
  	} else {
    	
    		printf("%d word\n", res);
  	}*/
    unsigned char buf[10] = {0};
    unsigned char w[1] = {0x31};
    //unsigned char X_MSB, X_LSB, Y_MSB, Y_LSB, Z_MSB, Z_LSB;
    int writeval = write(file, &w, 1);
    if(writeval == -1){
    	printf("Error in write\n");
    }
    printf("writeval %d\n", writeval);
    for (int i = 0; i<6; i++) {
        // Using I2C Read
        int readval = read(file,buf,6);
        printf("readval : %d\n", readval);
        if (readval != 2) {
            /* ERROR HANDLING: i2c transaction failed */
            printf("Failed to read from the i2c bus: %s.\n", strerror(errno));
                printf("\n");
        } else {
        	printf("to do calculation\n");
        	printf("%x %x\n", buf[0], buf[1]);
        	printf("%x %x\n", buf[2], buf[3]);
        	printf("%x %x\n", buf[4], buf[5]);
            /* Device specific stuff here */
        }
    }
}