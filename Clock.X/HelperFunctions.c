#include <stdio.h>
#include <string.h>

void NumberToString(unsigned char num, char* buff){
   
    buff[0] = (num/10) + 48;
    buff[1] = (num%10) + 48;
    buff[2] = '\0';
}
