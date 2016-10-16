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
#include "config.h"

static void do_stock(char *, char *, int);

int main()
{
  pthread_t p_id = 0;
  int socket_fd = 0;
  int ret = 0;
  //--receive both shanghhai and shenzhen market stock
  init_market();
  init_socket(&socket_fd);
  ret = pthread_create(&p_id, NULL, (void *)init_receive, (void *)&socket_fd);
  printf("ret:%d\n", ret);
  //init_receive(&socket_fd);
  //---init data and sort
  //get realtime data
  send_realtime(socket_fd, 0, 1);
  //sort by price

  //---auto push data---
  //get auto push data and resort data
  int menu = 1;
  while(1){
    sleep(3);
    send_heart(socket_fd);
    heart_times++;
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

  *socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(*socket_fd < 0){
    printf("socket() failrue!\n");
    return -1;
  }

  if(connect(*socket_fd, (struct sockaddr*)&cli, sizeof(cli)) < 0){
    printf("connect() failure!\n");
    return -1;
  } 
  printf("connect() success\n");
  return 0;
}

int send_realtime(int socket_fd, int index, int size)
{
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
  data->m_nSize = size;
  data->m_nOption= 0x0080;
  
  //股票:pass  
  int i=0;
  entity = (entity_t *)(market_list[0].list+i*entity_length);
  for(i=index; i<(index+1)*size; i++){ 
    codeinfo = (CodeInfo *)(request+off+i*codeinfo_length);
    codeinfo->code_type = market_list[0].code_type;
    strncpy(codeinfo->code, entity->code, 6);
    entity ++;
  }
 
  if(send(socket_fd, request, request_length, 0)){
    printf("send success!\n");
    return 0;
  }
  
  return -1;
}

void init_receive(void * socket_fd)
{
  char head[4];
  char * buff = NULL;
  int package_body_length = 0;
  int ret_count = 0;
  int head_length = 8;
  int length = head_length;  
  int off = 0;
  int fd = *((int *)socket_fd);

   while(1){
    memset(&head, 0x00, 4);
    buff = (char *)malloc(length);
    if(buff == NULL){
      printf("malloc err!\n");
      exit(-1);
    }
    memset(buff, 0x00, length);
    //接受头部
    ret_count = read(fd, buff,length);
    printf("fd:%d\n", fd);
    if(ret_count == 0){
      printf("connect close!\n");
      sleep(3);
      return 0;
      //      continue;
      //pthread_exit();
    } 
    if(ret_count <0){
      printf("recive server err!\n");
      sleep(3);
            return 0;
      // continue;
      //pthread_exit(-1);
    }
    
    if(ret_count == length){
      //receive body of package
      strncpy(head, buff, 4);
      length = *((int *)(buff+4));
      free(buff);
      buff = (char *)malloc(length);
      if(buff == NULL){
	printf("malloc err!\n");
	exit(-1);
      }
      memset(buff, 0x00, length);
      while(length != (ret_count = read(fd, buff+off, length))){
	off += ret_count;
	length -= ret_count;	
      }
      //parse
      printf("%s\n", buff);
      printf("recive complete!\n");
      parse(buff, length);
    }
   }
}

int send_auto_push(int socket_fd, int index, int size)
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

int send_heart(int socket_fd)
{
  char request[1024];
  TestSrvData2 data ;
  memset(&data,0x00,sizeof(TestSrvData2));
  memcpy(data.head, HEADER,4);
  data.length      = sizeof(TestSrvData2) -8;    
  data.m_nType     = TYPE_HEART;
  data.m_nIndex= 1;

  memset(request, 0, 1024);
  memcpy(request, &data, sizeof(data));
  send(socket_fd, request, sizeof(data), 0);
  printf("心跳请求发送完毕\n");
  return 0;
}

int parse(char * buff, int buff_len)
{
  unsigned short type;
  
  type = (*(unsigned short *)buff);
  printf("type:%x\n", type);
  switch(type){
  case TYPE_REALTIME:{
    printf("realtime...\n");
    parse_realtime(buff, buff_len);
  }
    break;
  case TYPE_AUTO_PUSH:{
    printf("auto_push...\n");
    parse_auto_push(buff, buff_len);
  }
    break;
  case TYPE_HEART:{
    printf("heart...\n");
    heart_times--;
  }
    break;
  default:{
    printf("unknown type...\n");
  }
    break;
  }
  return 0;
}

int parse_realtime(char * buff, int buff_len)
{
  AskData2 * data_head = (AskData2 *)(buff);
  char code[7]={0};
  int i=0;

  for(i=0; i< data_head->m_nSize; i++){
    CommRealTimeData * data_type = (CommRealTimeData *)(buff
							+ 20
							+ i*(sizeof(CommRealTimeData)+sizeof(HSStockRealTime)));
    memcpy(code, data_type->m_cCode, 6);
    if(data_type->m_cCodeType == 0x1101){//股票
	do_stock(code, buff, i);
    }
  }
  return 0;
}

//处理股票
static void
do_stock(code, buff, i)
     char * code;
     char * buff;
     int i;
{
  printf("处理股票...\n");
  HSStockRealTime * tmp = (HSStockRealTime *)(buff
					            +20
					      +sizeof(CommRealTimeData)
					      +i*(sizeof(CommRealTimeData)+sizeof(HSStockRealTime)));
  printf("品种:%s, 最新价:%d\n", code, tmp->m_lNewPrice);
}


int parse_auto_push(char * buff, int buff_len)
{
  return 0;
}
