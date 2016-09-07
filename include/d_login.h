#ifndef __LOGIN_H__
#define __LOGIN_H__

#include "common.h"
#include "data.h"

extern void request_login(int sclient, t_base_c_request_head * head);
extern void parse_login(buff_t * my_buff);
extern t_base_c_request_head * json_to_request_of_login(char * json_str);

/**
	µÇÂŒÇëÇóœá¹¹Ìå
*/
typedef struct
{
	char head[4];           //ÀïÃæ·Å"2010"			4žö×ÖœÚ  32 30 31 30
	int  length;           //ºóÃæÊýŸÝµÄ³€¶È£š°üµÄ³€¶ÈŒõÈ¥8£©			4žö×ÖœÚ    94 00 00 00
	unsigned short 		m_nType;	     //ÇëÇóÀàÐÍ						2žö×ÖœÚ   02 01
	char				m_nIndex;     	 //ÇëÇóË÷Òý£¬ÓëÇëÇóÊýŸÝ°üÒ»ÖÂ   1žö×ÖœÚ  00
	char				m_No;            //ÔÝÊ±²»ÓÃ 					1žö×ÖœÚ 00 
	long				m_lKey;		 	 //Ò»Œ¶±êÊ¶  					4žö×ÖœÚ 03 00 00 00 
	short				m_cCodeType;	 //Ö€È¯ÀàÐÍ 					2žö×ÖœÚ 00  00 
	char				m_cCode[6];		 //Ö€È¯ŽúÂë                     6žö×ÖœÚ 00 00 00 00 00 00
	short     			m_nSize;         //ÇëÇóÖ€È¯×ÜÊý                 2žö×ÖœÚ 00 00 
	unsigned short		m_nOption;       //ÎªÁË4×ÖœÚ¶ÔÆë¶øÌíŒÓµÄ×Ö¶Î    2žö×ÖœÚ  00 00
	char			m_szUser[64];	     //ÓÃ»§Ãû     64žö×ÖœÚ  
	char			m_szPWD[64];	     //ÃÜÂë            64žö×ÖœÚ
}t_login;
#endif
