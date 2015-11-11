#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LENGTH 1000
#define TRUE 1
#define FALSE 0

int getInt();
float getFloat();
void getString(char *);
float getFloatBetween(float, float);
void upperString(char* );
char getChar();
void getStringWithMess(char* mes, char* str);
int getIntWithMess(char* mes);
char getCharWithMess(char* mes);
float getFloatWithMess(char* mes);
void myERROR(char * mes);

#endif