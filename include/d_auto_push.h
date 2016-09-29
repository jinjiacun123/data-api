#ifndef __D_AUTO_PUSH_H__
#define __D_AUTO_PUSH_H__
#include "data.h"
/**
	主推
*/
extern void request_auto_push(int sclient);
extern void parse_auto_push();
extern int general_sql_of_auto_push(server_package_t *);
extern int general_json_from_db_auto_push(server_package_t *);

/*
struct AskData2
{
	unsigned short 		m_nType;	     // 请求类型
	char				m_nIndex;     	 // 请求索引，与请求数据包一致

	char m_not;

	long				m_lKey;		 	 // 一级标识，通常为窗口句柄
	short	m_cCodeType;	// 证券类型
	char				m_cCode[6];		// 证券代码

	short     			m_nSize;         // 请求证券总数，小于零时，

	// 其绝对值代表后面的字节数
	unsigned short		m_nOption;       // 为了4字节对齐而添加的字段

	short	m_cCodeType2;	// 证券类型
	char				m_cCode2[6];		// 证券代码
};
*/

typedef struct
{					
	unsigned short m_nTime;
	unsigned short m_nSecond;

	unsigned long  m_lCurrent;    // 现在总手

	unsigned long  m_lOutside;    // 外盘
	unsigned long  m_lInside;     // 内盘

	unsigned long  m_lPreClose;   // 对于外汇时，昨收盘数据		
					

	long		   m_lSortValue;  // 排名时，为排序后的值
}StockOtherData2;

/*
struct CommRealTimeData2
{  	
	short	m_cCodeType;	// 证券类型
	char				m_cCode[6];		// 证券代码


	char	m_othData[24];			// 实时其它数据
	int			m_cNowData[1];		// 指向ShareRealTimeData的任意一个
};
*/

/*
struct HSQHRealTime2 
{

	long				 m_lOpen;         	// 今开盘
	long				 m_lMaxPrice;     	// 最高价
	long				 m_lMinPrice;     	// 最低价
	long				 m_lNewPrice;     	// 最新价

	unsigned long		 m_lTotal;		   	// 成交量(单位:合约单位)
	long				 m_lChiCangLiang;    // 持仓量(单位:合约单位)

	long				 m_lBuyPrice1;		// 买一价
	long				 m_lBuyCount1;		// 买一量
	long				 m_lSellPrice1;		// 卖一价
	long				 m_lSellCount1;		// 卖一量
	long				 m_lPreJieSuanPrice; // 昨结算价 //44

	long				m_lBuyPrice2;			// 买二价
	unsigned short		m_lBuyCount2;			// 买二量
	long				m_lBuyPrice3;			// 买三价
	unsigned short		m_lBuyCount3;			// 买三量
	long				m_lBuyPrice4;			// 买四价
	unsigned short		m_lBuyCount4;			// 买四量
	long				m_lBuyPrice5;			// 买五价
	unsigned short		m_lBuyCount5;			// 买五量

	long				m_lSellPrice2;			// 卖二价
	unsigned short		m_lSellCount2;			// 卖二量
	long				m_lSellPrice3;			// 卖三价
	unsigned short		m_lSellCount3;			// 卖三量
	long				m_lSellPrice4;			// 卖四价
	unsigned short		m_lSellCount4;			// 卖四量
	long				m_lSellPrice5;			// 卖五价
	unsigned short		m_lSellCount5;			// 卖五量

	long				m_lPreClose1;			// 昨收 9*4 +8*2 +8
	long		m_nHand;				// 每手股数
	long 		m_lPreCloseChiCang;		// 昨持仓量(单位:合约单位) 
};
*/

typedef struct 
{
	unsigned short m_nTime;
	unsigned short m_nSecond;
}StockOtherDataDetailTime;

// 各股票其他数据
typedef struct
{
	union
	{
		unsigned long					 m_nTimeOld;	  // 现在时间	
		unsigned short					 m_nTime;		  // 现在时间	
		StockOtherDataDetailTime		 m_sDetailTime;
	};

	unsigned long  m_lCurrent;    // 现在总手

	unsigned long  m_lOutside;    // 外盘
	unsigned long  m_lInside;     // 内盘

	union
	{
		unsigned long  m_lKaiCang;    // 今开仓,深交所股票单笔成交数,港股交易宗数

		unsigned long  m_lPreClose;   // 对于外汇时，昨收盘数据		
	};

	union
	{
		unsigned long  m_rate_status; // 对于外汇时，报价状态
									  // 对于股票，信息状态标志,
									  // MAKELONG(MAKEWORD(nStatus1,nStatus2),MAKEWORD(nStatus3,nStatus4))

		unsigned long  m_lPingCang;   // 今平仓

		long		   m_lSortValue;  // 排名时，为排序后的值
	};
	
}StockOtherData;
#endif
