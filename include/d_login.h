#ifndef __LOGIN_H__
#define __LOGIN_H__

#include "common.h"
#include "data.h"

extern void request_login(int sclient, t_base_c_request_head * head);
extern void parse_login(buff_t * my_buff);
extern t_base_c_request_head * json_to_request_of_login(char * json_str);

/**
	login struct
*/
typedef struct
{
	char head[4];           
	int  length;
	unsigned short m_nType;
	char m_nIndex;
	char m_No;
	long m_lKey;
	short m_cCodeType;
	char  m_cCode[6];
	short m_nSize;
	unsigned short m_nOption;
	char m_szUser[64];
	char m_szPWD[64];
}t_login;
#endif
