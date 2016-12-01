#ifndef __COMM_PIPE_H__
#define __COMM_PIPE_H__
#define HEADER_EX "SERV"
//public pipe
#define PUBLIC_PIPE "/home/jim/source_code/data-api/proxy/jim_sort"
#define PRIVATE_PIPE_TEMPLATE "/home/jim/source_code/data-api/proxy/child_pipe/sort_%d"

//app request struct
typedef struct
{
  bool is_create;
  pid_t pid;
  int app_fifo_fd;
  int option;
  int index;
  int column;
  int begin;
  int size;
}app_request_t;

typedef enum{
  SORT_REGISTER,
  SORT_GET,
  SORT_CLOSE
}sort_command_n;
#endif
