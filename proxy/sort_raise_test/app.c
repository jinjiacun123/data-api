#include<stdio.h>
#include<sys/stat.h>
#include<limits.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include "config.h"
#include "market.h"
#include "./../comm_pipe.h"

#define MAX_BUFF_SIZE 4*1024

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
  int buff_len = entity_len *my_size + sizeof(int);
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

  printf("begin:%d,size:%d\n", my_begin, my_size);
  app_request_t app_request;
  memset(&app_request, 0x00, sizeof(app_request_t));
  //request
  app_request.pid = pid;
  app_request.begin = my_begin;
  app_request.size = my_size;
  app_request.column = column;
  res = write(pipe_write_fd, &app_request, app_request_len);
  /*
  app_request.option = 1;
  app_request.begin = my_begin+10;
  res = write(pipe_write_fd, &app_request, app_request_len);
  */
  close(pipe_write_fd);

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
    entity = (entity_t *)(buff + sizeof(int));
    for(i = 0; i<my_size; i++){
      printf("code_type:%x, code:%.6s,pre_close:%d,price:%d,raise:%d\n",
	     entity->type,
	     entity->code,
	     entity->pre_close,
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
