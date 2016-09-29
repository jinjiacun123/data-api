#ifndef __D_TIME_SHARE_H__
#define __D_TIME_SHARE_H__
#include "common.h"
#include "data.h"
/**
 分时走势
*/
extern void request_time_share(int sclient);
extern void parse_time_share();
extern t_base_c_request_head * json_to_request_of_time_share(char *);
extern int general_sql_of_time_share(server_package_t *);
extern int general_json_from_db_time_share(server_package_t *);
extern unsigned long format_json_to_client_time_share(server_package_t *);

typedef struct
{

	char m_head[4];           //里面放"2010"    4个字节      32 30 31 30
	int  m_length;           //后面数据的长度（包的长度减去8）4个字节  1c 00 00 00
	unsigned short 		m_nType;	     // 请求类型          2个字节  01 03
	char				m_nIndex;     	 // 请求索引，        1个字节 00
	char				m_No;            //暂时不用           1个字节   00
	long				m_lKey;		 	 // 一级标识，通常为窗口句柄 4个字节    00 00 00 00
	unsigned short		m_cCodeType;	//证券类型                   2个字节    00 81
	char				m_cCode[6];		//证券代码                   6个字节    45 55 52 55 53 44
	short     			m_nSize;         //请求证券总数              2个字节    00 00
	unsigned short		m_nOption;       // 为了4字节对齐而添加的字段 2个字节   80 00
	unsigned short		m_cCodeType2;	//证券类型                    2个字节   00 81
	char				m_cCode2[6];		// 证券代码               6个字节   45 55 52 55 53 44 
}TrendPack;

typedef struct
{
	long			    m_lNewPrice;	// 最新价
	unsigned long		m_lTotal;		// 成交量(在外汇时，是跳动量)
}PriceVolItem2;
struct AnsTrendData2
{
	unsigned short		m_nType;         // 请求类型，与请求数据包一致
	char				m_nIndex;     	 // 请求索引，与请求数据包一致					
	char			    m_cSrv;          // 服务器使用
	long				m_lKey;		 	 // 一级标识，通常为窗口句柄
	short	            m_cCodeType;	// 证券类型
	char				m_cCode[6];		// 证券代码
	short				m_nHisLen;		// 分时数据个数
	short 			    m_nAlignment;	// 为了4字节对齐而添加的字段
	char				m_othData[24];			// 实时其它数据
	char  			  	m_otnerdata2[112];
};
#endif
