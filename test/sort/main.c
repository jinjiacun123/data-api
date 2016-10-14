#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<assert.h>
#include "cJSON.h"

#define SERVER_MARKET_PRE "http://dsapp.yz.zjwtj.com:8010/initinfo/stock/"
//#define SERVER_MARKET "221.6.167.245"
//#define SERVER_MARKET_PORT 8881
#define SERVER_MARKET "192.168.1.131"
#define SERVER_MARKET_PORT 8001

#define USERNAME "jrjvip_android"
#define PASSWORD "zjw_android"
#define HEADER   "ZJHR"

/*number of type as hexadicimal*/
#define TYPE_EMPTY      0x0D03 //empty
#define TYPE_INIT       0x0101 //init
#define TYPE_LOGIN      0X0102 //login
#define TYPE_HEART      0x0905 //heart tick
#define TYPE_ZIB        0x8001 //
#define TYPE_REALTIME   0x0201 //
#define TYPE_HISTORY    0x0402 //
#define TYPE_TIME_SHARE 0x0301 //
#define TYPE_AUTO_PUSH  0x0A01 //
#define TYPE_SERVERINFO 0x0103 //
#define TYPE_DAY_CURPOS 0x020c //

char buff[100*1024];

typedef struct
{
  char m_head[4]; 
  int  m_length;  
  unsigned short m_nType;
  char  m_nIndex;   
  char  m_Not;   
  long  m_lKey;
  short m_cCodeType;
  char  m_cCode[6];
  short m_nSize;
  unsigned short m_nOption; 
}RealPack;

typedef struct
{
  unsigned short code_type;
  char code[6];
}CodeInfo;

typedef struct
{
  char code[6];
  char name[20];
  int  pre_close;  //close price of yestoday
  int price;       //now price
}entity_t;

typedef struct
{
  char file_name[10];
  char date[8];//year-month-day
  short code_type;
  char name[20];
  short unit;
  char open_close_time[50];
  entity_t * list;
  int entity_list_size;
}market_t;

market_t market_list[] = {
  //上证a股
  {"1101.txt","20161012",0x1101,"上证A股",1000,"[570-690][780-900][-1--1][-1--1]"},
  //深证a股
  {"1201.txt","20161012",0x1201,"深证A股",1000,"[570-690][780-900][-1--1][-1--1]"}
};
entity_t * entity_list;
int init_market();
int last_time_market;//effective time
int cur_time;        //current time
int init_socket(int * sock_fd);
void init_receive(void * socket_fd);
int get_content(char * filename, char * buff, int length);
int get_market(cJSON * root_json, int index);
void send_realtime(int socket_fd, int index, int size);
int send_auto_push(int socket_fd);

int main()
{
  pthread_t p_id = 0;
  int socket_fd = 0;
  int ret = 0;
  //--receive both shanghhai and shenzhen market stock
  // init_market();
  init_socket(&socket_fd);
  //init_receive(&socket_fd);
  //ret = pthread_create(&p_id, NULL, (void *)init_receive, (void *)&socket_fd);
 
  int menu = 1;
  while(1){
    /*
    printf("1-realtime\n \
            2-auto_push\n");
    menu = getchar();
    */
    switch(menu){
    case 1:{//realtime
      //---init data and sort
      //get realtime data
      send_realtime(socket_fd, 0, 10);
      char *buff;
      int length = 8;
      buff = (char *)malloc(length);
      if(buff == NULL){
	printf("memory malloc err!\n");
	exit(-1);
      }
      memset(buff, 0x00, length);
      buff = read(socket_fd, buff, length);
      menu = 2;
    }
    case 2:{//autopush

    }
    default:{
      
    }
    }
    
    //sort by price
  
    //---auto push data---
    //get auto push data and resort data
  }
  
  
}

//get code for both market
int init_market()
{
  //get both txt  
  
  char template_str[][100] =  {
    "rm ./txt/%s ./txt/%s",
    "wget -P ./txt/ http://dsapp.yz.zjwtj.com:8010/initinfo/stock/%s",
    "./txt/%s"
  };
  char cmd[100];
  memset(&cmd, 0x00, 100);
  sprintf(cmd, template_str[0], market_list[0].file_name, market_list[1].file_name);
  system(cmd);
  memset(&cmd, 0x00, 100);
  sprintf(cmd, template_str[1], market_list[0].file_name);
  system(cmd);
  //system("wget -P ./txt/ http://dsapp.yz.zjwtj.com:8010/initinfo/stock/1101.txt");
  memset(&cmd, 0x00, 100);
  sprintf(cmd, template_str[1], market_list[1].file_name);
  system(cmd);

  //parse
  int index = 0;
  memset(buff, 0x00, 1024*100);
  memset(&cmd, 0x00, 50);
  sprintf(cmd, template_str[2], market_list[index].file_name);
  int length = get_content(cmd, buff, 1024*100);
  cJSON * root_json = cJSON_Parse(buff);    //将字符串解析成json结构体
  if (NULL == root_json){
      printf("error:%s\n", cJSON_GetErrorPtr());
      cJSON_Delete(root_json);
      return -1;
  }
  //parse 1101
  get_market(root_json, index);
  free(root_json);
  //parse 1201
  index = 1;
  memset(buff, 0x00, 1024*100);
  memset(&cmd, 0x00, 50);
  sprintf(cmd, template_str[2], market_list[index].file_name);
  length = get_content(cmd, buff, 1024*100);
  root_json = cJSON_Parse(buff);    //将字符串解析成json结构体
  if (NULL == root_json){
      printf("error:%s\n", cJSON_GetErrorPtr());
      cJSON_Delete(root_json);
      return;
  }
  get_market(root_json, index);
 
  return 0;
}

int get_content(char * filename, char * buff, int buff_len)
{
  int fd;
  fd = open(filename, O_RDONLY);
  if( fd == -1){
    printf("open err!\n");
    exit(-1);
  }

  int ret = 0;
  int length = 0;
  while(1){
    ret = read( fd, buff, buff_len);
    if(ret == -1){
      printf("read err!\n");
      exit(-1);
    }
    length += ret;
    //   printf("buff:%s\n", buff);
    if(ret == 0){
      break;
    }
  }
  
  close(fd);
  return length;
}

int get_market(cJSON * root_json, int index)
{
  entity_t * entity;
  cJSON * obj;
  int entity_list_len = 0;
  obj = cJSON_GetObjectItem(root_json, "date");
  if(obj == NULL){
    printf("get object of date err!\n");
    exit(-1);
  }
  strncpy(market_list[index].date, obj->valuestring, 6);
  obj = cJSON_GetObjectItem(root_json, "codetype");
  if(obj == NULL){
    printf("get object of date err!\n");
    exit(-1);
  }
  market_list[index].code_type = strtol(obj->valuestring, NULL, 16);
  obj = cJSON_GetObjectItem(root_json, "szname");
  if(obj == NULL){
    printf("get object of date err!\n");
    exit(-1);
  }
  strcpy(market_list[index].name, obj->valuestring);
  obj = cJSON_GetObjectItem(root_json, "priceunit");
  if(obj == NULL){
    printf("get object of date err!\n");
    exit(-1);
  }
  market_list[index].unit = atoi(obj->valuestring);
  obj = cJSON_GetObjectItem(root_json, "open_close_time");
  if(obj == NULL){
    printf("get object of date err!\n");
    exit(-1);
  }
  strcpy(market_list[index].open_close_time, obj->valuestring);
  obj = cJSON_GetObjectItem(root_json, "list");
  if(obj == NULL){
    printf("get object of date err!\n");
    exit(-1);
  }
  market_list[index].entity_list_size = cJSON_GetArraySize(obj);
  if(market_list[index].list != NULL){free(market_list[index].list);}
  market_list[index].list = (entity_t *)malloc(market_list[index].entity_list_size*sizeof(entity_t));
  if(market_list[index].list == NULL){
    printf("molloc menory err!\n");
    exit(-1);
  }
  memset(market_list[index].list, 0x00, market_list[index].entity_list_size);
  int i = 0;
  cJSON * item;
  entity = market_list[index].list;
  for(;i< market_list[index].entity_list_size; i++){
    item = cJSON_GetArrayItem(obj, i);
    strncpy(entity->code, cJSON_GetObjectItem(item, "code")->valuestring,6);
    strcpy(entity->name, cJSON_GetObjectItem(item, "name")->valuestring);
    entity->pre_close = atoi(cJSON_GetObjectItem(item, "preclose")->valuestring);
    printf("code:%s,name:%s,preclose:%d\n", 
	   entity->code,
	   entity->name,
	   entity->pre_close);
    entity++;
  }

  printf("date:%s,code_type:%x\n", 
	 market_list[index].date, 
	 market_list[index].code_type,
	 market_list[index].name,
	 market_list[index].unit,
	 market_list[index].open_close_time,
	 market_list[index].entity_list_size);
  
  return 0;
}

int init_socket(int * socket_fd)
{
  int c_len = 0;
  struct sockaddr_in cli;
  
  cli.sin_family = AF_INET;
  cli.sin_port = htons(SERVER_MARKET_PORT);
  cli.sin_addr.s_addr = inet_addr(SERVER_MARKET);

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_fd < 0){
    printf("socket() failrue!\n");
    return -1;
  }

  if(connect(socket_fd, (struct sockaddr*)&cli, sizeof(cli)) < 0){
    printf("connect() failure!\n");
    return -1;
  } 
  printf("connect() success\n");
  return 0;
}

void send_realtime(int socket_fd, int index, int size)
{
  size = 1;
  char * request;
  int entity_count = market_list[0].entity_list_size;
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
  data->m_nSize = entity_count;
  data->m_nOption= 0x0080;

  codeinfo = (CodeInfo *)(request+off);
  codeinfo->code_type = 0x1101;
  strncpy(codeinfo->code, "600000", 6);
  //股票:pass
  /*
  int i=0;
  entity = (entity_t *)(market_list[0].list+i*entity_length);
  for(i=index; (i< entity_count)&&(i<(index+1)*size); i++){ 
    codeinfo = (CodeInfo *)(&request+off+i*codeinfo_length);
    codeinfo->code_type = market_list[0].code_type;
    strncpy(codeinfo->code, entity->code, 6);
    entity ++;
  }
  */
 
  if(send(socket_fd, request, request_length, 0)){
    printf("send success!\n");
  }
  
}

void init_receive(void * socket_fd)
{
  char head[4];
  char * buff = NULL;
  int package_body_length = 0;
  int * fd = (int *)socket_fd;
  int ret_count = 0;
  int head_length = 8;
  int length = head_length;  
  int off = 0;

  while(1){
    memset(&head, 0x00, 6);
    buff = (char *)malloc(length);
    if(buff == NULL){
      printf("malloc err!\n");
      exit(-1);
    }
    memset(buff, 0x00, length);
    //接受头部
    ret_count = read(socket_fd, buff,8);
    if(ret_count == 0){
      printf("connect close!\n");
      sleep(3);
      continue;
      //pthread_exit();
    } 
    if(ret_count <0){
      printf("recive server err!\n");
      sleep(3);
      continue;
      //pthread_exit(-1);
    }
    
    if(ret_count == length){
      //receive body of package
      strncpy(head, buff, 6);
      length = *((int *)(buff+6));
      free(buff);
      buff = (char *)malloc(length);
      if(buff == NULL){
	printf("malloc err!\n");
	exit(-1);
      }
      memset(buff, 0x00, length);
      while(length != (ret_count = read(socket_fd, buff+off, length))){
	off += ret_count;
	length -= ret_count;	
      }
      //parse
      printf("%s\n", buff);
      printf("recive complete!\n");
      
    }
  }
}

int send_auto_push(int socket_fd)
{
  char request[1024];

  RealPack data;
  memset(&data,0x00,sizeof(RealPack));
  memcpy(data.m_head,HEADER,4);
  data.m_length =  sizeof(RealPack) - 8;
  data.m_nType = TYPE_AUTO_PUSH;
  data.m_nSize =1;
  data.m_nOption= 0x0080;
  //memcpy(data.m_cCode2,"EURUSD",6);
  //data.m_cCodeType2 = 0x8100;

  memset(request, 0, sizeof(data));
  memcpy(request, &data, sizeof(data));
  int r = send(socket_fd, request, sizeof(data), 0);  
  printf("r:%d\n", r);
  printf("主推请求\n");
  return 0;
}
