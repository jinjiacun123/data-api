#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<assert.h>
#include "cJSON.h"

#define SERVER_MARKET_PRE "http://dsapp.yz.zjwtj.com:8010/initinfo/stock/"
#define SERVER_MARKET 127.0.0.1
#define SERVER_MARKET_PORT 8001

char buff[100*1024];

typedef struct{
  char code[6];
  char name[20];
  int  pre_close;  
}entity_t;

typedef struct{
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
int get_content(char * filename, char * buff, int length);
int get_market(cJSON * root_json, int index);

int main()
{
  //--receive both shanghhai and shenzhen market stock
  init_market();
  //---init data and sort
  //get realtime data
  //sort by price

  //---auto push data---
  //get auto push data and resort data
  
  
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
  //parse 1201r
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
