#include<stdio.h>
#include<sys/stat.h>
#include<limits.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include<errno.h>
#include "config.h"
#include "market.h"
#include "./../comm_pipe.h"

#define MAX_BUFF_SIZE 4*1024

int main(int argc, char * argv[])
{
  int begin = 0;
  int size = 0;
  pid_t pid = getpid();
  begin = atoi(argv[1]);
  size = atoi(argv[2]);
  //begin = 0;
  //size = 1;
  char * buff = NULL;
  int entity_len = sizeof(entity_t);
  int buff_len = entity_len *size;
  char cur_app_pipe[100];
  const char *fifo_name = PUBLIC_PIPE;
  char *template = PRIVATE_PIPE_TEMPLATE;
  int pipe_read_fd = -1;
  int pipe_write_fd = -1;
  int app_request_len = sizeof(app_request_t);
  int res = 0;
  entity_t * entity = NULL;
  int i = 0;

  buff = (entity_t *)malloc(buff_len+1);
  if(buff == NULL){
    printf("malloc error!\n");
    exit(-1);
  }
  memset(buff, 0x00, buff_len);
  memset(&cur_app_pipe, 0x00, 100);
  snprintf(cur_app_pipe, 100, template, getpid());
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

  printf("begin:%d,size:%d\n", begin, size);
  app_request_t app_request;
  memset(&app_request, 0x00, sizeof(app_request_t));
  //request
  app_request.pid = pid;
  app_request.begin = begin;
  app_request.size = size;
  app_request.column = 0;
  res = write(pipe_write_fd, &app_request, app_request_len);

  pipe_read_fd = open(cur_app_pipe, O_RDONLY);
  close(pipe_write_fd);
  if(pipe_read_fd == -1){
    printf("open pipe read fd error!\n");
    exit(-1);
  }

  //read sort
  while(true){
    memset(buff, 0x00, buff_len+1);
    res = read(pipe_read_fd, buff, buff_len);
    if(res < 0){
      if(errno == EAGAIN){
	sleep(1);
	continue;
      }
      printf("read error!\n");
      exit(-1);
    }else if(res == 0){
      printf("read complete...\n");
      sleep(3);
      break;
    }

    //printf("price:%s\n", buff);
    //display
    entity = (entity_t *)buff;
    for(i = 0; i<size; i++){
      printf("code_type:%x\tcode:%.6s\tprice:%d\traise:%d\n",
	     entity->type,
	     entity->code,
	     entity->price,
	     entity->raise);
      entity ++;
    }
    printf("------------------------------------\n");
    sleep(1);
  }
  close(pipe_read_fd);

  return 0;
}
