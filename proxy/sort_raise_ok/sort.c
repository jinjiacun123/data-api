#include "config.h"
#include "sort.h"
#include "market.h"
#include <assert.h>
#include <stdbool.h>
#include <math.h>

static int sort_area(market_t * my_market, column_n column);
static int find_location(market_t * my_market, entity_t * entity, column_n column, int * area_index, int * queue_index);
static int remove_entity(market_t * my_market, entity_t * entity, column_n column);
extern market_t market_list[];

//init area and queueof area
//malloc queue and setting area value
int init_sort_area(my_market)
     market_t * my_market;
{
  int i = 0;
  sort_area_t * price_sort_area = NULL;
  sort_area_t * raise_sort_area = NULL;
  sort_area_t * range_sort_area = NULL;
  int price_step_len = 0;
  int raise_step_len = 0;
  int range_step_len = 0.0;
  //char tmp_range[6] = {0};
  price_step_len = (my_market->setting_max - my_market->setting_min)/AREA_NUMBER;
  raise_step_len = (my_market->yestoday_max *0.1*2)/AREA_NUMBER;
  //range_step_len = ceilf(0.4 * 10000/AREA_NUMBER)/10000;
  //snprintf(tmp_range, "%.4f", (float)(0.4/AREA_NUMBER), 6);
  //range_step_len = (float)atoll(tmp_range);
  range_step_len = 4000/AREA_NUMBER;
  //range_step_len = ( (float)( (int)( (range_step_len+0.005)*100 ) ) )/100;
  //range_step_len = ceilf(37.777779*100)/100;
  int raise_min = (-1)*(my_market->yestoday_max * 0.1);
  float range_min = -2000;
  int price_area_queue_len = 0;
  int raise_area_queue_len = 0;
  int range_area_queue_len = 0;
  int default_area_queue_len = AREA_NUMBER*AREA_QUEUE_DEFAULT_LEN*sizeof(sort_area_queue_t);
  int price_page_size = 0;
  int raise_page_size = 0;
  int range_page_size = 0;
  price_page_size = default_area_queue_len;
  raise_page_size = default_area_queue_len;
  range_page_size = default_area_queue_len;
  //price_page_size = ceil(1.0*default_area_queue_len/PAGE_SIZE);
  //raise_page_size = ceil(1.0*default_area_queue_len/PAGE_SIZE);
  //price_area_queue_len = price_page_size * PAGE_SIZE;
  //raise_area_queue_len = raise_page_size * PAGE_SIZE;
  price_area_queue_len = price_page_size;
  raise_area_queue_len = raise_page_size;
  range_area_queue_len = range_page_size;
  sort_area_queue_t * price_base_queue = (char *)malloc(price_area_queue_len);
  sort_area_queue_t * raise_base_queue = (char *)malloc(raise_area_queue_len);
  sort_area_queue_t * range_base_queue = (char *)malloc(range_area_queue_len);
  if(price_base_queue == NULL){
    DEBUG("error:[%s]", "malloc error!");
    return -1;
  }
  memset(price_base_queue, 0x00, price_area_queue_len);
  if(raise_base_queue == NULL){
    DEBUG("error:[%s]", "malloc error!");
    return -1;
  }
  memset(raise_base_queue, 0x00, raise_area_queue_len);
  if(range_base_queue == NULL){
    DEBUG("error:[%s]", "malloc error!");
    return -1;
  }
  memset(range_base_queue, 0x00, range_area_queue_len);

  price_sort_area = &my_market->sort_area_price;
  raise_sort_area = &my_market->sort_area_raise;
  range_sort_area = &my_market->sort_area_range;
  for(; i< AREA_NUMBER; i++){
    price_sort_area->cur = price_base_queue + AREA_QUEUE_DEFAULT_LEN * i;
    raise_sort_area->cur = raise_base_queue + AREA_QUEUE_DEFAULT_LEN * i;
    range_sort_area->cur = range_base_queue + AREA_QUEUE_DEFAULT_LEN * i;

    price_sort_area->min_value.ivalue = my_market->setting_min + price_step_len * i;
    price_sort_area->max_value.ivalue = price_sort_area->min_value.ivalue + price_step_len +1;
    raise_sort_area->min_value.ivalue = raise_min + raise_step_len * i;
    raise_sort_area->max_value.ivalue = raise_sort_area->min_value.ivalue + raise_step_len + 1;
    range_sort_area->min_value.ivalue = range_min + range_step_len * i;
    range_sort_area->max_value.ivalue = range_sort_area->min_value.ivalue + range_step_len + 1;

    price_sort_area->real_size  = 0;
    raise_sort_area->real_size  = 0;
    range_sort_area->real_size  = 0;
    price_sort_area->allow_size = AREA_QUEUE_DEFAULT_LEN;
    raise_sort_area->allow_size = AREA_QUEUE_DEFAULT_LEN;
    range_sort_area->allow_size = AREA_QUEUE_DEFAULT_LEN;

    price_sort_area ++;
    raise_sort_area ++;
    range_sort_area ++;
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
  sort_area_t * cur_area = NULL;
  sort_area_queue_t * cur_queue = NULL, * pre_queue = NULL;
  int ret = -1;

  //find entity from area
  ret = find_location(my_market, entity, column, &area_index, &queue_index);
  assert(ret == 0);
  switch(column){
  case NEW_PRICE:{
    entity->price_area[0] = area_index;
    entity->price_area[1] = queue_index;
  }break;
  case RAISE:{
    entity->raise_area[0] = area_index;
    entity->raise_area[1] = queue_index;
  }break;
  case RANGE:{
    entity->range_area[0] = area_index;
    entity->range_area[1] = queue_index;
  }break;
  default:{

  }
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
  case RANGE:{
    cur_area = &my_market->sort_area_range[area_index];
    if(queue_index == 0){//first
      if(cur_area->real_size == 0){
	cur_queue = cur_area->cur;
	cur_queue->index = 0;
	cur_queue->entity = entity;
	cur_queue->entity->range_area[1] = cur_queue->index;
      }else{
	i = cur_area->real_size;
	pre_queue = cur_area->cur + i -1;
	cur_queue = cur_area->cur + i;
	for(; i>queue_index; i--){
	  cur_queue->index = pre_queue->index+1;
	  cur_queue->entity = pre_queue->entity;
	  cur_queue->entity->range_area[1] = cur_queue->index;
	  cur_queue --;
	  pre_queue --;
	}
	cur_queue = cur_area->cur + queue_index;
	cur_queue->entity = entity;
	cur_queue->index = queue_index;
	cur_queue->entity->range_area[1] = cur_queue->index;
      }
    }else if(queue_index == cur_area->real_size){//last
      cur_queue = cur_area->cur + queue_index;
      cur_queue->entity = entity;
      cur_queue->index = queue_index;
      cur_queue->entity->range_area[1] = queue_index;
    }else if(queue_index < cur_area->real_size){//in btween real_size
      //move
      i = cur_area->real_size;
      pre_queue = cur_area->cur + i -1;
      cur_queue = cur_area->cur + i;
      for(; i>queue_index; i--){
	cur_queue->index = pre_queue->index+1;
	cur_queue->entity = pre_queue->entity;
	cur_queue->entity->range_area[1] = cur_queue->index;
	cur_queue --;
	pre_queue --;
      }
      cur_queue = cur_area->cur + queue_index;
      cur_queue->entity = entity;
      cur_queue->index = queue_index;
      cur_queue->entity->range_area[1] = cur_queue->index;
    }else{//not enough and remalloc
      cur_queue = cur_area->cur + queue_index;
      cur_queue->index = queue_index;
      cur_queue->entity = entity;
      cur_queue->entity->range_area[1] = cur_queue->index;
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
  int tmp_ivalue = 0;
  switch(column){
  case NEW_PRICE:
    area = &my_market->sort_area_price;
    break;
  case RAISE:
    area = &my_market->sort_area_raise;
    break;
  case RANGE:
    area = &my_market->sort_area_range;
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
  case RANGE:{
    value.ivalue = entity->range;
  }break;
  default:{

  }break;
  }

  pre_area = area;
#define FIND_WAY 1

#if FIND_WAY
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
#else
  
#endif
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
	case NEW_PRICE:{
	  tmp_ivalue = item->price;
	  if(value.ivalue > tmp_ivalue){
	    queue++;
	    continue;
	  }else{
	    *queue_index = i;
	    return 0;
	  }
	}break;
	case RAISE:{
	  tmp_ivalue = item->raise;
	  if(value.ivalue > tmp_ivalue){
	    queue++;
	    continue;
	  }else{
	    *queue_index = i;
	    return 0;
	  }
	}break;
	case RANGE:{
	  tmp_ivalue = item->range;
	  if(value.ivalue > tmp_ivalue){
	    queue++;
	    continue;
	  }else{
	    *queue_index = i;
	    return 0;
	  }
	}break;
	default:
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
	DEBUG("error:[%s]", "malloc err!");
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
  case RANGE:{
    area_index  = entity->range_area[0];
    queue_index = entity->range_area[1];
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
  case RANGE:{
    cur_area = &my_market->sort_area_range[area_index];
    if(queue_index != cur_area->real_size-1){//not last
      i = queue_index;
      cur_queue = cur_area->cur+i;
      after_queue = cur_area->cur +i + 1;
      for(; i < cur_area->real_size-1 ; i++){
	cur_queue->entity = after_queue->entity;
	cur_queue->entity->range_area[1] --;
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
      cur_queue->entity->range_area[0] = -1;
      cur_queue->entity->range_area[1] = -1;
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
  case RANGE:
    area = &my_market->sort_area_range;
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
    DEBUG("error:[%s]", "malloc error!");
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

  return 0;
}
