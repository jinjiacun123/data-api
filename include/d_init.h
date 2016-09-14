#ifndef __INIT_H__
#define __INIT_H__
#include "common.h"
#include "data.h"

extern void client_request_init(int sclient, t_base_c_request_head * head);
extern void client_parse_init(buff_t *);
extern void client_parse_init_pack(buff_t *);
extern t_base_c_request_head * json_to_request_of_init(char *);

//初始化请求
typedef struct 
{
	char				head[4];		   // 里面放"2010"		4个字节		32 30 31 30
	int 				length;			   // 整个包的长度		4个字节		24 00 00 00 
	unsigned short 		m_nType;	       // 请求类型 		    2个字节		01 01
	char				m_nIndex;     	   // 请求索引，与请求数据包一致 1个字节         00  		
	char			    m_cNot;			   // 暂时不用                  1个字节         00
	long				m_lKey;		 	   // 一级标识，通常为窗口句柄  4个字节           00 00 00 00
	short				m_cCodeType;	   // 证券类型  				   2个字节	         00 00
	char				m_cCode[6];		   // 证券代码                  6个字节           00 00 00 00 00 00
	short     			m_nSize;           // 请求证券总数，小于零时,其绝对值代表后面的字节数 2个字节 00 00
	unsigned short		m_nOption;         // 为了4字节对齐而添加的字段  2个字节           00 00
	short	            m_cCodeType2;	   // 证券类型       			   2个字节           00 00
	char				m_cCode2[6];       // 证券代码                   6个字节           00 00 00 00 00 00	
}InitPack;

//下面为解析初始化
typedef struct 
{
	short	m_nOpenTime;	// 前开市时间
	short	m_nCloseTime;	// 前闭市时间
}HSTypeTime;

//总市场信息
typedef struct  
{
	unsigned short		m_nType;         // 请求类型，与请求数据包一致
	char				m_nIndex;     	 // 请求索引，与请求数据包一致
	char				m_cSrv;          // 服务器使用														
	long				m_lKey;		 	 // 一级标识，通常为窗口句柄
	short				m_cCodeType;	// 证券类型
	char				m_cCode[6];		// 证券代码
	short		   	    m_nSize;		// 市场个数	
	short 				m_nAlignment;   // 是否为主推初始化包(0：请求初始化包，非0：主推初始化包)
}AnsInitialData2;

/* 单个市场包含的信息 */
typedef struct
{
	//struct CommBourseInfo2 begin
	char m_szName[20];				// 股票分类名称
	short			m_nMarketType;	// 市场类别(最高俩位)
	short			m_cCount;		// 有效的证券类型个数
	long			m_lDate;		// 今日日期（19971230）
	unsigned int	m_dwCRC;		// CRC校验码（分类）
		
	//CommBourseInfo2里的struct StockType begin      此结构体中有m_cCount个CommBourseInfo2结构体，为了不让结构体互相潜逃因此拆开成这种样式，解析时需要注意
	char m_szName2[20];			// 股票分类名称
	short   m_nStockType;		// 证券类型
	short   m_nTotal;			// 证券总数
	short   m_nOffset;			// 偏移量
	short   m_nPriceUnit;		// 价格单位
	short   m_nTotalTime;		// 总开市时间（分钟）
	short   m_nCurTime;			// 现在时间（分钟）
	HSTypeTime		 m_nNewTimes[12];
		
	short           m_nSize;         // 股票个数据
	short 		    m_nAlignment;    // 为了4字节对齐而添加的字段

	char		m_cStockName[16];	// 股票名称
	short	m_cCodeType;	// 证券类型
	char				m_cCode[6];		// 证券代码
	long		m_lPrevClose;		// 昨收
	long		m_l5DayVol;			// 5日量(是否可在此加入成交单位？？？？）
}OneMarketData2;
	
//下面两个是上面被拆分的结构体，为了方便截取数据在此封装
struct CommBourseInfo2
{
	char m_szName[20];				// 股票分类名称
	short			m_nMarketType;	// 市场类别(最高俩位)
	short			m_cCount;		// 有效的证券类型个数
	long			m_lDate;		// 今日日期（19971230）
	unsigned int	m_dwCRC;		// CRC校验码（分类）
	char m_szName2[20];          	// 股票分类名称
	short   m_nStockType;			// 证券类型
	short   m_nTotal;				// 证券总数
	short   m_nOffset;				// 偏移量
	short   m_nPriceUnit;			// 价格单位
	short   m_nTotalTime;			// 总开市时间（分钟）
	short   m_nCurTime;				// 现在时间（分钟）
	HSTypeTime		 m_nNewTimes[12];
};

struct StockType2
{
	char m_szName[20];			// 股票分类名称
	short   m_nStockType;		// 证券类型
	short   m_nTotal;			// 证券总数
	short   m_nOffset;			// 偏移量
	short   m_nPriceUnit;		// 价格单位
	short   m_nTotalTime;		// 总开市时间（分钟）
	short   m_nCurTime;			// 现在时间（分钟）
	HSTypeTime		 m_nNewTimes[12];
};

struct StockInitInfo2
{
	char		m_cStockName[16];		// 股票名称
	short	m_cCodeType;				// 证券类型
	char				m_cCode[6];		// 证券代码
	long		m_lPrevClose;			// 昨收
	long		m_l5DayVol;				// 5日量
};
#endif
