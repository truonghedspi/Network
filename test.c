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
	FILE *f;
	char data[100];
	char buff[104];
	A user;
	B user1, user2;

	user1.type = LOGIN;
	strcpy(user1.userName, "Ha");
	strcpy(user1.password, "123");
	memcpy(buff, &user1, 104);
	puts(buff);

	user = *((A*)buff);
	user2 = *((B*)buff);
	if (user.type == LOGIN) {
		printf("Login\n");
	}

	printf("%s-%s\n", user2.userName, user2.password);
	
	return 0;
}