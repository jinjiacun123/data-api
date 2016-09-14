#ifndef __D_HEART_H__
#define __D_HEART_H__
#include "common.h"
#include "data.h"
/**
	ÐÄÌø
*/
extern void client_request_heart(int sclient, t_base_c_request_head *);
extern void client_parse_heart(buff_t *);
extern t_base_c_request_head * json_to_request_of_heart(char *);

typedef struct
{
		char head[4];           //   32 30 31 30
		int  length;           //04 00 00 00
		unsigned short m_nType;				//  05 09
		char		   m_nIndex;     		// 00
		char		   m_cOperator;   		// 00
}TestSrvData2;
#endif
