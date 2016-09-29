#ifndef __D_HISTORY_H__
#define __D_HISTORY_H__
#include "common.h"
#include "data.h"
/*
	历史
**/
extern void request_history(int sclient);
extern void parse_history();
extern t_base_c_request_head * json_to_request_of_history(char *);
extern int general_sql_of_history(server_package_t *);
extern int general_json_from_db_history(server_package_t *);

typedef struct
{
	char m_head[4];                         // 里面放"2010"       4个字节   32 30 31 30
	int  m_length;                          // 后面数据的长度（包的长度减去8）  4个字节     28 00 00 00 
	unsigned short 		m_nType;	        // 请求类型            2个字节     02 04
	char				m_nIndex;     	    // 请求索引            1个字节     00
	char				m_No;               // 暂时不用             1个字节     00
	long				m_lKey;		 	    // 一级标识，通常为窗口句柄   4个字节  00 00 00 00
	unsigned short		m_cCodeType;	    // 证券类型                     2个字节  00 81
	char				m_cCode[6];		    // 证券代码                    6个字节  45 55 52 55 53 44
	short     			m_nSize;            // 请求证券总数                2个字节  01 00 
	unsigned short		m_nOption;          // 为了4字节对齐而添加的字段  2个字节  0 00
	short				m_nPeriodNum;		// 周期长度                2个字节  01 00
	unsigned short		m_nSize2;			// 本地数据当前已经读取数据起始个数   2个字节 00 00
	long				m_lBeginPosition;	// 起始个数，-1 表示当前位置。 （服务器端已经返回的个数） 4个字节   00 00 00 00
	unsigned short		m_nDay;				// 申请的个数                       2个字节  0a 00 
	short				m_cPeriod;		    // 周期类型                             2个字节 10 00
	unsigned short		m_cCodeType2;	    // 证券类型                              2个字节   00 81
	char				m_cCode2[6];		// 证券代码                          6个字节  45 55 52 55 53 44
}TeachPack;

typedef struct
{
	unsigned short		m_nType;        // 请求类型，与请求数据包一致
	char				m_nIndex;     	// 请求索引，与请求数据包一致
	char m_Not;		
	long				m_lKey;		 	// 一级标识，通常为窗口句柄
	short	            m_cCodeType;	// 证券类型
	char				m_cCode[6];		// 证券代码
	long				m_nSize;		// 日线数据个数	
	int	m_sdData[1];					// 日线数据
}AnsDayDataEx2;

typedef struct
{
	long			m_lDate;			  // 日期
	long			m_lOpenPrice;		  // 开
	long			m_lMaxPrice;		  // 高
	long	        m_lMinPrice;		  // 低
	long	        m_lClosePrice;		  // 收
	long	        m_lMoney;			  // 成交金额
	unsigned long	m_lTotal;			  // 成交量
	long			m_lNationalDebtRatio; // 国债利率(单位为0.1分),基金净值(单位为0.1分), 无意义时，须将其设为0 2004年2月26日加入
}StockCompDayDataEx2;
#endif
