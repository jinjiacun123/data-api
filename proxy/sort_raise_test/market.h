#ifndef __MARKET_H__
#define __MARKET_H__
#include "config.h"
#include "sort.h"
#include "cJSON.h"
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

struct entity_s
{
  unsigned short type;
  char code[7];
  int  pre_close;  //close price of yestoday

  int pre_price;  //prefix value,use to check is sort
  int price;       //now price
  int price_area[2]; //first is area, second is queue

  int raise;
  int raise_area[2];
  float down;
  float range;
  int max;
  int min;
  int total;
  int money;
};

//from first floor to sixth floor
//every floor one item of node have child node
#define MAX_CHILDS 36 //0-9 A-Z
typedef struct
{
  int floor;
  void * childs[MAX_CHILDS];
}my_key_t;
//first floor

////4byte, every byte map two byte of code and code_type
//code and code_type map to dictionary
//unsigned int key_map[36] = {0};//0-9 A-Z

struct market_s
{
  char file_name[10];
  char date[8];        //year-month-day
  short code_type;
  short unit;
  char open_close_time[50];

  entity_t * list;
  int entity_list_size;

  int yestoday_max;
  int yestoday_min;

  int setting_max;
  int setting_min;

  /*--sort--*/
  sort_area_t sort_area_price_zero;
  sort_area_t sort_area_price[AREA_NUMBER];
  sort_area_t sort_area_raise[AREA_NUMBER];

  int max_price;
  int min_price;

  int max_raise;
  int min_raise;
};

int init_market();
int get_market(int index, FILE * fp, int size);
int out_market(int code_type_index);
#endif
