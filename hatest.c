#include <stdio.h>
#include "respond.h"
#include "request.h"
#define LEN 204 


void main(){
	int so;
	char buff[LEN];

	printf("\nNhap 1 cai j do: ");
	scanf("%d",&so);
	fgets(buff,LEN,stdin);
	printf("%d-%s\n",so,buff );
	so--;
	printf("%d\n",so );
	printf("\ndo dai: %d",strlen(buff));
}