#ifndef __API_H__
#define __API_H_

//type of data
#define TYPE_EMPTY      0x0D03
#define TYPE_HEART      0X0905
#define TYPE_ZIB        0X8001
#define TYPE_REALTIME   0X0201
#define TYPE_HISTORY    0X0402
#define TYPE_TIME_SHARE 0X0301
#define TYPE_AUTO_PUSH  0X0A01

//circly of history
#define PERIOD_TYPE_DAY      0x0010
#define PERIOD_TYPE_SECOND1  0x00E0
#define PERIOD_TYPE_MINUTE1  0X00C0
#define PERIOD_TYPE_MINUTERS 0X0030

typedef struct{
  short code_type;
  char code[6];
}entity_t;

typedef struct{
entity_t entity;
long new_price;
}entity_info_t;

//request
int request(const unsigned short type,
	      const entity_t entity[], 
	      const int size, 
	      char * package);

int response(const char * package, const int length,
	       unsigned short * type,
	       entity_info_t * entity_info,
	       int size);
	       

//response realtime
//response auto_push
//response history
//response time_share
//response zlib

#endif
