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

  int min_value;
  int max_value;

  sort_area_queue_t * pre;
  sort_area_queue_t * next;
  sort_area_queue_t * cur;

  int effect_min_value;
  int effect_max_value;

  int allow_size;
  int real_size;
}sort_area_t;


int my_sort(int code_type_index, column_n column);
int init_sort_area(int code_type_index);
#endif
