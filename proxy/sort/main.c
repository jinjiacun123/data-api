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
#include<stdbool.h>
#include<signal.h>
#include "cJSON.h"
#include "comm.h"
#include "market.h"

int last_time_market;//effective time
int cur_time;        //current time
int heart_times = 0;

static void do_stock(market_t * my_market, unsigned short, char *, char *, int);

bool is_exit = false;
int socket_fd = 0;

market_t market_list[] = {
  //上证a股
  {"1101.txt","20161012",0x1101,1000,"[570-690][780-900][-1--1][-1--1]"},
  //深证a股
  {"1201.txt","20161012",0x1201,1000,"[570-690][780-900][-1--1][-1--1]"}
};

int main()
{
  pthread_t p_id = 0;
  int ret = 0;

  signal(SIGINT, sig_stop);
  //--receive both shanghhai and shenzhen market stock
  init_market();
  init_socket(&socket_fd);
  ret = pthread_create(&p_id, NULL, (void *)init_receive, (void *)&socket_fd);
  printf("ret:%d\n", ret);
  //---init data and sort
  //get realtime data
  send_realtime(socket_fd, 0, market_list[0].entity_list_size, 0);
  //---auto push data---
  //get auto push data and resort data
  //send_auto_push(socket_fd, 0, market_list[0].entity_list_size, 0);
  //send_auto_push(socket_fd, 0, 1, 0);

  int menu = 1;
  while(!is_exit){
    sleep(3);
    send_heart(socket_fd);
    heart_times++;
  }


  printf("exit system...\n");
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

int send_realtime(int socket_fd, int index, int size, int code_type_index)
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

int send_auto_push(int socket_fd, int index, int size, int code_type_index)
{
  char * request;
  int off = sizeof(RealPack_ex);
  int codeinfo_length = sizeof(CodeInfo);
  CodeInfo * codeinfo;
  entity_t * entity;
  int entity_length = sizeof(entity_t);
  int i = 0;
  int request_length = sizeof(RealPack_ex)+codeinfo_length*size;

  request = (char *)malloc(request_length);
  if(request == NULL){
    printf("malloc err!\n");
    exit(-1);
  }
  memset(request, 0x00, request_length);

  RealPack_ex * data = (RealPack_ex *)request;
  memcpy(data->m_head,HEADER,4);
  data->m_length =  request_length-8;
  data->m_nType = TYPE_AUTO_PUSH;
  data->m_nSize = size;
  data->m_nOption= 0x0080;

  entity = (entity_t *)(market_list[code_type_index].list+i*entity_length);
  for(i = index; i<(index+1)*size; i++){
    codeinfo = (CodeInfo *)(request + off + i * codeinfo_length);
    codeinfo->code_type = market_list[code_type_index].code_type;
    strncpy(codeinfo->code, entity->code, 6);
    entity ++;
  }

  if(send(socket_fd, request, request_length, 0)){
    printf("send auto_push success!\n");
    return 0;
  }

  return -1;
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

int parse(char * buff, uLongf  buff_len)
{
  unsigned short type;

  if(buff == NULL)
    return -2;
  type = (*(unsigned short *)buff);
  printf("type:%02x\n", type);
  switch(type){
  case TYPE_REALTIME:{
    printf("realtime...\n");
    parse_realtime(buff, buff_len);
    free(buff);
    //sort
    column_n sort_column = NEW_PRICE;
    //my_sort(0, sort_column);
    //display sort
    //display_sort(0);
    //    is_exit = true;
  }
    break;
  case TYPE_AUTO_PUSH:{
    printf("recieve auto_push...\n");
    parse_auto_push(buff, buff_len);
    // parse_auto_push(buff, buff_len);
    free(buff);
  }
    break;
  case TYPE_HEART:{
    printf("heart...\n");
    heart_times--;
  }
    break;
  case TYPE_ZIB:{
    printf("bzib...\n");
    char * src_buff = NULL;
    uLongf src_buff_len = 0;
    assert(unpack(buff, buff_len, &src_buff, &src_buff_len) == 0);
    free(buff);
    parse(src_buff, src_buff_len);
  }
    break;
  default:{
    printf("unknown type:%d...\n", type);
  }
    break;
  }
  return 0;
}

int parse_realtime(char * buff, uLongf buff_len)
{
  AskData2 * data_head = (AskData2 *)(buff);
  char code[7]={0};
  int i=0;
  int index = 0;
  market_t * my_market = NULL;

  for(i=0; i< data_head->m_nSize; i++){
    CommRealTimeData * data_type = (CommRealTimeData *)(buff
							+ 20
							+ i*(sizeof(CommRealTimeData)+sizeof(HSStockRealTime)));
    memcpy(code, data_type->m_cCode, 6);
    if(data_type->m_cCodeType == 0x1101){//股票
      my_market = &market_list[index];
      do_stock(my_market, data_type->m_cCodeType, code, buff, i, ADD);
    }
  }
  return 0;
}

//处理股票
static void
do_stock(my_market, code_type, code, buff, i, option)
     market_t * my_market;
     unsigned short code_type;
     char * code;
     char * buff;
     int i;
     option_n option;
{
  int address = 0;
  unsigned int code_type_index = 0;
  entity_t * entity;
  column_n column = NEW_PRICE;
  if(code_type == 0x1201){
    code_type_index = 1;
  }
  assert((address = find_entity_by_key(code, 6, code_type_index)) > 0);
  entity = (entity_t *)address;

  HSStockRealTime * tmp = (HSStockRealTime *)(buff
					      +20
					      +sizeof(CommRealTimeData)
					      +i*(sizeof(CommRealTimeData)+sizeof(HSStockRealTime)));
  /*
  printf("index:%d,code_type:%2x,code:%s, new_price:%d\n",
	 i,
	 code_type,
	 code,
	 tmp->m_lNewPrice);
  */
  entity->price = tmp->m_lNewPrice;
  //add to sort
  sort_add(my_market, entity, column);
  //printf("index:%d\n", i);

  printf("index:%d,code_type:%2x,code:%s, new_price:%d\n",
	 i,
	 code_type,
	 code,
	 entity->price);
}

int parse_auto_push(char * buff, uLong   buff_len)
{
  printf("parse auto_push...\n");
  AskData2 * data_head = (AskData2 *)(buff);
  char code[7]={0};
  int i=0;
  int index = 0;
  market_t * my_market = NULL;

  for(i=0; i< data_head->m_nSize; i++){
    CommRealTimeData * data_type = (CommRealTimeData *)(buff
							+ 20
							+ i*(sizeof(CommRealTimeData)+sizeof(HSStockRealTime)));
    memcpy(code, data_type->m_cCode, 6);
    if(data_type->m_cCodeType == 0x1101){//股票
      my_market = &market_list[index];
      do_stock(my_market, data_type->m_cCodeType, code, buff, i, UPDATE);
    }
  }

  return 0;
}

int unpack(char * des_buff, uLongf des_buff_len, char ** src_buff, uLongf * src_buff_len)
{
  TransZipData2   * zheader;
  zheader = (TransZipData2 *)des_buff;
  if(zheader->m_nType != TYPE_ZIB){
    printf("parse zlib package type error!\n");
    return -100;
  }

  *src_buff_len = (uLongf)zheader->m_lOrigLen;
  *src_buff = (char *)malloc(*src_buff_len);
  if(*src_buff == NULL){
    printf("malloc err!\n");
    return -1;
  }
  memset(*src_buff, 0 , *src_buff_len);

  int unzip =  uncompress((Bytef *)(*src_buff), src_buff_len,
			  (Bytef*)zheader->m_cData, (uLongf)zheader->m_lZipLen);
  if(unzip == Z_MEM_ERROR){
    printf("memory not enough\n");
    return -2;
  }
  if(unzip == Z_BUF_ERROR){
    printf("buff not enough!\n");
    return -3;
  }
  if(unzip == Z_DATA_ERROR){
    printf("unpack data err!\n");
    return -4;
   }
  if(unzip == Z_OK
     && *((long*)src_buff_len) == zheader->m_lOrigLen){
    // my_buff->p_res_media_h = (p_response_meta_header)my_buff->unpack_buff;
    return 0;
    //parse(my_buff);
    //return;
  }
  return -1;
}

int get_index_by_code_ascii(char ascii)
{
  int base_digit = 0;
  int base_alphabet = 10;
  //digit
  if(ascii>=48 || ascii <=57){
    return base_digit+ascii-48;
  }

  //alphabet
  if(ascii>=65 || ascii <= 80){
    return base_alphabet+ascii-65;
  }

  return -1;
}

void sig_stop(int signo)
{
  printf("abovt exit!\n");
  shutdown(socket_fd, SHUT_RDWR);
  //close(socket_fd);
  exit(-1);
}
