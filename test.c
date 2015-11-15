#include <stdio.h>
#include "respond.h"
#include "request.h"
#define LEN 204 

char* check_whitespace (char *str)
{
  char key[] = { ' ', '\n', '\t', 0 };
  return strpbrk (str, key);
}

int main() {
	char buff[10][19];


	printf("%d\n", sizeof(buff));
}