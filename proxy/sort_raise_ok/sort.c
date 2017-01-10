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
  sort_area_t * price_sort_area = NULL;
  sort_area_t * raise_sort_area = NULL;
  int price_step_len = 0;
  int raise_step_len = 0;
  price_step_len = (my_market->setting_max - my_market->setting_min)/AREA_NUMBER;
  raise_step_len = (my_market->yestoday_max *0.1*2)/AREA_NUMBER;
  int raise_min = (-1)*(my_market->yestoday_max * 0.1);
  int price_area_queue_len = 0;
  int raise_area_queue_len = 0;
  int default_area_queue_len = AREA_NUMBER*AREA_QUEUE_DEFAULT_LEN*sizeof(sort_area_queue_t);
  int price_page_size = 0;
  int raise_page_size = 0;
  price_page_size = default_area_queue_len;
  raise_page_size = default_area_queue_len;
  //price_page_size = ceil(1.0*default_area_queue_len/PAGE_SIZE);
  //raise_page_size = ceil(1.0*default_area_queue_len/PAGE_SIZE);
  //price_area_queue_len = price_page_size * PAGE_SIZE;
  //raise_area_queue_len = raise_page_size * PAGE_SIZE;
  price_area_queue_len = price_page_size;
  raise_area_queue_len = raise_page_size;
  sort_area_queue_t * price_base_queue = (char *)malloc(price_area_queue_len);
  sort_area_queue_t * raise_base_queue = (char *)malloc(raise_area_queue_len);
  if(price_base_queue == NULL){
    printf("malloc error!\n");
    return -1;
  }
  memset(price_base_queue, 0x00, price_area_queue_len);
  if(raise_base_queue == NULL){
    printf("malloc error!\n");
    return -1;
  }
  memset(raise_base_queue, 0x00, raise_area_queue_len);

  price_sort_area = &my_market->sort_area_price;
  raise_sort_area = &my_market->sort_area_raise;
  for(; i< AREA_NUMBER; i++){
    price_sort_area->cur = price_base_queue + AREA_QUEUE_DEFAULT_LEN * i;
    raise_sort_area->cur = raise_base_queue + AREA_QUEUE_DEFAULT_LEN * i;

    price_sort_area->min_value.ivalue = my_market->setting_min + price_step_len * i;
    price_sort_area->max_value.ivalue = price_sort_area->min_value.ivalue + price_step_len +1;
    raise_sort_area->min_value.ivalue = raise_min + raise_step_len *i;
    raise_sort_area->max_value.ivalue = raise_sort_area->min_value.ivalue + raise_step_len + 1;

    price_sort_area->real_size  = 0;
    raise_sort_area->real_size  = 0;
    price_sort_area->allow_size = AREA_QUEUE_DEFAULT_LEN;
    raise_sort_area->allow_size = AREA_QUEUE_DEFAULT_LEN;

    price_sort_area ++;
    raise_sort_area ++;
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
  int ret = -1;

  switch(column){
  case NEW_PRICE:{
  //find entity from area
    ret = find_location(my_market, entity, column, &area_index, &queue_index);
    assert(ret == 0);
    entity->price_area[0] = area_index;
    entity->price_area[1] = queue_index;
  }break;
  case RAISE:{
    ret = find_location(my_market, entity, column, &area_index, &queue_index);
    assert(ret == 0);
    entity->raise_area[0] = area_index;
    entity->raise_area[1] = queue_index;
  }break;
  default:
    break;
  }
  //check is exits,by pre_*
  switch(column){
  case NEW_PRICE:{
    cur_area = &my_market->sort_area_price[area_index];

    if(queue_index == 0){//first
      if(cur_area->real_size == 0){
	cur_queue = cur_area->cur;
	cur_queue->index = 0;
	cur_queue->entity = entity;
	cur_queue->entity->price_area[1] = cur_queue->index;
      }
      else{
	i = cur_area->real_size;
	pre_queue = cur_area->cur + i -1;
	cur_queue = cur_area->cur + i;
	for(; i>queue_index; i--){
	  cur_queue->index = pre_queue->index+1;
	  cur_queue->entity = pre_queue->entity;
	  cur_queue->entity->price_area[1] = cur_queue->index;
	  cur_queue --;
	  pre_queue --;
	}
	cur_queue = cur_area->cur + queue_index;
	cur_queue->entity = entity;
	cur_queue->index = queue_index;
	cur_queue->entity->price_area[1] = cur_queue->index;
      }
    }
    else if(queue_index == cur_area->real_size){//last
      cur_queue = cur_area->cur + queue_index;
      cur_queue->entity = entity;
      cur_queue->index = queue_index;
      cur_queue->entity->price_area[1] = queue_index;
    }
    else if(queue_index < cur_area->real_size){//in btween real_size
      //move
      i = cur_area->real_size;
      pre_queue = cur_area->cur + i -1;
      cur_queue = cur_area->cur + i;
      for(; i>queue_index; i--){
	cur_queue->index = pre_queue->index+1;
	cur_queue->entity = pre_queue->entity;
	cur_queue->entity->price_area[1] = cur_queue->index;
	cur_queue --;
	pre_queue --;
      }
      cur_queue = cur_area->cur + queue_index;
      cur_queue->entity = entity;
      cur_queue->index = queue_index;
      cur_queue->entity->price_area[1] = cur_queue->index;
    }
    else{//not enough and remalloc
      cur_queue = cur_area->cur + queue_index;
      cur_queue->index = queue_index;
      cur_queue->entity = entity;
      cur_queue->entity->price_area[1] = cur_queue->index;
    }

    cur_area->real_size ++;
  }break;
  case RAISE:{
    cur_area = &my_market->sort_area_raise[area_index];

    if(queue_index == 0){//first
      if(cur_area->real_size == 0){
	cur_queue = cur_area->cur;
	cur_queue->index = 0;
	cur_queue->entity = entity;
	cur_queue->entity->raise_area[1] = cur_queue->index;
      }
      else{
	i = cur_area->real_size;
	pre_queue = cur_area->cur + i -1;
	cur_queue = cur_area->cur + i;
	for(; i>queue_index; i--){
	  cur_queue->index = pre_queue->index+1;
	  cur_queue->entity = pre_queue->entity;
	  cur_queue->entity->raise_area[1] = cur_queue->index;
	  cur_queue --;
	  pre_queue --;
	}
	cur_queue = cur_area->cur + queue_index;
	cur_queue->entity = entity;
	cur_queue->index = queue_index;
	cur_queue->entity->raise_area[1] = cur_queue->index;
      }
    }
    else if(queue_index == cur_area->real_size){//last
      cur_queue = cur_area->cur + queue_index;
      cur_queue->entity = entity;
      cur_queue->index = queue_index;
      cur_queue->entity->raise_area[1] = queue_index;
    }
    else if(queue_index < cur_area->real_size){//in btween real_size
      //move
      i = cur_area->real_size;
      pre_queue = cur_area->cur + i -1;
      cur_queue = cur_area->cur + i;
      for(; i>queue_index; i--){
	cur_queue->index = pre_queue->index+1;
	cur_queue->entity = pre_queue->entity;
	cur_queue->entity->raise_area[1] = cur_queue->index;
	cur_queue --;
	pre_queue --;
      }
      cur_queue = cur_area->cur + queue_index;
      cur_queue->entity = entity;
      cur_queue->index = queue_index;
      cur_queue->entity->raise_area[1] = cur_queue->index;
    }
    else{//not enough and remalloc
      cur_queue = cur_area->cur + queue_index;
      cur_queue->index = queue_index;
      cur_queue->entity = entity;
      cur_queue->entity->raise_area[1] = cur_queue->index;
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
  sort_area_t * area = NULL;
  sort_area_t * pre_area = NULL;
  int tmp_value = 0;
  switch(column){
  case NEW_PRICE:
    area = &my_market->sort_area_price;
    break;
  case RAISE:
    area = &my_market->sort_area_raise;
    break;
  default:
    break;
  }
  sort_area_queue_t * queue = NULL;
  int real_size = 0;
  value_t value;
  entity_t * item = NULL;
  sort_area_queue_t * tmp_queue = NULL;

  switch(column){
  case NEW_PRICE:{
    value.ivalue = entity->price;
  }break;
  case RAISE:{
    value.ivalue = entity->raise;
  }break;
  default:{

  }break;
  }

  pre_area = area;
  //find area
  for(; i<AREA_NUMBER; i++){
    if(value.ivalue > area->max_value.ivalue){
      pre_area = area;
      area ++;
      continue;
    }

    *area_index = i;
    real_size = area->real_size;
    queue = area->cur;
    break;
  }
  if(i == AREA_NUMBER){
    area = pre_area;
    i = AREA_NUMBER -1;
    *area_index = i;
    real_size = area->real_size;
    queue = area->cur;
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
	switch(column){
	case NEW_PRICE:
	  tmp_value = item->price;
	  break;
	case RAISE:
	  tmp_value = item->raise;
	  break;
	}
	if(value.ivalue > tmp_value){
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
      if(area->allow_size != AREA_QUEUE_DEFAULT_LEN){ 
      	sort_area_queue_t * ttt = area->cur;
	free(ttt);
      }	
      area->cur = tmp_queue;
      area->allow_size = area->allow_size *  2;
      *queue_index = area->real_size;
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

  switch(column){
  case NEW_PRICE:{
    area_index  = entity->price_area[0];
    queue_index = entity->price_area[1];
  }break;
  case RAISE:{
    area_index  = entity->raise_area[0];
    queue_index = entity->raise_area[1];
  }break;
  default:
    break;
  }

  switch(column){
  case NEW_PRICE:{
    cur_area = &my_market->sort_area_price[area_index];
    if(queue_index != cur_area->real_size-1){//not last
      i = queue_index;
      cur_queue = cur_area->cur+i;
      after_queue = cur_area->cur +i + 1;
      for(; i < cur_area->real_size-1 ; i++){
	cur_queue->entity = after_queue->entity;
	cur_queue->entity->price_area[1] --;
	cur_queue ++;
	after_queue ++;
      }
      /*
      after_queue = cur_area->cur + cur_area->real_size -1;
      after_queue->index = -1;
      after_queue->entity = NULL;
      */
      /*
      cur_queue->entity->price_area[0] = -1;
      cur_queue->entity->price_area[1] = -1;
      cur_queue->index = -1;
      cur_queue->entity = NULL;
      */
    }
    else if(queue_index == cur_area->real_size-1){//last
      cur_queue = cur_area->cur+cur_area->real_size-1;
      cur_queue->entity->price_area[0] = -1;
      cur_queue->entity->price_area[1] = -1;
      cur_queue->index = -1;
      cur_queue->entity = NULL;
    }
    cur_area->real_size --;
  }break;
  case RAISE:{
    cur_area = &my_market->sort_area_raise[area_index];
    if(queue_index != cur_area->real_size-1){//not last
      i = queue_index;
      cur_queue = cur_area->cur+i;
      after_queue = cur_area->cur +i + 1;
      for(; i < cur_area->real_size-1 ; i++){
	cur_queue->entity = after_queue->entity;
	cur_queue->entity->raise_area[1] --;
	cur_queue ++;
	after_queue ++;
      }
      /*
      after_queue = cur_area->cur + cur_area->real_size -1;
      after_queue->index = -1;
      after_queue->entity = NULL;
      */
      /*
      cur_queue->entity->price_area[0] = -1;
      cur_queue->entity->price_area[1] = -1;
      cur_queue->index = -1;
      cur_queue->entity = NULL;
      */
    }
    else if(queue_index == cur_area->real_size-1){//last
      cur_queue = cur_area->cur+cur_area->real_size-1;
      cur_queue->entity->raise_area[0] = -1;
      cur_queue->entity->raise_area[1] = -1;
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
  /*
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
  */
  printf("sort complete...\n");

  return 0;
}

int display_sort(my_market)
     market_t * my_market;
{
  entity_t * entity;
  int i = 0;
  char * template = "code:%s\tprice:%d\n";
  /*
  for(; i<my_market->entity_list_size; i++){
    entity = (entity_t *)(*(my_market->sort_price_list+i));
    printf(template, entity->code, entity->price);
  }
  */
  printf("display sort complete...\n");

  return 0;
}

int sort_get(my_market, column, index, size, entity_list)
     market_t * my_market;
     column_n column;
     int index;
     int size;
     entity_t * entity_list;
{
  sort_area_t * area = NULL;
  int cur_real_size = 0;
  switch(column){
  case NEW_PRICE:
    area = &my_market->sort_area_price;
    break;
  case RAISE:
    area = &my_market->sort_area_raise;
    break;
  default:
    break;
  }
  entity_t * entity_target = &entity_list[0];
  entity_t * entity = NULL;
  int off      = index;
  int off_size = size;
  int i = 0, j = 0;
  int entity_index = 0;
  bool is_finish = false;
  int begin = 1;
  int area_count = 0;

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
    area_count ++;
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
    if(area_count > AREA_NUMBER) return -1;
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
