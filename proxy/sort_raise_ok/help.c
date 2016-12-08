#include<stdio.h>
#include "config.h"

int jim_malloc(int size, void ** out){
  int page_size = 0;
  unsigned int length = 0;
  page_size = ceil(size / 4);
  length = page_size * 4;
  //  if(length == size) length = (page_size+1)*4;
  *out = (void *)malloc(size);
  if(*out == NULL){
    printf("malloc error!\n");
    return -1;
  }
  memset(*out, 0x00, size);
  return 0;
}
