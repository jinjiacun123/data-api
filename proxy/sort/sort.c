#include "config.h"
#include "cJSON.h"
#include "sort.h"
#include "market.h"
#include <assert.h>
#include <stdbool.h>

static int sort_area(market_t * my_market, column_n column);
static int find_location(market_t * my_market, entity_t * entity, column_n column, int * area_index, int * queue_index);
static int remove_entity(market_t * my_market, entity_t * entity, column_n column);
static int search_queue_index(int value, int begin, int length, sort_area_queue_t * queue, column_n column);
extern market_t market_list[];

//init area and queueof area
//malloc queue and setting area value
int init_sort_area(my_market)
     market_t * my_market;
{
  int i = 0;
  sort_area_t * sort_area = NULL;
  sort_area_t * sort_area_zero = NULL;
  int step_len = (my_market->setting_max - my_market->setting_min)/AREA_NUMBER;
  int area_queue_len = 0;
  int default_area_queue_len = AREA_NUMBER*AREA_QUEUE_DEFAULT_LEN*sizeof(sort_area_queue_t);
  int page_size = ceil(1.0*default_area_queue_len/PAGE_SIZE);
  area_queue_len = page_size * PAGE_SIZE;
  sort_area_queue_t * base_queue = (char *)malloc(area_queue_len);
  if(base_queue == NULL){
    printf("malloc error!\n");
    return -1;
  }
  memset(base_queue, 0x00, area_queue_len);

  /*
  //deal zero
  sort_area_zero = &my_market->sort_area_price_zero;
  sort_area_zero->cur = (sort_area_queue_t *)malloc(area_queue_len);
  if(sort_area_zero->cur == NULL){
    printf("malloc err!\n");
    return -1;
  }
  memset(sort_area_zero->cur, 0x00, default_area_queue_len+1);
  */

  sort_area = &my_market->sort_area_price;
  for(; i< AREA_NUMBER; i++){
    sort_area->cur = base_queue +AREA_QUEUE_DEFAULT_LEN * i;
    sort_area->min_value.ivalue = my_market->setting_min + step_len * i;
    sort_area->max_value.ivalue = sort_area->min_value.ivalue + step_len +1;
    sort_area->allow_size = AREA_QUEUE_DEFAULT_LEN;

    sort_area ++;
  }

  return 0;
}

//from samll to big
int my_sort(my_market, column)
  market_t * my_market;
  column_n column;
{
  //sort_list(code_type_index, column);
  sort_area(my_market, column);

  return 0;
}

int sort_add(my_market, entity, column)
     market_t * my_market;
     entity_t * entity;
     column_n column;
{
  int i = 0;
  int area_index = 0;
  int queue_index = 0;
  int pre_ivalue = 0;
  float pre_fvalue = 0;
  sort_area_t * cur_area = NULL;
  sort_area_queue_t * cur_queue = NULL, * pre_queue = NULL;

  //find entity from area
  assert(find_location(my_market, entity, column, &area_index, &queue_index) == 0);
  entity->price_area[0] = area_index;
  entity->price_area[1] = queue_index;
  //check is exits,by pre_*
  switch(column){
  case NEW_PRICE:{
    cur_area = &my_market->sort_area_price[area_index];

    if(queue_index == 0){//first
      if(cur_area->real_size == 0){
	cur_queue = cur_area->cur;
	cur_queue->entity = entity;
	cur_queue->index = 0;
      }
      else{
	i = cur_area->real_size;
	pre_queue = cur_area->cur + i -1;
	cur_queue = cur_area->cur + i;
	for(; i>queue_index; i--){
	  cur_queue->index = pre_queue->index+1;
	  cur_queue->entity = pre_queue->entity;
	  cur_queue --;
	  pre_queue --;
	}
	cur_queue = cur_area->cur + queue_index;
	cur_queue->entity = entity;
	cur_queue->index = queue_index;
      }
    }
    else if(queue_index == cur_area->real_size){//last
      cur_queue = cur_area->cur + queue_index;
      cur_queue->entity = entity;
      cur_queue->index = queue_index;
    }
    else if(queue_index < cur_area->real_size){//in btween real_size
      //move
      i = cur_area->real_size;
      pre_queue = cur_area->cur + i -1;
      cur_queue = cur_area->cur + i;
      for(; i>queue_index; i--){
	cur_queue->index = pre_queue->index+1;
	cur_queue->entity = pre_queue->entity;
	cur_queue --;
	pre_queue --;
      }
      cur_queue = cur_area->cur + queue_index;
      cur_queue->entity = entity;
      cur_queue->index = queue_index;
    }
    else{//not enough and remalloc
      cur_queue = cur_area->cur + queue_index;
      cur_queue->index = queue_index;
      cur_queue->entity = entity;
    }

    cur_area->real_size ++;
  }break;
  default:{

  }break;
  }

  return 0;
}

int sort_update(my_market, entity, column)
     market_t * my_market;
     entity_t * entity;
     column_n column;
{
  //remove
  remove_entity(my_market, entity, column);
  //add
  sort_add(my_market, entity, column);

  return 0;
}

//find area and queue by entity
static int find_location(my_market, entity, column, area_index, queue_index)
     market_t * my_market;
     entity_t * entity;
     column_n column;
     int * area_index;
     int * queue_index;
{
  int i = 0;
  sort_area_t * area = &my_market->sort_area_price;
  sort_area_queue_t * queue = NULL;
  int real_size = 0;
  value_t value;
  entity_t * item = NULL;
  sort_area_queue_t * tmp_queue = NULL;

  switch(column){
  case NEW_PRICE:{
    value.ivalue = entity->price;
  }break;
  default:{

  }break;
  }

  //find area
  for(; i<AREA_NUMBER; i++){
    if(value.ivalue > area->max_value.ivalue){
      area ++;
      continue;
    }

    *area_index = i;
    real_size = area->real_size;
    queue = area->cur;
    break;
  }

  //find queue
  if(area->real_size == 0){
    *queue_index = 0;
  }
  else{
    if(area->real_size< area->allow_size){//enough
      //i = real_size/2;
      //queue = queue+i;
      //      *queue_index = search_queue_index(value.ivalue, 0, real_size, queue, column);
      for(i=0; i < real_size; i++){
	item = queue->entity;
	if(value.ivalue > item->price){
	  queue++;
	  continue;
	}
	else{
	  *queue_index = i;
	  break;
	}
      }

      if(i == real_size){
	*queue_index = i;
      }
    }
    else{//not enough, remalloc
      tmp_queue = (sort_area_queue_t *)malloc(2 * area->allow_size * sizeof(sort_area_queue_t));
      if(tmp_queue == NULL){
	printf("malloc err!\n");
	return -1;
      }
      memset(tmp_queue, 0x00, 2 * area->allow_size * sizeof(sort_area_queue_t));
      //copy old to new
      memcpy(tmp_queue, area->cur, area->allow_size * sizeof(sort_area_queue_t));
      //sort_area_queue_t * ttt = area->cur;
      area->cur = tmp_queue;
      area->allow_size = area->allow_size *  2;
      *queue_index = area->real_size;
      //free(ttt);
    }
  }

  return 0;
}

//from pre_value remove prefix time value,and update relatively area
static int remove_entity(my_market, entity, column)
     market_t * my_market;
     entity_t * entity;
     column_n column;
{
  int i = 0;
  int area_index = 0;
  int queue_index = 0;
  sort_area_t * cur_area = NULL;
  sort_area_queue_t * cur_queue = NULL;
  sort_area_queue_t * after_queue = NULL;

  area_index = entity->price_area[0];
  queue_index = entity->price_area[1];

  switch(column){
  case NEW_PRICE:{
    cur_area = &my_market->sort_area_price[area_index];
    if(queue_index != cur_area->real_size-1){//not last
      i = queue_index;
      cur_queue = cur_area->cur+i;
      after_queue = cur_area->cur +i + 1;
      for(; i < cur_area->real_size-1 ; i++){
	cur_queue->entity = after_queue->entity;
	cur_queue ++;
	after_queue ++;
      }
      /*
      after_queue = cur_area->cur + cur_area->real_size -1;
      after_queue->index = -1;
      after_queue->entity = NULL;
      */
      after_queue->index = -1;
      after_queue->entity = NULL;
    }
    else if(queue_index == cur_area->real_size-1){//last
      cur_queue = cur_area->cur+cur_area->real_size-1;
      cur_queue->index = -1;
      cur_queue->entity = NULL;
    }
    cur_area->real_size --;
  }break;
  default:{

  }
  }

  return 0;
}

static int sort_area(my_market, column)
     market_t * my_market;
     column_n column;
{
  return 0;
}


static int sort_list(my_market, column)
     market_t * my_market;
     column_n column;
{
  int i = 0, j = 0;
  int size = market_list[0].entity_list_size;
  entity_t *p, *q, *swap;
  int sort_size = sizeof(market_t *);
  int *ip = 0, *iq = 0, iswap = 0;

  printf("begin sort...\n");

  for(i=0; i<size; i++){
    for(j=i+1; j< size; j++){
      ip = my_market->sort_price_list+(j-1);
      p = (entity_t*)(*(my_market->sort_price_list+(j-1)));
      iq = my_market->sort_price_list+j;
      q = (entity_t *)(*(my_market->sort_price_list+j));
      if(p->price > q->price){
	iswap = *ip;
	*ip = *iq;
	*iq = iswap;
      }
    }
  }
  printf("sort complete...\n");

  return 0;
}

int display_sort(my_market)
     market_t * my_market;
{
  entity_t * entity;
  int i = 0;
  char * template = "code:%s\tprice:%d\n";

  for(; i<my_market->entity_list_size; i++){
    entity = (entity_t *)(*(my_market->sort_price_list+i));
    printf(template, entity->code, entity->price);
  }
  printf("display sort complete...\n");

  return 0;
}

int sort_get(my_market, index, size, entity_list)
     market_t * my_market;
     int index;
     int size;
     entity_t * entity_list;
{
  sort_area_t * area = NULL;
  int cur_real_size = 0;
  area = &my_market->sort_area_price;
  entity_t * entity_target = &entity_list[0];
  entity_t * entity = NULL;
  int off = index;
  int off_size = size;
  int i = 0, j = 0;
  int entity_index = 0;
  bool is_finish = false;
  int begin = 1;

  if(entity_list == NULL){
    printf("malloc error!\n");
    exit(-1);
  }

  for(i = 0; i< AREA_NUMBER; i++){
    cur_real_size = area->real_size;
    if(off > cur_real_size){
      off -= cur_real_size;
      area ++;
      continue;
    }else{
      break;
    }
  }

  begin = off;

  int part_end = 0;
  //target area and queue
  if(area == NULL) return -1;
  if(size > cur_real_size - begin){
    for(j = begin; j< cur_real_size; j++){
      entity_list[entity_index] = *(area->cur+j)->entity;
      entity_index ++;
      entity_target ++;
      off_size --;
    }
  }else{
    part_end = begin + size;
    for(j = begin; j< part_end; j++){
      entity_list[entity_index] = *(area->cur+j)->entity;
      entity_index ++;
      entity_target ++;
      off_size --;
    }
  }
  while(off_size >0){
    area++;
    if(area == NULL) break;
    cur_real_size = area->real_size;if(cur_real_size == 0) continue;
    if(off_size > cur_real_size){//area all
      for(j = 0; j< cur_real_size; j++){
	entity_list[entity_index] = *(area->cur + j)->entity;
	entity_target ++;
	entity_index ++;
	off_size --;
      }
    }else{//area part
      part_end = off_size;
      for(j = 0; j< part_end; j++){
	entity_list[entity_index] = *(area->cur + j)->entity;
	entity_target ++;
	entity_index ++;
	off_size --;
      }
    }
  }

  // entity = entity_list[0];
  /*
  for(i = 0; i< size; i++){
    printf("code:%s\tprice:%d\n", entity_list[i].code, entity_list[i].price);
    entity ++;
    }
  */

  return 0;
}

static int search_queue_index(value, begin, length, queue, column)
     int value;
     int begin;
     int length;
     sort_area_queue_t * queue;
     column_n column;
{
  if( begin > length){
    return -1;
  }
  else{
    if(value == (queue+((begin + length)/2))->entity->price){
      return (begin+length)/2;
    }
    else{
      if(value < (queue+((begin + length)/2))->entity->price){
	return search_queue_index(value, begin, ((begin + length)/2) -1, queue, column);
      }
      else{
	return search_queue_index(value, ((begin + length)/2)+1, length, queue, column);
      }
    }
  }

  return 0;
}
