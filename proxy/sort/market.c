#include "market.h"
#include "assert.h"

my_key_t key_root = {0};
extern market_t market_list[];

//get code for both market
int init_market()
{
  char buff[100*1024];
  //get both txt

  char template_str[][100] =  {
    "rm -f ./txt/%s* ./txt/%s*",
    "wget -P ./txt/ http://dsapp.yz.zjwtj.com:8010/initinfo/stock/%s",
    "./txt/%s"
  };
  char cmd[100];
  sort_area_queue_t * area_queue_item = NULL; 

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
  return 0;

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
  //market_list[index].entity_list_size = 12;
  if(market_list[index].list != NULL){free(market_list[index].list);}
  //init list
  assert(jim_malloc(market_list[index].entity_list_size*sizeof(entity_t), &market_list[index].list) == 0);
  if(market_list[index].sort_price_list != NULL){free(market_list[index].sort_price_list);}
  //init sort_price_list
  assert(jim_malloc(market_list[index].entity_list_size*sizeof(int *), &market_list[index].sort_price_list) == 0);
  if(market_list[index].sort_up_list != NULL){free(market_list[index].sort_up_list);}
  //init sort_up_list
  assert(jim_malloc(market_list[index].entity_list_size*sizeof(int *), &market_list[index].sort_up_list) == 0);
  if(market_list[index].sort_down_list != NULL){free(market_list[index].sort_down_list);}
  //init sort_down_list
  assert(jim_malloc(market_list[index].entity_list_size*sizeof(int *), &market_list[index].sort_down_list) == 0);

  int i = 0;
  cJSON * item;
  entity = market_list[index].list;
  int * item_sort_price = market_list[index].sort_price_list;
  int * item_sort_up    = market_list[index].sort_up_list;
  int * item_sort_down  = market_list[index].sort_down_list;
  int * yestoday_max_price = &market_list[index].yestoday_max;
  int * yestoday_min_price = &market_list[index].yestoday_min;
  *yestoday_min_price = 100000;
  char * code = NULL;
  int max = 0;
  for(; i< market_list[index].entity_list_size; i++){
    //if(max >12) break;
    item = cJSON_GetArrayItem(obj, i);
    //printf("code:%s\n", cJSON_GetObjectItem(item, "code")->valuestring);
    code = cJSON_GetObjectItem(item, "code")->valuestring;
    //strcpy(entity->code, cJSON_GetObjectItem(item, "code")->valuestring);
    strncpy(entity->code, code, 6);
    entity->code[6] = '\0';
    entity->pre_close = atoi(cJSON_GetObjectItem(item, "preclose")->valuestring);
    *item_sort_price = entity;
    *item_sort_up    = entity;
    *item_sort_down  = entity;
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
    assert(save_key(entity->code, 6, index, entity) == 0);

    entity ++;
    item_sort_price ++;
    item_sort_up ++;
    item_sort_down ++;
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

int find_entity_by_key(char * code, unsigned int code_len, int code_type_index)
{
  int location = 0;
  char ascii   = 0;
  int i        = 0;
  int address  = 0;
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
