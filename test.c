#include <stdio.h>

typedef enum {
	LOGIN,
	LOGOUT
}Request;


typedef struct A {
	Request type;
	char data[100];
}A;

typedef struct B {
	int type;
	char userName[50];
	char password[50];
}B;


int main() {
	char listOnlineUser[10][20];
	
	
	return 0;
}