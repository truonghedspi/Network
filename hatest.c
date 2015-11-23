
#include <string.h>
 #include <stdio.h>





 /* Let's test it out */
 int main(void) {
 char pass[15];

 printf("Nhap pass: ");
 get_pass(pass);
 printf("\nPass la: %s: ", pass);
 printf("\nCo do dai %d: ",strlen(pass));
 return 0;
 } 