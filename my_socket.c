#include "my_socket.h"

int sendAll(int sockFD, char* buf, int * len) {
	int total = 0;        
    int bytesLeft = *len; 
    int n;

    while(total < *len) {
        n = send(sockFD, buf+total, bytesLeft, 0);
        if (n == -1){
        	break; 
        }
        total += n;
        bytesLeft -= n;
    }

    *len = total;

    return n==-1?-1:0;
}

int receive(int sockFD, char* buff, int maxLength, int flags) {
    fd_set readSet;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 100;
    FD_ZERO(&readSet);
    FD_SET(sockFD,&readSet);
    select(sockFD + 1, &readSet, NULL, NULL, &tv);
    if (FD_ISSET(sockFD, &readSet)) {
        return recv(sockFD, buff, maxLength, flags);
    }
    return -2;
}