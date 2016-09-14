#ifndef __D_REALTIME_H__
#define __D_REALTIME_H__
#include "common.h"
#include "data.h"


/**
	实时行情处理
*/
/*---------客户端处理函数----------------*/
extern void client_request_realtime(int sclient, t_base_c_request_head *);
extern void client_parse_realtime(buff_t *);
extern void client_parse_realtime_pack(buff_t *);

//单品种请求
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
}RealPack;

typedef struct{
  unsigned short m_cCodeType2;
  char m_cCode2[6];
}CodeInfo;

//解析实时数据包
typedef struct {
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

typedef struct{
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

typedef struct{
  short m_cCodeType;
  char m_cCode[6];
  
  char m_othData[24];
  int m_cNowData[1];

}CommRealTimeData2;

//处理非外汇
typedef struct{
  long m_lOpen;        //今开盘
  long m_lMaxPrice;    //最高价
  long m_lMinPrice;    //最低价
  long m_lNewPrice;    //最新价
  
  unsigned long m_lTotal;  //成交量
  long m_lChiCangLiang;    //持仓量
  
  long m_lBuyPrice1;   //买一价
  long m_lBuyCount1;   //买一量
  long m_lSellPrice1;  //卖一价
  long m_lSellCount1;  //卖一量
  long m_lPreJieSuanPrice;  //昨结算价
  
  long m_lBuyPrice2;            //买二价
  unsigned short m_lBuyCount2;  //买二量
  long m_lBuyPrice3;            //买三价
  unsigned short m_lBuyCount3;  //买三量
  long m_lBuyPrice4;            //买四价
  unsigned short m_lBuyCount4;  //买四量
  long m_lBuyPrice5;            //买五价
  unsigned short m_lBuyCount5;  //买五量

  long m_lSellPrice2;           //卖二价
  unsigned short m_lSellCount2; //卖二量
  long m_lSellPrice3;           //卖三价
  unsigned short m_lSellCount3; //卖三量
  long m_lSellPrice4;           //卖四价
  unsigned short m_lSellCount4; //卖四量
  long m_lSellPrice5;           //卖五价
  unsigned short m_lSellCount5; //卖五量

  long m_lPreClose1;             //昨收
  long m_nHead;                  //每手股数
  long m_lPreCloseChiCang;       //昨持仓量 

}HSQHRealTime2;

//外汇结构体
typedef struct{
  long m_lOpen;     //今开盘
  long m_lMaxPrice; //最高价
  long m_lMinPrice; //最低价
  long m_lNewPrice; //最新价
  long m_lBuyPrice; //买价
  long m_lSellPrice; //卖价
}HSWHRealTime;

//处理指数
typedef struct{
  long m_lOpen;           //今开盘
  long m_lMaxPrice;       //最高价
  long m_lMinPrice;       //最低价
  long m_lNewPrice;       //最新价
  unsigned short m_lTotal;//成交量 
  float m_fAvgPrice;      //成交金额
  
  short m_nRiseCount;     //上涨家数
  short m_nFallCount;     //下跌家数
  short m_nTotalStock1;   

  unsigned long m_lBuyCount;  //委买数
  unsigned long m_lSellCount; //委卖数
  short m_nType;              //指数种类:0-综合指数，1-A股，2-B股
  short m_nLead;              //领先指标
  short m_nRiseTrend;         //上涨趋势
  short m_nFallTrend;         //下跌趋势
  short m_nNo2[5];            
  short m_nTotalStock2;             

  long m_lADL;    //adl指标
  long m_lNo3[3]; 
  long m_nHand;   //每手股数
  
}HSIndexRealTime;

//处理股票
typedef struct{
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

/*----------服务器处理函数--------------------------*/
extern t_base_c_request_head * json_to_request_of_realtime(char *);
extern int general_sql_of_realtime(server_package_t *);
extern int general_json_from_db_realtime(server_package_t *);
#endif
