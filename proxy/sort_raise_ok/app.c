#include<stdio.h>
#include<sys/stat.h>
#include<limits.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include<sys/time.h>
#include "config.h"
#include "market.h"
#include "./../comm_pipe.h"

#define MAX_BUFF_SIZE 4*1024
int times = 0;
int max_time = 0;
struct timeval start_time;
struct timeval end_time;
struct timeval diff_time;
int   timeval_subtract(struct   timeval*   result,   struct   timeval*   x,   struct   timeval*   y);
int main(int argc, char * argv[])
{
  int my_begin = 0;
  int my_size = 0;
  int column = 0;
  pid_t pid = getpid();
  my_begin = atoi(argv[1]);
  my_size = atoi(argv[2]);
  column  = atoi(argv[3]);
  //my_begin = 0;
  //my_size = 10;
  char * buff = NULL;
  int entity_len = sizeof(entity_t);
  int buff_len = entity_len *my_size + 2*sizeof(int);
  char cur_app_pipe[100];
  const char *fifo_name = PUBLIC_PIPE;
  char *template = PRIVATE_PIPE_TEMPLATE;
  int pipe_read_fd = -1;
  int pipe_write_fd = -1;
  int app_request_len = sizeof(app_request_t);
  int res = 0;
  entity_t * entity = NULL;
  int i = 0;
  int begin = 0;

  buff = (entity_t *)malloc(buff_len+1);
  if(buff == NULL){
    printf("malloc error!\n");
    exit(-1);
  }
  memset(buff, 0x00, buff_len);
  memset(&cur_app_pipe, 0x00, 100);
  snprintf(cur_app_pipe, 100, template, getpid());
  unlink(cur_app_pipe);
  //create cur app pipe
  if(access(cur_app_pipe, F_OK) == -1){
    res = mkfifo(cur_app_pipe, 0777);
    if(res != 0){
      printf(stderr, "count not create fifo %s\n", cur_app_pipe);
      exit(-1);
    }
  }

  if(access(fifo_name, F_OK) == -1){
    printf("fifo is not exists!\n");
    exit(-1);
  }
  pipe_write_fd = open(fifo_name, O_WRONLY);
  if(pipe_write_fd == -1){
    printf("open pipe err!\n");
    exit(-1);
  }

  request_sort(pipe_write_fd, my_begin, my_size, column, pid, app_request_len);
  gettimeofday(&start_time, 0);
  /*
  app_request.option = 1;
  app_request.begin = my_begin+1;
  res = write(pipe_write_fd, &app_request, app_request_len);
  */

  pipe_read_fd = open(cur_app_pipe, O_RDONLY);
  if(pipe_read_fd == -1){
    printf("open pipe read fd error!\n");
    exit(-1);
  }

  printf("wait read ...\n");
  //read sort
  while(true){
    memset(buff, 0x00, buff_len+1);
    res = read(pipe_read_fd, buff, buff_len);
    printf("read ...\n");
    if(res == -1){
      printf("read error!\n");
      exit(-1);
    }else if(res == 0){
      printf("read complete...\n");
      sleep(3);
      break;
    }
    printf("option:%d\n", *(int*)buff);

    //printf("price:%s\n", buff);
    //display
    entity = (entity_t *)(buff + 2*sizeof(int));
    for(i = 0; i<my_size; i++){
      printf("code_type:%x, code:%.6s,pre_close:%d,price:%d,raise:%d,range:%d\n",
	     entity->type,
	     entity->code,
	     entity->pre_close,
	     entity->price,
	     entity->raise,
	     entity->range);
      entity ++;
    }
    printf("------------------------------------\n");
    printf("times:%d\n", ++times);
    gettimeofday(&end_time, 0);
    timeval_subtract(&diff_time, &start_time, &end_time);
    printf("总计用时:%d微妙\n", diff_time.tv_usec);
    if(max_time < diff_time.tv_usec)
      max_time = diff_time.tv_usec;
    printf("到目前为止最大时间:%d\n", max_time);
    /*
    sleep(1);
    request_sort(pipe_write_fd, my_begin, my_size, column, pid, app_request_len);
    */
  }
  close(pipe_read_fd);
  close(pipe_write_fd);

  return 0;
}

void request_sort(pipe_write_fd, my_begin, my_size, column, pid, app_request_len)
  int pipe_write_fd;
  int my_begin;
  int my_size;
  int column;
  int pid;
  int app_request_len;
{
  gettimeofday(&start_time, 0);
  int res = 0;
  printf("begin:%d,size:%d\n", my_begin, my_size);
  app_request_t app_request;
  memset(&app_request, 0x00, sizeof(app_request_t));
  //request
  app_request.pid = pid;
  app_request.begin = my_begin;
  app_request.size = my_size;
  app_request.column = column;
  app_request.index  = -100;
  app_request.option = 0;
  res = write(pipe_write_fd, &app_request, app_request_len);
  printf("write res:%d\n", res);
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
