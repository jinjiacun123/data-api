#ifndef __D_TIME_SHARE_H__
#define __D_TIME_SHARE_H__
#include "data.h"
/**
 分时走势
*/
extern void client_request_time_share(int, t_base_c_request_head *);
extern void client_parse_time_share(buff_t *);

typedef struct
{

	char m_head[4];
	int  m_length; 
	unsigned short 	m_nType;
	char	m_nIndex;
	char	m_No;
	long	m_lKey;
	unsigned short	m_cCodeType;
	char	m_cCode[6];
	short    m_nSize;
	unsigned short	m_nOption;
	unsigned short	m_cCodeType2;
	char	m_cCode2[6];
}TrendPack;

typedef struct
{
	long	 m_lNewPrice;
	unsigned long	m_lTotal;
}PriceVolItem2;

typedef struct
{
  unsigned short	m_nType;
  char	m_nIndex;
  char	m_cSrv;
  long	m_lKey;
  short	m_cCodeType;
  char	m_cCode[6];
  short	m_nHisLen;
  short	m_nAlignment;
  char	m_othData[24];
  char 	m_otnerdata2[112];
  int   m_pHisData[1];
}AnsTrendData2;

/*********服务器处理函数**************/
extern int
json_to_request_of_time_share(server_package_t *);
#endif
