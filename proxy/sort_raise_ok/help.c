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

/**
      *   计算两个时间的间隔，得到时间差
      *   @param   struct   timeval*   resule   返回计算出来的时间
      *   @param   struct   timeval*   x             需要计算的前一个时间
      *   @param   struct   timeval*   y             需要计算的后一个时间
      *   return   -1   failure   ,0   success
      **/
int   timeval_subtract(struct   timeval*   result,   struct   timeval*   x,   struct   timeval*   y)
{
  int   nsec;

  if   (   x->tv_sec>y->tv_sec   )
    return   -1;

  if   (   (x->tv_sec==y->tv_sec)   &&   (x->tv_usec>y->tv_usec)   )
    return   -1;

  result->tv_sec   =   (   y->tv_sec-x->tv_sec   );
  result->tv_usec   =   (   y->tv_usec-x->tv_usec   );

  if   (result->tv_usec<0)
    {
      result->tv_sec--;
      result->tv_usec+=1000000;
    }

  return   0;
}
