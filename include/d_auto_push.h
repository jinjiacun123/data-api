#ifndef __D_AUTO_PUSH_H__
#define __D_AUTO_PUSH_H__
#include "data.h"
/**
	主推
*/
extern void client_request_auto_push(int sclient, t_base_c_request_head *);
extern void client_parse_auto_push();

struct StockOtherData2
{					
	unsigned short m_nTime;
	unsigned short m_nSecond;
	unsigned long  m_lCurrent;    // 现在总手
	unsigned long  m_lOutside;    // 外盘
	unsigned long  m_lInside;     // 内盘
	unsigned long  m_lPreClose;   // 对于外汇时，昨收盘数据		  
	long	 m_lSortValue;  // 排名时，为排序后的值
};

typedef struct 
{
	unsigned short m_nTime;
	unsigned short m_nSecond;
}StockOtherDataDetailTime;


/**********服务器处理函数************************/
extern int json_to_request_of_auto_push(server_package_t *);
#endif
