#ifndef __SORT_H__
#define __SORT_H__

//queue of area
struct sort_area_queue_s
{
  int index;
  entity_t * entity;
};

//from min to max to N area
typedef struct
{
  int area_index;

  value_t min_value;
  value_t max_value;

  sort_area_queue_t * pre;
  sort_area_queue_t * next;
  sort_area_queue_t * cur;

  value_t effect_min_value;
  value_t effect_max_value;

  int allow_size;
  int real_size;
}sort_area_t;

int init_sort_area(market_t * my_market);
int sort_add(market_t * my_market, entity_t * entity, column_n column);
int sort_update(market_t * my_market, entity_t * entity, column_n column);
int my_sort(market_t * my_market, column_n column);
int display_sort(market_t * my_market);
int sort_get(market_t * my_market, int index, int size, entity_t * entity_list);
#endif
