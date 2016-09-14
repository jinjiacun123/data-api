#ifndef __D_AUTO_PUSH_H__
#define __D_AUTO_PUSH_H__
#include "data.h"
/**
	����
*/
extern void client_request_auto_push(int sclient, t_base_c_request_head *);
extern void client_parse_auto_push();

struct StockOtherData2
{					
	unsigned short m_nTime;
	unsigned short m_nSecond;
	unsigned long  m_lCurrent;    // ��������
	unsigned long  m_lOutside;    // ����
	unsigned long  m_lInside;     // ����
	unsigned long  m_lPreClose;   // �������ʱ������������		  
	long	 m_lSortValue;  // ����ʱ��Ϊ������ֵ
};

typedef struct 
{
	unsigned short m_nTime;
	unsigned short m_nSecond;
}StockOtherDataDetailTime;


/**********������������************************/
extern int json_to_request_of_auto_push(server_package_t *);
#endif
