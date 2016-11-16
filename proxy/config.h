#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <errno.h>
//#include <poll.h>
#include <sys/epoll.h>
#include <assert.h>
#include <stdbool.h>
#include <netinet/tcp.h>

#ifndef _SCO_DS
#include <fcntl.h>
#endif
#include <signal.h>
#define LISTENMAX    1024
#define PROXY_SERVER "127.0.0.1"
#define PROXY_PORT   8001
#define SERVER       "221.6.167.245"
//#define SERVER "127.0.0.1"
#define SERVER_PORT  8881
//#define SERVER_PORT 8000
#define USERNAME "jrjvip_android"
#define PASSWORD "zjw_android"
#define HEADER   "ZJHR"

#define MAX_BUFF 1024*1024

#define TYPE_EMPTY 0x0D03
#define TYPE_INIT  0X0101
#define TYPE_LOGIN 0X0102
#define TYPE_HEART 0X0905
#define TYPE_ZIB 0X8001
#define TYPE_REALTIME 0X0201
#define TYPE_HISTORY 0X0402
#define TYPE_TIME_SHARE 0X0301
#define TYPE_AUTO_PUSH 0X0A01
#define TYPE_SERVERINFO 0X0103
#define TYPE_DAY_CURPOS 0X020C

//history circle type
#define PERIOD_TYPE_DAY 0x0010
#define PERIOD_TYPE_SECOND1 0X00E0
#define PERIOD_TYPE_MINUTE1 0x00C0
#define PERIOD_TYPE_MINUTERS 0x0030

//category of market
#define FOREIGN_EXCHANGE    0x00
#define NO_FOREIGN_EXCHANGE 0x01
#define EXPONENT            0x02
#define STOCK               0X03

#pragma pack (4)

typedef struct{
  int socket_fd;
  char *buff;
  int buff_len;
  bool start;
}app_request_data;

//map category and market
typedef struct{
  short market;
  short category;
}map_market_category_t;
map_market_category_t category[] = {
  //foreign_exchange
  {0xffff8100, FOREIGN_EXCHANGE},
  //no_foreign_exchange
  {0x5b00, NO_FOREIGN_EXCHANGE},
  //exponent
  {0x1200, EXPONENT},
  //stock
  {0x1101, STOCK}
};

//app request struct
typedef struct
{
  bool is_create;
  pid_t pid;
  int app_fifo_fd;
  int index;
  int column;
  int begin;
  int size;
}app_request_t;

//entity
typedef struct{
  unsigned short code_type;
  char code[6];
}entity_t;
typedef struct entity_s sort_entity_t;
struct entity_s
{
  unsigned short type;
  char code[7];
  int  pre_close;  //close price of yestoday

  int pre_price;  //prefix value,use to check is sort
  int price;       //now price
  int price_area[2]; //first is area, second is queue

  float add;
  float down;
  float range;
  int max;
  int min;
  int buy;
  int sell;
};

//buff------------------------------------------------------------
//result of back include infomation of package(direct parse not compress package)
struct response_header{
  char str[4];
  int length;
};
typedef struct response_header * p_response_header;

//tuple data
struct response_meta_header{
  unsigned short type;
  char index;
};
typedef struct response_meta_header * p_response_meta_header;

typedef struct{
  char buff[MAX_BUFF];
  unsigned int buff_len;
  unsigned int buff_off;

  unsigned int buff_parse_off;
  char unpack_buff[MAX_BUFF];
  unsigned int unpack_buff_len;

  p_response_header p_res_h;
  p_response_meta_header p_res_media_h;
  unsigned int result_type;

  bool is_direct;
}buff_t;
//buff-------------------------------------------------------------

//response---------------------------------------------------------
#define RESPONSE_DEAL(param) deal_response_of_##param
//处理非外汇
typedef struct{
  long open;        //今开盘
  long max_price;    //最高价
  long min_price;    //最低价
  long new_price;    //最新价

  unsigned long total;  //成交量
  long chi_cang_liang;    //持仓量

  long buy_price_1;   //买一价
  long buy_count_1;   //买一量
  long sell_price_1;  //卖一价
  long sell_count_1;  //卖一量
  long pre_jie_suan_price;  //昨结算价

  long buy_price_2;            //买二价
  unsigned short buy_count_2;  //买二量
  long buy_price_3;            //买三价
  unsigned short buy_count_3;  //买三量
  long buy_price_4;            //买四价
  unsigned short buy_count_4;  //买四量
  long buy_price_5;            //买五价
  unsigned short buy_count_5;  //买五量

  long sell_price_2;           //卖二价
  unsigned short sell_count2; //卖二量
  long sell_price_3;           //卖三价
  unsigned short sell_count_3; //卖三量
  long sell_price_4;           //卖四价
  unsigned short sell_count_4; //卖四量
  long sell_price_5;           //卖五价
  unsigned short sell_count_5; //卖五量

  long pre_close_1;             //昨收
  long head;                  //每手股数
  long pre_close_chi_cang;       //昨持仓量
}response_realtime_price_no_foreign_exchange_t;
//外汇结构体
typedef struct{
  long open;     //今开盘
  long max_price; //最高价
  long min_price; //最低价
  long new_price; //最新价
  long buy_price; //买价
  long sell_price; //卖价
}response_realtime_price_foreign_exchange_t;
//处理指数
typedef struct{
  long open;           //今开盘
  long max_price;       //最高价
  long min_price;       //最低价
  long new_price;       //最新价
  unsigned short total;//成交量
  float avg_price;      //成交金额

  short rise_count;     //上涨家数
  short fall_count;     //下跌家数
  short total_stock_1;

  unsigned long buy_count;  //委买数
  unsigned long sell_count; //委卖数
  short type;               //指数种类:0-综合指数，1-A股，2-B股
  short lead;               //领先指标
  short rise_trend;         //上涨趋势
  short fall_trend;         //下跌趋势
  short no2[5];
  short total_stock_2;

  long adl;    //adl指标
  long no3[3];
  long hand;   //每手股数
}response_realtime_price_exponent_t;
//处理股票
typedef struct{
  long open;             //今开盘
  long max_price;        //最高价
  long min_price;        //最低价
  long new_price;        //最新价
  unsigned long total;   //成交量
  float avg_price;       //成交金额

  long buy_price_1;       //买一价
  long buy_count_1;       //买一量
  long buy_price_2;       //买二价
  long buy_count_2;       //买二量
  long buy_price_3;       //买三价
  long buy_count_3;       //买三量
  long buy_price_4;       //买四价
  long buy_count_4;       //买四量
  long buy_price_5;       //买五价
  long buy_count_5;       //买五量

  long sell_price_1;       //卖一价
  long sell_count_1;       //卖一量
  long sell_price_2;       //卖二价
  long sell_count_2;       //卖二量
  long sell_price_3;       //卖三价
  long sell_count_3;       //卖三量
  long sell_price_4;       //卖四价
  long sell_count_4;       //卖四量
  long sell_price_5;       //卖五价
  long sell_count_5;       //卖五量

  long hand;             //每手股数
  long national_debt_ratio;//国债利率，基金净值

}response_realtime_price_stock_t;

//解析实时数据包
typedef struct{
  short code_type;       //品种类型
  char code[6];         //品种
  unsigned short time;  //开盘到现在分钟数
  unsigned short second;//开盘到先的秒数
  unsigned long current;

  unsigned long out_side;//外盘
  unsigned long in_side; //内盘
  unsigned long pre_close; //对于外汇，昨收盘数据
  unsigned long m_rate_status; //对于外汇，报价状态
}response_realtime_price_t;
typedef struct{
  short code_type;
  char code[6];

  char oth_data[24];
  int now_data[1];
}response_realtime_price_ex_t;

typedef struct{
  //body
  unsigned short type;//data type
  char index;

  char not;
  long key;
  short code_type;
  char code[6];
  short size;
  unsigned short option;
}response_realtime_t;

typedef struct{
  //body
  unsigned short type;//data type
  char index;

  char srv;
  long key;
  short code_type;
  char code[6];
  short his_len;
  short alignment;
  char oth_data[24];
  char otner_data2[112];
  int  his_data[1];
}response_time_share_t;

typedef struct{
  long new_price;
  unsigned long total;
}response_time_share_price_t;

typedef struct{
  long date;           //日期
  long open_price;      //开
  long max_price;       //高
  long min_price;       //低
  long close_price;     //收
  long money;          //成交金额
  unsigned long total; //成交量
  long national_debt_ratio;//国债利率
}response_history_price_t;

typedef struct{
  unsigned short type;
  char index;
  char not:2;
  char operator:6;
  long key;
  unsigned short code_type;
  char code[6];
  long size;
  response_history_price_t data[1];
}response_history_t;

//response of server
typedef struct{
  //header
  char header_name[4];
  int  body_len;
}response_s_t;
typedef response_s_t * p_response_s_t;
//response--------------------------------------------------------

//request---------------------------------------------------------
#define REQUEST_T(param) request_##param##_t
#define REQUEST_DEAL(param) deal_request_of_##param
typedef struct{
  //header
  char header_name[4];
  int body_len;

  //body
  unsigned short type;
}request_s_t;
typedef struct{
  //  request_s_t pre;
  //header
  char header_name[4];
  int body_len;

  //body
  unsigned short type;

  char index;
  char no;
  long key;
  unsigned short code_type;
  char  code[6];
  short size;
  unsigned short option;
  char username[64];
  char password[64];
}request_login_t;

typedef struct{
  //  request_s_t pre;
  //header
  char header_name[4];
  int body_len;

  //body
  unsigned short type;

  char index;
  char operator;
}request_heart_t;

typedef struct{
  //  request_s_t pre;
  //header
  char header_name[4];
  int body_len;

  //body
  unsigned short type;

  char  index;
  char  no;
  long  key;
  unsigned short code_type;
  char  code[6];
  short size;
  unsigned short option;
}request_realtime_t;
typedef struct{
  unsigned short type;
  int size;
}request_c_realtime_t;

typedef struct{
  //  request_s_t pre;
  //header
  char header_name[4];
  int body_len;

  //body
  unsigned short type;

  char index;
  char no;
  long key;
  unsigned short code_type;
  char code[6];
  short size;
  unsigned short option;
  unsigned short code_type2;
  char code2[6];
}request_time_share_t;

typedef struct{
  //  request_s_t pre;
  //header
  char header_name[4];
  int body_len;

  //body
  unsigned short type;

  char index;
  char no;
  int key;
  unsigned short code_type;
  char code[6];
  short  size;   
  unsigned short option;        
  short period_num;
  unsigned short size2;
  int begin_position;
  unsigned short day;
  short period;
  unsigned short code_type2;   
  char code2[6];
}request_history_t;

request_s_t request_data[] = {
  {HEADER, sizeof(REQUEST_T(login))+4, TYPE_LOGIN},//login
  {HEADER, sizeof(REQUEST_T(heart))+4, TYPE_HEART},//heart
  {HEADER,0, TYPE_REALTIME},//realtime
  {HEADER,0, TYPE_AUTO_PUSH},//auto_push
  {HEADER,0, TYPE_TIME_SHARE},//time_share
  {HEADER,0, TYPE_HISTORY},//history

  //empty
  {}
};

//custom sort of request
typedef struct{
  char header_name[4];
  int body_len;

  int index;
  int column;
  int begin;
  int size;
}request_sort_t;
//request-----------------------------------------------------------
#endif
