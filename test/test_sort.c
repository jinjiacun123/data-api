#include<stdio.h>
#include<pthread.h>
#include<signal.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<stdbool.h>
#include<assert.h>
#define SERVER_HOST "127.0.0.1"
//#define SERVER_HOST "192.168.1.131"
#define SERVER_PORT 8001
#define HEADER   "ZJHR"
#define HEADER_EX "SERV"
#define TYPE_HEART      0x0905 //heart tick
#pragma pack (4)

typedef struct
{
  bool is_create;
  pid_t pid;
  int app_fifo_fd;
  int index;
  int column;
  int begin;
  int size;
}app_request_t;

//custom sort of request
typedef struct{
  char header_name[4];
  int body_len;

  app_request_t app_request;
}request_sort_t;

typedef struct
{
  char head[4];
  int  length;
  unsigned short m_nType;
  char   m_nIndex;
  char   m_cOperator;
}TestSrvData2;
typedef struct entity_s entity_t;

struct entity_s
{
  unsigned short type;
  char code[7];
  int  pre_close;  //close price of yestoday

  int pre_price;  //prefix value,use to check is sort
  int price;       //now price
  int price_area[2]; //first is area, second is queue

  float add;
  float down;
  float range;
  int max;
  int min;
  int buy;
  int sell;
};

static int request_sort(int socket_fd);
static void * init_receive(void * param);
void stop(int signo);
static int send_heart(int socket_fd);
int client;
char buff[1024*1024];

int main()
{
  int ret = -1;
  pthread_t t_id;
  void *thread_result;

  //signal(SIGUSR2, send_request);
  struct sockaddr_in cli;
  signal(SIGINT, stop);

  cli.sin_family = AF_INET;
  cli.sin_port = htons(SERVER_PORT);
  cli.sin_addr.s_addr = inet_addr(SERVER_HOST);

  client = socket(AF_INET, SOCK_STREAM, 0);
  if(client < 0){
    printf("socket() failrue!\n");
    return -1;
  }

  if(connect(client, (struct sockaddr*)&cli, sizeof(cli)) < 0){
    printf("connect() failure!\n");
    return -1;
  }

	
  //init receive
  ret = pthread_create(&t_id, NULL, init_receive, NULL);
  if(ret != 0){
    perror("create thread err!\n");
    exit(-1);
  }

  //send sort request
  ret = request_sort(client);
  assert(ret == 0);
  /*
  ret = read(client, buff, 1024*1024);
  assert(ret >0);
  */
  
  printf("connect success...\n");
  //pthread_join(t_id, &thread_result);
  while(true){
    sleep(3);
    send_heart(client);
  }
  pthread_join(t_id, NULL);

  return 0;
}

static int request_sort(int socket_fd)
{
  request_sort_t my_request_sort;
  int ret = -1;

  memcpy(my_request_sort.header_name, HEADER_EX, 4);
  my_request_sort.body_len = sizeof(request_sort_t)-8;
  my_request_sort.app_request.column = 0;
  my_request_sort.app_request.index = 0;
  my_request_sort.app_request.begin = 100;
  my_request_sort.app_request.size = 10;

  ret = write(socket_fd, &my_request_sort, sizeof(request_sort_t));
  assert(ret >0);
  return 0;
}

static void *init_receive(void * param)
{
  int ret = -1;
  char head_buff[9];
  char * body_buff;
  int length;
  entity_t * entity = NULL;
  int i = 0;

  while(true){
    memset(&head_buff, 0x00, 9);
    //read head
    ret = read(client, head_buff, 8);
    if(ret == 0){
      //printf("close continue\n");
      continue;
    }else if(ret == -1){
      pthread_exit("receive err!\n");
    }else if(ret == 8){
      //read content
      length = *(int *)(head_buff+4);
      body_buff = (char *)malloc(length +1);
      if(body_buff == NULL){
	pthread_exit("malloc error!\n");
      }
      ret = read(client, body_buff, length);
      if(ret <0){
	printf("read body_buff error\n");
	break;
      }else if(ret == 0){
	printf("close...\n");
	break;
      }
      body_buff[length] = '\0';		
      entity = (entity_t*)body_buff;	
      for(i = 0; i<10; i++){
	printf("code:%.6s,price:%d\n", entity->code, entity->price);
	entity++;
      }
      printf("----------------------------------\n");
    }
  }
}

void stop(int signo)
{
  printf("sigint close...\n");
  //shutdown(client, 2);	
  close(client);
  exit(-1);
}

static int send_heart(int socket_fd)
{
  char request[1024];
  TestSrvData2 data ;
  memset(&data,0x00,sizeof(TestSrvData2));
  memcpy(data.head, HEADER,4);
  data .length      = sizeof(TestSrvData2) -8;
  data.m_nType     = TYPE_HEART;
  data.m_nIndex= 1;

  memset(request, 0, 1024);
  memcpy(request, &data, sizeof(data));
  send(socket_fd, request, sizeof(data), 0);
  //printf("send heart message...\n");
  return 0;
}
