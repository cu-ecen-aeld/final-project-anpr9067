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

all:server

server: Driver/server.c
	$(CC) $(CFLAGS) Driver/server.c -o server $(LDFLAGS)

clean:
	$(RM) aesdsocket
