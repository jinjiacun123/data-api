#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct {
  int code_type;
  int preclose;
  char code[6];
}my_t;
#define BUFF_LEN 14

int main()
{
	my_t *my;		
	char buff[BUFF_LEN];
	FILE *fp = fopen("my_data", "r");
	if(fp == 0){
	printf("can't open file\n");	
	return -1;
	}			
	memset(&buff, 0x00, BUFF_LEN);
	char * c = fread(buff,sizeof(unsigned char), BUFF_LEN, fp);
	if(c !=0){
            my = (my_t*)buff;
	    printf("code:%s\n", my->code);
	} 

	return 0;
}
