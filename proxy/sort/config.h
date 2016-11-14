#ifndef __CONFIG_H__
#define __CONFIG_H__
#include<zlib.h>
#include<stdbool.h>
#define SERVER_MARKET_PRE "http://dsapp.yz.zjwtj.com:8010/initinfo/stock/"
//#define SERVER_MARKET "221.6.167.245"
//#define SERVER_MARKET_PORT 8881
#define SERVER_MARKET "192.168.1.131"
//#define SERVER_MARKET "127.0.0.1"
#define SERVER_MARKET_PORT 8001

#define USERNAME "jrjvip_android"
#define PASSWORD "zjw_android"
#define HEADER   "ZJHR"

/*number of type as hexadicimal*/
#define TYPE_EMPTY      0x0D03 //empty
#define TYPE_INIT       0x0101 //init
#define TYPE_LOGIN      0X0102 //login
#define TYPE_HEART      0x0905 //heart tick
#define TYPE_ZIB        0x8001 //
#define TYPE_REALTIME   0x0201 //
#define TYPE_HISTORY    0x0402 //
#define TYPE_TIME_SHARE 0x0301 //
#define TYPE_AUTO_PUSH  0x0A01 //
#define TYPE_SERVERINFO 0x0103 //
#define TYPE_DAY_CURPOS 0x020c //

#define PAGE_SIZE 4096
#define AREA_NUMBER 50
#define AREA_QUEUE_DEFAULT_LEN 100
#define SORT_SHOW_MAX_NUM 10

#define system_32
//#define system_64
//#define PIPE_NAME "./../jim_sort"
//#define PRIVATE_PIPE_TEMPLATE "./../child_pipe/sort_%d"
#define APP_SIZE  10

//basic type
/*
#ifdef system_64
#define long int
#else
#define long long
#endif
*/

typedef enum option_s option_n;
typedef enum column_s column_n;
typedef struct sort_area_queue_s sort_area_queue_t;
typedef struct market_s market_t;
typedef struct entity_s entity_t;

typedef union
{
  int ivalue;
  float fvalue;
}value_t;

//column of sort
enum column_s
{
  NEW_PRICE,
  UP_RANGE,
  DOWN_RANGE
};

//option type
enum option_s
{
  ADD,
  UPDATE
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

typedef struct
{
  char m_head[4];
  int  m_length;
  unsigned short m_nType;
  char  m_nIndex;
  char  m_Not;
  #ifdef system_32
  long  m_lKey;
  #else
  int  m_lKey;
  #endif
  short m_cCodeType;
  char  m_cCode[6];
  short m_nSize;
  unsigned short m_nOption;
  unsigned short code_type;
  char code[6];
}RealPack;

typedef struct
{
  char m_head[4];
  int  m_length;
  unsigned short m_nType;
  char  m_nIndex;
  char  m_Not;
  #ifdef system_32
  long  m_lKey;
  #else
  int m_lKey;
  #endif
  short m_cCodeType;
  char  m_cCode[6];
  short m_nSize;
  unsigned short m_nOption;
}RealPack_ex;

typedef struct
{
  unsigned short m_nType;
  char m_nIndex;
  char m_not;
  #ifdef system_32
  long m_lKey;
  #else
  int m_lKey;
  #endif
  short m_cCodeType;
  char m_cCode[6];
  short m_nSize;
  unsigned short m_nOption;
  short m_cCodeType2;
  char m_cCode2[6];
}AskData2;

typedef struct
{
  short m_cCodeType;
  char m_cCode[6];

  char m_othData[24];
  int m_cNowData[1];
}CommRealTimeData2;

typedef struct
{
  short m_cCodeType;       //品种类型
  char m_cCode[6];         //品种
  unsigned short m_nTime;  //开盘到现在分钟数
  unsigned short m_nSecond;//开盘到先的秒数
  #ifdef system_32
  unsigned long m_lCurrent;

  unsigned long m_lOutside;//外盘
  unsigned long m_lInside; //内盘
  unsigned long m_lPreClose; //对于外汇，昨收盘数据
  unsigned long m_rate_status; //对于外汇，报价状态
  #else
  unsigned int m_lCurrent;

  unsigned int m_lOutside;//外盘
  unsigned int m_lInside; //内盘
  unsigned int m_lPreClose; //对于外汇，昨收盘数据
  unsigned int m_rate_status; //对于外汇，报价状态
  #endif
}CommRealTimeData;

//处理股票
typedef struct
{
  #ifdef system_32
  long m_lOpen;            //今开盘
  long m_lMaxPrice;        //最高价
  long m_lMinPrice;        //最低价
  long m_lNewPrice;        //最新价
  unsigned long m_lTotal;  //成交量
  #else
  int m_lOpen;            //今开盘
  int m_lMaxPrice;        //最高价
  int m_lMinPrice;        //最低价
  int m_lNewPrice;        //最新价
  unsigned int m_lTotal;  //成交量
  #endif
  float m_fAvgPrice;       //成交金额

  #ifdef system_32
  long m_lBuyPrice1;       //买一价
  long m_lBuyCount1;       //买一量
  long m_lBuyPrice2;       //买二价
  long m_lBuyCount2;       //买二量
  long m_lBuyPrice3;       //买三价
  long m_lBuyCount3;       //买三量
  long m_lBuyPrice4;       //买四价
  long m_lBuyCount4;       //买四量
  long m_lBuyPrice5;       //买五价
  long m_lBuyCount5;       //买五量

  long m_lSellPrice1;       //卖一价
  long m_lSellCount1;       //卖一量
  long m_lSellPrice2;       //卖二价
  long m_lSellCount2;       //卖二量
  long m_lSellPrice3;       //卖三价
  long m_lSellCount3;       //卖三量
  long m_lSellPrice4;       //卖四价
  long m_lSellCount4;       //卖四量
  long m_lSellPrice5;       //卖五价
  long m_lSellCount5;       //卖五量

  long m_nHand;             //每手股数
  long m_lNationalDebtRatio;//国债利率，基金净值
  #else
  int m_lBuyPrice1;       //买一价
  int m_lBuyCount1;       //买一量
  int m_lBuyPrice2;       //买二价
  int m_lBuyCount2;       //买二量
  int m_lBuyPrice3;       //买三价
  int m_lBuyCount3;       //买三量
  int m_lBuyPrice4;       //买四价
  int m_lBuyCount4;       //买四量
  int m_lBuyPrice5;       //买五价
  int m_lBuyCount5;       //买五量

  int m_lSellPrice1;       //卖一价
  int m_lSellCount1;       //卖一量
  int m_lSellPrice2;       //卖二价
  int m_lSellCount2;       //卖二量
  int m_lSellPrice3;       //卖三价
  int m_lSellCount3;       //卖三量
  int m_lSellPrice4;       //卖四价
  int m_lSellCount4;       //卖四量
  int m_lSellPrice5;       //卖五价
  int m_lSellCount5;       //卖五量

  int m_nHand;             //每手股数
  int m_lNationalDebtRatio;//国债利率，基金净值
  #endif
}HSStockRealTime;

typedef struct
{
  char head[4];
  int  length;
  unsigned short m_nType;
  char   m_nIndex;
  char   m_cOperator;
}TestSrvData2;

typedef struct
{
  unsigned short m_nType;
  short m_nAlignment;
  #ifdef system_32
  long m_lZipLen;
  long m_lOrigLen;
  #else
  int m_lZipLen;
  int m_lOrigLen;
  #endif
  char m_cData[1];
}TransZipData2;

typedef struct
{
  unsigned short code_type;
  char code[6];
}CodeInfo;

//entity_t * entity_list;

int init_socket(int * sock_fd);
void init_receive(void * socket_fd);
void reset_socket(int * sock_fd);
void init_pipe(void * param);
void init_sort_display(void * param);
void init_app(void *param);
void write_app(void * param);
int get_content(char * filename, char * buff, int length);
int send_realtime(int socket_fd, int index, int size, int code_type_index);
int send_auto_push(int socket_fd, int index, int size, int code_type_index);
int send_heart(int socket_fd);
int parse(char * buff, uLongf buff_len);
int parse_realtime(char * buff, uLongf buff_len);
int parse_auto_push(char * buff, uLongf buff_len);
int unpack(char * des_buff, uLongf des_buff_len);
int save_key(char * code, unsigned code_len, int code_type_index, entity_t * entity);
int find_entity_by_key(char * code, unsigned int code_len, int code_type_index);
int get_index_by_code_ascii(char ascii);
int get_quick_image(int code_type_index, int begin, int end); 
int display_sort(market_t * my_market);
void sig_stop(int signo);
void test();
#endif
