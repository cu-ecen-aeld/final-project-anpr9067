RM=rm -f

ifeq ($(CC),)
	CC = $(CROSS_COMPILE)gcc
endif
ifeq ($(CFLAGS),)
	CFLAGS = -g -Wall -Werror
endif
ifeq ($(LDFLAGS),)
	LDFLAGS = -pthread -lrt
endif

all:server client i2c test

server: Driver/server.c
	$(CC) $(CFLAGS) Driver/server.c -o server $(LDFLAGS)

client: Driver/client.c
	$(CC) $(CFLAGS) Driver/client.c -o client $(LDFLAGS)

i2c: Driver/i2c_driver.c
	$(CC) $(CFLAGS) Driver/i2c_driver.c -o i2cdriver $(LDFLAGS)

test: Driver/test.c
	$(CC) $(CFLAGS) Driver/test.c -o test $(LDFLAGS)

clean:
	$(RM) server
	$(RM) client
	$(RM) i2cdriver
	$(RM) test
