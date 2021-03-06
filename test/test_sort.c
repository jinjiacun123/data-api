#include<stdio.h>
#include<pthread.h>
#include<signal.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<stdbool.h>
#include<assert.h>
//#define SERVER_HOST "127.0.0.1"
//#define SERVER_HOST "192.168.1.131"
#define SERVER_HOST "122.144.139.237"
#define SERVER_PORT 8001
//#define SERVER_PORT 8800
#define HEADER   "ZJHR"
#define HEADER_EX "SERV"
#define TYPE_HEART      0x0905 //heart tick
#pragma pack (4)
int option = 0;
typedef struct{
  bool is_create;
  pid_t pid;
  int app_fifo_fd;
  int option;
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

typedef struct{
  char head[4];
  int  length;
  unsigned short m_nType;
  char   m_nIndex;
  char   m_cOperator;
}TestSrvData2;
typedef struct entity_s entity_t;

struct entity_s{
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
#define TYPE_REALTIME 0X0201
#define system_32
typedef struct{
  char m_head[4];
  int  m_length;
  unsigned short m_nType;
  char  m_nIndex;
  char  m_Not;
  #ifdef system_32
  long  m_lKey;
  #else
  int  m_lKey;
  #endif
  short m_cCodeType;
  char  m_cCode[6];
  short m_nSize;
  unsigned short m_nOption;
  unsigned short code_type;
  char code[6];
}RealPack;
typedef struct{
  unsigned short code_type;
  char code[6];
}CodeInfo;

static int request_sort(int socket_fd);
static void * init_receive(void * param);
void stop(int signo);
static int send_heart(int socket_fd);
int client;
char buff[1024*1024];
int send_realtime(int socket_fd);
static int send_test(int socket_fd);

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
  /*
  ret = request_sort(client);
  assert(ret == 0);
  */
  /*
  sleep(3);
  option = 1;
  ret = request_sort(client);
  assert(ret == 0);
  */

  
  sleep(3);
  ret = send_realtime(client);
  assert(ret == 0);
 
  /*
  ret = send_test(client);
  assert(ret == 0);
  */
   
  /*
  if((ret = read(client, buff, 1024*1024)) == 0){
    sleep(2);
  }
  printf("ret:%d\n", ret);
  assert(ret >0);
  printf("buff:%s\n", buff);
  */

  printf("connect success...\n");
  //pthread_join(t_id, &thread_result);
  while(true){
    sleep(8);
    send_heart(client);
    /*
    sleep(1);
    ret = request_sort(client);
    assert(ret == 0);
    */
  }
  pthread_join(t_id, NULL);
  return 0;
}

int send_realtime(int socket_fd)
{
  int size = 1;
  char * request;
  int off = sizeof(RealPack);
  int codeinfo_length = sizeof(CodeInfo);
  CodeInfo * codeinfo;
  entity_t * entity;
  int entity_length = sizeof(entity_t);

  request = (char *)malloc(sizeof(RealPack) + codeinfo_length*size);
  if(request == NULL){
    printf("malloc err!\n");
    exit(-1);
  }
  int request_length = sizeof(RealPack)+ codeinfo_length * size;

  if(request == NULL){
    printf("malloc err!\n");
    exit(-1);
  }
  memset(request, 0x00, sizeof(RealPack)+ codeinfo_length*size);

  RealPack * data = (RealPack*)request;
  memcpy(data->m_head, HEADER, 4);
  data->m_length =  request_length - 8;
  data->m_nType = TYPE_REALTIME;
  data->m_nSize = size;
  data->m_nOption= 0x0080;

  //股票:pass
  int i = 0;
  codeinfo = (CodeInfo *)(request+off+i*codeinfo_length);
  codeinfo->code_type = 0x1101;
  strncpy(codeinfo->code, "600000", 6);

  if(send(socket_fd, request, request_length, 0)){
    printf("send success!\n");
    return 0;
  }

  return -1;
}

static int request_sort(int socket_fd)
{
  request_sort_t my_request_sort;
  int ret = -1;

  memcpy(my_request_sort.header_name, HEADER_EX, 4);
  my_request_sort.body_len = sizeof(request_sort_t)-8;
  my_request_sort.app_request.column = 0;
  my_request_sort.app_request.option = option;
  my_request_sort.app_request.index = 0;
  my_request_sort.app_request.begin = 500+option;
  my_request_sort.app_request.size = 10;

  ret = write(socket_fd, &my_request_sort, sizeof(request_sort_t));
  assert(ret >0);
  sleep(5);
  my_request_sort.app_request.begin = 600;
  my_request_sort.app_request.option = 1;
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
      printf("close continue\n");
      break;
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
      printf("option times:%d\n", option);
      printf("type:%x\n", *(int*)body_buff);
      //printf("body_buff:%s\n", body_buff);
      entity = (entity_t*)(body_buff+4);	
      for(i = 0; i<10; i++){
	printf("code:%.6s,price:%d\n", entity->code, entity->price);
	entity++;
      }
      printf("----------------------------------\n");
      //ret = request_sort(client);
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

static int send_test(int socket_fd)
{
  int ret = -1;
  char request[] = {
    0x5a,0x4a,0x48,0x52,
    0x14,0x00,0x00,0x00,
    0x01,0x02,0x30,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x80,0x00
  };
  /*
{
		0x5a,0x4a,0x48,0x52,
		0x44,0x00,0x00,0x00,
		0x01,0x02,0x31,0x00,
		0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,
		0x06,0x00,0x80,0x00,
		0x00,0x5b,0x58,0x41,
		0x55,0x00,0x00,0x00,
		0x00,0x5b,0x58,0x41,
		0x47,0x00,0x2c,0x32,
		0x00,0x5b,0x58,0x41,
		0x50,0x00,0x2c,0x31,
		0x00,0x5b,0x58,0x50,
		0x44,0x00,0x2c,0x32,
		0x00,0x5b,0x54,0x57,
		0x47,0x44,0x00,0x32,
		0x00,0x5b,0x48,0x4b,
		0x47,0x54,0x00,0x2c
	};
    */
		   //stock
                   /*
		   {0x5a,0x4a,0x48,0x52, 0x6c,0x01,0x00,0x00,
		    0x01,0x02,0x32,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
		    0x00,0x00,0x00,0x00, 0x2b,0x00,0x80,0x00, 0x01,0x12,0x30,0x30,
		    0x30,0x30,0x30,0x31, 0x01,0x12,0x30,0x30, 0x30,0x30,0x30,0x32,
		    0x01,0x12,0x30,0x30, 0x30,0x30,0x30,0x34, 0x01,0x12,0x30,0x30,
		    0x30,0x30,0x30,0x35, 0x01,0x12,0x30,0x30, 0x30,0x30,0x30,0x36,
		    0x01,0x12,0x30,0x30, 0x30,0x30,0x30,0x37, 0x01,0x12,0x30,0x30,
		    0x30,0x30,0x30,0x38, 0x01,0x12,0x30,0x30, 0x30,0x30,0x30,0x39,
		    0x01,0x12,0x30,0x30, 0x30,0x30,0x31,0x30, 0x01,0x12,0x30,0x30,
		    0x30,0x30,0x31,0x31, 0x01,0x12,0x30,0x30, 0x30,0x30,0x31,0x32,
		    0x01,0x12,0x30,0x30, 0x30,0x30,0x31,0x34, 0x01,0x12,0x30,0x30,
		    0x30,0x30,0x31,0x36, 0x01,0x12,0x30,0x30, 0x30,0x30,0x31,0x37,
		    0x01,0x12,0x30,0x30, 0x30,0x30,0x31,0x38, 0x01,0x12,0x30,0x30,
		    0x30,0x30,0x31,0x39, 0x01,0x12,0x30,0x30, 0x30,0x30,0x32,0x30,
		    0x01,0x12,0x30,0x30, 0x30,0x30,0x32,0x31, 0x01,0x12,0x30,0x30,
		    0x30,0x30,0x32,0x32, 0x01,0x12,0x30,0x30, 0x30,0x30,0x32,0x33,
		    0x01,0x11,0x36,0x30, 0x30,0x30,0x30,0x30, 0x01,0x11,0x36,0x30,
		    0x30,0x30,0x30,0x34, 0x01,0x11,0x36,0x30, 0x30,0x30,0x30,0x35,
		    0x01,0x11,0x36,0x30, 0x30,0x30,0x30,0x36, 0x01,0x11,0x36,0x30,
		    0x30,0x30,0x30,0x37, 0x01,0x11,0x36,0x30, 0x30,0x30,0x30,0x38,
		    0x01,0x11,0x36,0x30, 0x30,0x30,0x30,0x39, 0x01,0x11,0x36,0x30,
		    0x30,0x30,0x31,0x30, 0x01,0x11,0x36,0x30, 0x30,0x30,0x31,0x31,
		    0x01,0x11,0x36,0x30, 0x30,0x30,0x31,0x32, 0x01,0x11,0x36,0x30,
		    0x30,0x30,0x31,0x35, 0x01,0x11,0x36,0x30, 0x30,0x30,0x31,0x36,
		    0x01,0x11,0x36,0x30, 0x30,0x30,0x31,0x37, 0x01,0x11,0x36,0x30,
		    0x30,0x30,0x31,0x38, 0x01,0x11,0x36,0x30, 0x30,0x30,0x31,0x39,
		    0x01,0x11,0x36,0x30, 0x30,0x30,0x32,0x30, 0x01,0x11,0x36,0x30,
		    0x30,0x30,0x32,0x31, 0x01,0x11,0x36,0x30, 0x30,0x30,0x32,0x32,
		    0x01,0x11,0x36,0x30, 0x30,0x30,0x32,0x33, 0x01,0x11,0x36,0x30,
		    0x30,0x30,0x32,0x36, 0x00,0x12,0x33,0x39, 0x39,0x30,0x30,0x36,
		    0x00,0x11,0x31,0x41, 0x30,0x30,0x30,0x31, 0x00,0x12,0x32,0x41,
		    0x30,0x31,0x00,0x31};
  */
  ret = send(socket_fd, request, sizeof(request), 0);
  assert(ret >0);
  if(ret <=0){
	return -1;
  }
	return 0;
}
