#include "input.h"

int getInt() {
	int num;

	while (scanf("%d", &num) != 1) {
		while(getchar() != '\n');
	}
	while(getchar() != '\n');

	return num;
}

char getChar() {
	char c;

	while (scanf("%c", &c) != 1) {
		while(getchar() != '\n');
	}
	while(getchar() != '\n');

	return c;
}

float getFloat() {
	float num;

	while(scanf("%f", &num) != 1) {
		while(getchar() != '\n');
	}
	while(getchar() != '\n');

	return num;
}

float getFloatBetween(float low, float up) {
	float num;
	
	while (1) {
		num = getFloat();
		if (num < low) {
			printf("Number must >= %f\nPlease try again!\n", low);
			continue;
		}

		if (num >  up) {
			printf("Number must <= %f\nPlease try again!\n", up);
			continue;
		}
		return num;
	}
}

void getString(char * str) {
	while (1) {
		fgets(str,MAX_LENGTH + 1, stdin);
		str[strlen(str) - 1] = '\0';
		if (str[0] == '\n' || str[0] == '\0') {
			printf("Can't be Empty!\nTry again\n");
			continue;
		}
		return;
	}
}

void upperString(char* str) {
	unsigned i = 0;

	while (i != strlen(str)) {
		str[i] = toupper(str[i]);
		++i;
	}
}

void getStringWithMess(char* mes, char* str) {
	printf("%s", mes);
	getString(str);
}

int getIntWithMess(char* mes) {
	printf("%s", mes);
	return getInt();
}

char getCharWithMess(char* mes) {
	printf("%s", mes);
	return getChar();
}

float getFloatWithMess(char *mes) {
	printf("%s", mes);
	return getFloat();
}

void myERROR(char * mes) {
	printf("%s\n", mes);
	exit(1);
}