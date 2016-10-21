#ifndef __CONFIG_H__
#define __CONFIG_H__
#include<zlib.h>
#define SERVER_MARKET_PRE "http://dsapp.yz.zjwtj.com:8010/initinfo/stock/"
//#define SERVER_MARKET "221.6.167.245"
//#define SERVER_MARKET_PORT 8881
//#define SERVER_MARKET "192.168.1.131"
#define SERVER_MARKET "127.0.0.1"
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
#define TYPE_AUTO_PUSH  0x0A03 //
#define TYPE_SERVERINFO 0x0103 //
#define TYPE_DAY_CURPOS 0x020c //

//column of sort
typedef enum
{
  NEW_PRICE,
  ADD_RANGE,
  DOWN_RANGE
}column_n;

char buff[100*1024];

typedef struct
{
  char m_head[4]; 
  int  m_length;  
  unsigned short m_nType;
  char  m_nIndex;   
  char  m_Not;   
  long  m_lKey;
  short m_cCodeType;
  char  m_cCode[6];
  short m_nSize;
  unsigned short m_nOption;
  unsigned short code_type;
  char code[6];
}RealPack;

typedef struct
{
  unsigned short m_nType;
  char m_nIndex;
  char m_not;
  long m_lKey;
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
  unsigned long m_lCurrent;

  unsigned long m_lOutside;//外盘
  unsigned long m_lInside; //内盘
  unsigned long m_lPreClose; //对于外汇，昨收盘数据
  unsigned long m_rate_status; //对于外汇，报价状态
}CommRealTimeData;

//处理股票
typedef struct
{
  long m_lOpen;            //今开盘
  long m_lMaxPrice;        //最高价
  long m_lMinPrice;        //最低价
  long m_lNewPrice;        //最新价
  unsigned long m_lTotal;  //成交量
  float m_fAvgPrice;       //成交金额

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
  long m_lZipLen;
  long m_lOrigLen;
  char m_cData[1];
}TransZipData2;

typedef struct
{
  unsigned short code_type;
  char code[6];
}CodeInfo;

typedef struct
{
  char code[6];
  int  pre_close;  //close price of yestoday
  int price;       //now price
}entity_t;

//from first floor to sixth floor
//every floor one item of node have child node
#define MAX_CHILDS 36 //0-9 A-Z
typedef struct
{
  int floor;
  unsigned int childs[MAX_CHILDS];
}my_key_t;

//first floor
my_key_t key_root = {0};

////4byte, every byte map two byte of code and code_type
//code and code_type map to dictionary
//unsigned int key_map[36] = {0};//0-9 A-Z

typedef struct
{
  char file_name[10];
  char date[8];        //year-month-day
  short code_type;
  short unit;
  char open_close_time[50];
  entity_t * list;
  int entity_list_size;
  int * sort_price_list; //sort by price
}market_t;

market_t market_list[] = {
  //上证a股
  {"1101.txt","20161012",0x1101,1000,"[570-690][780-900][-1--1][-1--1]"},
  //深证a股
  {"1201.txt","20161012",0x1201,1000,"[570-690][780-900][-1--1][-1--1]"}
};
entity_t * entity_list;
int init_market();
int last_time_market;//effective time
int cur_time;        //current time
int heart_times = 0;
int init_socket(int * sock_fd);
void init_receive(void * socket_fd);
int get_content(char * filename, char * buff, int length);
int get_market(cJSON * root_json, int index);
int send_realtime(int socket_fd, int index, int size, int code_type_index);
int send_auto_push(int socket_fd, int index, int size, int code_type_index);
int send_heart(int socket_fd);
int parse(char * buff, uLongf buff_len);
int parse_realtime(char * buff, uLongf buff_len);
int parse_auto_push(char * buff, uLongf buff_len);
int unpack(char * des_buff, uLongf des_buff_len, char ** src_buff, uLongf * src_buff_len);
int my_sort(int code_type_index, int column_index);
int save_key(char * code, unsigned code_len, int code_type_index, entity_t * entity);
int find_entity_by_key(char * code, unsigned int code_len, int code_type_index);
int get_index_by_code_ascii(char ascii);
int out_market(int code_type_index);
int get_quick_image(int code_type_index, int begin, int end); 
int display_sort(int code_type_index);
void sig_stop(int signo);
#endif
