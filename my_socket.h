#ifndef MY_SOCKET_H
#define MY_SOCKET_H


#include "input.h"
#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <poll.h>

typedef enum {
	ONLINE,
	OFFLINE
}UserStatus;

typedef struct _User {
	char userName[20];
	char password[20];
	UserStatus isOnline;
	int sockFD;
}User;

int sendAll(int sockFD, char* buf, int * len);
int receive(int sockFD, char* buff, int maxLength, int flags);

#endif