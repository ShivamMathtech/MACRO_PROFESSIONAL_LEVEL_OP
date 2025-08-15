#include<stdio.h>
#define CREATE_VAR(name,val) name##val
int main(int argc,char*vargs[]){

int CREATE_VAR(value,1) =100;
printf("The value of Value1 is %d",value1);
return 0;
}