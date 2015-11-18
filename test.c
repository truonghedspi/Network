#include <stdio.h>
#include "respond.h"
#include "request.h"
#define LEN 204 

typedef struct {
	int offline;
	char name[100];
}User;

User user;	

int set() {
	strcpy(user.name, "Truong");
	printf("%s\n",user.name );
	return 1;
}

void setData(User *user) {
	user->offline = 10;
	strcpy(user->name, "Truong");
}

int main() {
	

	printf("1%s\n", user.name);
	strcpy(user.name,"aaa");
	set();
	printf("2%s\n", user.name);
}