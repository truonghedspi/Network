#include <stdio.h>
#include "respond.h"
#include "request.h"
#define LEN 204 

typedef struct User {
	int offline;
	char name[100];
}User;

void setData(User *user) {
	user->offline = 10;
	strcpy(user->name, "Truong");
}

int main() {
	User user;

	setData(&user);
	printf("%s\n", user.name);
}