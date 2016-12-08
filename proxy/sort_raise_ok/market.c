#include "market.h"
#include "assert.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

my_key_t key_root = {0};
extern market_t market_list[];

//get code for both market
int init_market()
{
  char * data_file_name = "./txt/my_data";
  FILE * fp = fopen(data_file_name, "r");

  int size = 0;
  int index = 0;
  if(fp == 0){
    printf("read my_data err!\n");
    return -1;
  }
  fseek(fp, 0L, SEEK_END);
  size = ftell(fp)/14;
  if(size == 0){
    printf("get file size err!\n");
    return -1;
  }
  fseek(fp, 0L, SEEK_SET);
  //stock
  size = 10;
  get_market(index, fp, size);
  close(fp);
  return 0;
}

int out_market(int code_type_index)
{
  int i = 0;
  entity_t * entity;
  char * template = "code_type:%2x\tcode:%s\tprice:%d\n";

  entity = market_list[code_type_index].list;
  for(; i<market_list[code_type_index].entity_list_size; i++){
    printf(template, market_list[code_type_index].code_type,
	   entity->code,
	   entity->price);
  }
  return 0;
}

int get_market(int index, FILE * fp, int size)
{
  entity_t * entity;
  int entity_list_len = 0;
  char buff[14];
  int ret = -1;

  market_list[index].entity_list_size = size;
  //market_list[index].entity_list_size = 12;
  if(market_list[index].list != NULL){free(market_list[index].list);}
  //init list
  ret = jim_malloc(market_list[index].entity_list_size*sizeof(entity_t), &market_list[index].list);
  assert(ret == 0);

  int i = 0;
  entity = market_list[index].list;
  int * yestoday_max_price = &market_list[index].yestoday_max;
  int * yestoday_min_price = &market_list[index].yestoday_min;
  *yestoday_min_price      = 100000;
  char * code = NULL;
  unsigned short code_type = 0;
  int max = 0;
  char *c = NULL;
  for(; i< market_list[index].entity_list_size; i++){
    memset(&buff, 0x00, 14);
    //if(max >12) break;
    c = fread(buff, sizeof(unsigned char), 14, fp);
    if(c == 0){
      printf("read err!\n");
      return -1;
    }
    //printf("code:%s\n", cJSON_GetObjectItem(item, "code")->valuestring);
    entity->type = *(int*)buff;
    //strcpy(entity->code, cJSON_GetObjectItem(item, "code")->valuestring);
    strncpy(entity->code, buff+8, 6);
    entity->code[6] = '\0';
    entity->pre_close = *(int*)(buff+4);
    //find max and min of price
    if(entity->pre_close != 0){
      if(*yestoday_max_price < entity->pre_close){ *yestoday_max_price = entity->pre_close;}
      if(*yestoday_min_price > entity->pre_close){ *yestoday_min_price = entity->pre_close;}
    }

    printf("index:%d\tcode:%s\tpreclose:%d\n",
	   i,
	   entity->code,
	   entity->pre_close);
    //save to key
    switch(entity->type){
    case 0x1101:{
      assert(save_key(entity->code, 6, 0, entity) == 0);
    }break;
    case 0x1201:{
      assert(save_key(entity->code, 6, 1, entity) == 0);
    }break;
    case 0x1206:{
      assert(save_key(entity->code, 6, 2, entity) == 0);
    }break;
    case 0x120b:{
      assert(save_key(entity->code, 6, 3, entity) == 0);
    }break;
    }

    entity ++;
    //item_sort_price ++;
    //item_sort_raise ++;
    max ++;
  }

  printf("yestoday_max:%d\tyestoday_min:%d\m", 
	 *yestoday_max_price,
	 *yestoday_min_price);
  //set setting_price
  market_list[index].setting_max = market_list[index].yestoday_max * 1.1;
  market_list[index].setting_min = market_list[index].yestoday_min * 0.9;

  //init sort area
  init_sort_area(&market_list[index]);

  printf("date:%s\tcode_type:%x\tunit:%d\topen_close_time:%s\tcode_size:%d\n",
	 market_list[index].date,
	 market_list[index].code_type,
	 market_list[index].unit,
	 market_list[index].open_close_time,
	 market_list[index].entity_list_size);

  return 0;
}

/**
   from first letter to last,every letter point floor
*/
int save_key(char * code, unsigned code_len, int code_type_index, entity_t * entity)
{
  int i              = 0;
  char ascii         = 0;
  int unit           = 0;
  my_key_t * cur_key = &key_root;
  my_key_t * tmp_key = NULL;;
  void * last_key    = NULL;
  int location       = 0;

  for(i=0; i<6; i++){
    ascii = *(code+i);
    location = get_index_by_code_ascii(ascii);
    //check location is malloc
    if(cur_key->childs[location] == NULL){
      tmp_key = (my_key_t *)malloc(sizeof(my_key_t));
      if(tmp_key == NULL){
	printf("malloc error!\n");
	exit(-1);
      }
      memset(tmp_key, 0x00, sizeof(my_key_t));
      tmp_key->floor = i+1;
      cur_key->childs[location] = tmp_key;
      cur_key = tmp_key;
    }
    else{
      cur_key = cur_key->childs[location];
    }
  }
  //code_type node
  //save entity of point
  if(cur_key != NULL && cur_key->childs[code_type_index] == NULL){
    cur_key->childs[code_type_index] = entity;
  }

  return 0;
}

void * find_entity_by_key(char * code, unsigned int code_len, int code_type_index)
{
  int location = 0;
  char ascii   = 0;
  int i        = 0;
  void * address  = NULL;
  my_key_t * cur_key = &key_root;

  //from first to sixth bit
  for(i=0; i<6; i++){
    ascii = *(code+i);
    location = get_index_by_code_ascii(ascii);
    cur_key = cur_key->childs[location];
  }

  //7th bit
  location = code_type_index;
  address = cur_key->childs[location];

  return address;
}
