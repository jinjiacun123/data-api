#ifndef __D_HISTORY_H__
#define __D_HISTORY_H__
#include "common.h"
#include "data.h"
/*
	历史
**/
extern void client_request_history(int sclient, t_base_c_request_head * head);
extern void client_parse_history(buff_t * my_buff);

typedef struct
{
	char m_head[4];
	int  m_length; 
	unsigned short	m_nType;
	char		m_nIndex;
	char		m_No;
	long		m_lKey;
	unsigned short	m_cCodeType;
	char		m_cCode[6];
	short  		m_nSize;   
	unsigned short	m_nOption;        
	short		m_nPeriodNum;
	unsigned short	m_nSize2;	
	long		m_lBeginPosition;
	unsigned short	m_nDay;			
	short		m_cPeriod;	
	unsigned short	m_cCodeType2;	   
	char		m_cCode2[6];
}TeachPack;

typedef struct
{
	unsigned short	m_nType;       
	char	m_nIndex; 
	char m_Not;		
	long	m_lKey;	
	short	m_cCodeType;
	char	m_cCode[6];
	long	m_nSize;
	int	m_sdData[1];			
}AnsDayDataEx2;

typedef struct
{
	long	m_lDate;	
	long	m_lOpenPrice;	
	long	m_lMaxPrice;	
	long    m_lMinPrice;		
	long    m_lClosePrice;		
	long    m_lMoney;		
	unsigned long	m_lTotal;		
	long	m_lNationalDebtRatio; 
}StockCompDayDataEx2;

/********作为服务器处理函数********************/
extern int json_to_request_of_history(server_package_t *);
#endif
