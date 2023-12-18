#include <unistd.h>
#include <stdio.h>
int main(){
char buffer[1024]="HELLO WORLD";
printf("%s\n",buffer);
printf("CODE INJECT");
}