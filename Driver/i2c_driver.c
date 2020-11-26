// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// LIS331HH
// This code is designed to work with the LIS331HH_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Accelorometer?sku=LIS331HH_I2CS#tabs-0-product_tabset-2

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>


pthread_mutex_t mutex;

void sig_handler(int signo){
	if(signo == SIGINT){
		syslog(LOG_INFO, "Caught signal, exiting");
		if (remove("/var/tmp/accdata.txt") ==0){
			syslog(LOG_INFO, "Deleted /var/tmp/accdata");
		}
		else{
			syslog(LOG_INFO, "Unable to delete the file");
		}
		exit(1);
	}
	if(signo == SIGTERM){
		syslog(LOG_INFO, "Caught signal, exiting");
		if (remove("/var/tmp/accdata.txt") ==0){
			syslog(LOG_INFO, "Deleted /var/tmp/accdata");
		}
		else{
			syslog(LOG_INFO, "Unable to delete the file");
		}
		exit(1);
	}
}

int append_file(char *buffer, size_t size){
	int recvMsgSize;
	int fd = open("/var/tmp/accdata.txt", O_RDWR|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
    if(fd<0){
    	syslog(LOG_INFO,"Error in opening file");
    	return -1;
    }
    pthread_mutex_lock(&mutex);

    recvMsgSize = write(fd, buffer, size);
    if(recvMsgSize<0){
    	syslog(LOG_INFO, "error in write");
    }

    pthread_mutex_unlock(&mutex);
    close(fd);
    return 0;
}


int main(int argc, char const *argv []) 
{
	struct sigaction sa;

	sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; /* Restart functions*/

    if (sigaction(SIGINT, &sa, NULL) == -1){
    	syslog(LOG_ERR, "Error in siginit");
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1){
    	syslog(LOG_ERR, "Error in siginit");
    }
    syslog(LOG_INFO,"Signal check init \n"); 

    if (pthread_mutex_init(&mutex, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 

	// Create I2C bus
	int file;
	char *bus = "/dev/i2c-2";
	if((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, LIS331HH I2C address is 0x18(24)
	ioctl(file, I2C_SLAVE, 0x18);

	// Select control register1(0x20)
	// X, Y and Z axis enabled, power on mode, data rate o/p 50 Hz(0x27)
	char config[2] = {0};
	config[0] = 0x20;
	config[1] = 0x27;
	write(file, config, 2);

	// Select control register4(0x23)
	// Full scale +/- 6g, continuous update(0x00)
	config[0] = 0x23;
	config[1] = 0x00;
	write(file, config, 2);
	sleep(1);
	
	// Read 6 bytes of data
	// lsb first
	// Read xAccl lsb data from register(0x28)
	while(1){
		char *val = (char*)malloc(sizeof(char));
		char reg[1] = {0x28};
		write(file, reg, 1);
		char data[1] = {0};
		if(read(file, data, 1) != 1)
		{
			printf("Erorr : Input/output Erorr \n");
			exit(1);
		}
		char data_0 = data[0];

		// Read xAccl msb data from register(0x29)
		reg[0] = 0x29;
		write(file, reg, 1);
		read(file, data, 1);
		char data_1 = data[0];

		// Read yAccl lsb data from register(0x2A)
		reg[0] = 0x2A;
		write(file, reg, 1);
		read(file, data, 1);
		char data_2 = data[0];

		// Read yAccl msb data from register(0x2B)
		reg[0] = 0x2B;
		write(file, reg, 1);
		read(file, data, 1);
		char data_3 = data[0];

		// Read zAccl lsb data from register(0x2C)
		reg[0] = 0x2C;
		write(file, reg, 1);
		read(file, data, 1);
		char data_4 = data[0];

		// Read zAccl msb data from register(0x2D)
		reg[0] = 0x2D;
		write(file, reg, 1);
		read(file, data, 1);
		char data_5 = data[0];
		
		// Convert the data
		int xAccl = (data_1 * 256 + data_0);
		printf("x: %x  %x\n", data_1, data_0);
		if(xAccl > 32767)
		{
			xAccl -= 65536;
		}

		int yAccl = (data_3 * 256 + data_2);
		printf("y: %x  %x\n", data_3, data_2);
		if(yAccl > 32767)
		{
			yAccl -= 65536;
		}

		int zAccl = (data_5 * 256 + data_4);
		printf("z: %x  %x\n", data_5, data_4);
		if(zAccl > 32767)
		{
			zAccl -= 65536;
		}

		// Output data to screen
		printf("Acceleration in X-Axis : %d \n", xAccl);
		printf("Acceleration in Y-Axis : %d \n", yAccl);
		printf("Acceleration in Z-Axis : %d \n", zAccl);

		sprintf(val, "X:%d Y:%d Z:%d\n", xAccl, yAccl, zAccl);

		append_file(val, strlen(val));

		free(val);
	}
	
}