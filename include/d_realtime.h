#ifndef __D_REALTIME_H__
#define __D_REALTIME_H__
#include "common.h"
#include "data.h"


/**
	实时行情处理
*/
extern void request_realtime(int sclient, t_base_c_request_head *);
extern void parse_realtime(buff_t *);
extern void parse_realtime_pack(buff_t *);
extern t_base_c_request_head * json_to_request_of_realtime(char *);
extern int general_sql_of_realtime(char *, cJSON *, char *, char*);
extern int general_json_from_db_realtime(char *, db_back_t *, cJSON *);

//单品种请求
struct RealPack
{
	char m_head[4];           //里面放"2010"       4个字节   32 30 31 30
	int  m_length;           //后面数据的长度（包的长度减去8）        4个字节   1c 00 00 00
	unsigned short 		m_nType;	//请求类型     2个字节   01 02
	char				m_nIndex;   // 请求索引，与请求数据包一致 1个字节 00
	char				m_Not;     //暂时不用   1个字节    00
	long				m_lKey;	   // 一级标识，通常为窗口句柄    4个字节 00 00 00 00
	short		        m_cCodeType;	//证券类型                2个字节 00 00
	char				m_cCode[6];		// 证券代码               6个字节  00 00 00 00 00 00
	short     			m_nSize;        // 请求证券总数，小于零时,其绝对值代表后面的字节数 2个字节  01 00
	unsigned short		m_nOption;      //为了4字节对齐而添加的字段 2个字节   80 00 
	short		        m_cCodeType2;	//证券类型                  2个字节   00 81
	char				m_cCode2[6];		//证券代码              6个字节   45 55 52 55 53 44 
};
//扩展作为多品种请求使用
typedef struct 
{
	unsigned short		m_cCodeType;	//证券类型     2个字节 以下面的三个品种为例内存数据为 	        00 57 4f 49 4c 46 00 00 00 57 4f 49 4c 4b 00 00 00 5b 58 41 50 00 00 00
	char				m_cCode[6];		// 证券代码 6个字节 
}CodeInfo;
typedef struct
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
}AskData;

//const int ASKDATA_HEAD_COUNT = sizeof(AskData) - sizeof(CodeInfo);

//下面非外汇结构图
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

struct HSQHRealTime 
{

	//	long				 m_lPreClose; // 昨收盘
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

	long				 m_lPreJieSuanPrice; // 昨结算价

	union
	{
		struct  
		{
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

//			long				m_lPreClose1;			// 昨收
		};

		struct
		{
			long		m_lJieSuanPrice;    // 现结算价
			long		m_lCurrentCLOSE;	// 今收盘
			long		m_lHIS_HIGH;		// 史最高
			long		m_lHIS_LOW;	 		// 史最低
			long		m_lUPPER_LIM;		// 涨停板
			long		m_lLOWER_LIM;		// 跌停板

			long 		m_lLongPositionOpen;	// 多头开(单位:合约单位)
			long 		m_lLongPositionFlat;	// 多头平(单位:合约单位)
			long 		m_lNominalOpen;			// 空头开(单位:合约单位)	
			long 		m_lNominalFlat;			// 空头平(单位:合约单位)
//			long		m_lPreClose1;			// 前天收盘????
		};
	};
	long            m_lPreClose1;

	long		m_nHand;				// 每手股数
	long 		m_lPreCloseChiCang;		// 昨持仓量(单位:合约单位)
};
//end add by robert 2015.8.27

struct CommRealTimeData2
{  	
	short	m_cCodeType;	// 证券类型
	char	m_cCode[6];		// 证券代码


	char	m_othData[24];		// 实时其它数据
	int		m_cNowData[1];		// 指向ShareRealTimeData的任意一个
};

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

//下面是外汇结构体 codetype=0x8100和0x8200
struct HSWHRealTime 
{
	long		m_lOpen;         	// 今开盘(1/10000元)
	long		m_lMaxPrice;     	// 最高价(1/10000元)
	long		m_lMinPrice;     	// 最低价(1/10000元)
	long		m_lNewPrice;     	// 最新价(1/10000元)
	long		m_lBuyPrice;		// 买价(1/10000元)
	long		m_lSellPrice;		// 卖价(1/10000元)
};
#endif
