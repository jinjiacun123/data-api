#include "config.h"
#include "cJSON.h"
#include "sort.h"
#include "market.h"
static int sort_area(int code_type_index, column_n column);
extern market_t market_list[];

//init area and queueof area
//malloc queue and setting area value
int init_sort_area(int code_type_index)
{
  int i = 0;
  market_t * my_market = &market_list[code_type_index];
  sort_area_t * sort_area = NULL;
  sort_area_t * sort_area_zero = NULL;
  int step_len = (my_market->setting_max - my_market->setting_min)/AREA_NUMBER;
  int default_area_queue_len = AREA_QUEUE_DEFAULT_LEN*sizeof(sort_area_queue_t);

  //deal zero
  sort_area_zero = &my_market->sort_area_price_zero;
  sort_area_zero->cur = (sort_area_queue_t *)malloc(AREA_QUEUE_DEFAULT_LEN*sizeof(sort_area_queue_t));
  if(sort_area_zero->cur == NULL){
    printf("malloc err!\n");
    return -1;
  }
  memset(sort_area_zero->cur, 0x00, default_area_queue_len);

  sort_area = &my_market->sort_area_price;
  for(; i< AREA_NUMBER; i++){
    sort_area->cur = (sort_area_queue_t *)malloc(default_area_queue_len);
    if(sort_area->cur == NULL){
      printf("malloc err!\n");
      return -1;
    }
    memset(sort_area->cur, 0x00, default_area_queue_len);
    sort_area->min_value = my_market->setting_min + step_len * i;
    sort_area->max_value = sort_area->min_value+ step_len +1;
    sort_area->allow_size = AREA_QUEUE_DEFAULT_LEN;

    sort_area ++;
  }

  return 0;
}

//from samll to big
int my_sort(int code_type_index,column_n column)
{
  //sort_list(code_type_index, column);
  sort_area(code_type_index, column);

  return 0;
}

static int sort_area(int code_type_index, column_n column)
{
  return 0;
}


static int sort_list(int code_type_index, int column_index)
{
  int i = 0, j = 0;
  int size = market_list[0].entity_list_size;
  market_t * my_market = &market_list[0];
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

int display_sort(int code_type_index)
{
  entity_t * entity;
  int i = 0;
  int index = code_type_index;
  char * template = "code:%s\tprice:%d\n";

  for(; i<market_list[index].entity_list_size; i++){
    entity = (entity_t *)(*(market_list[index].sort_price_list+i));
    printf(template, entity->code, entity->price);
  }
  printf("display sort complete...\n");

  return 0;
}
