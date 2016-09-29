#ifndef __D_TIME_SHARE_H__
#define __D_TIME_SHARE_H__
#include "common.h"
#include "data.h"
/**
 ��ʱ����
*/
extern void request_time_share(int sclient);
extern void parse_time_share();
extern t_base_c_request_head * json_to_request_of_time_share(char *);
extern int general_sql_of_time_share(server_package_t *);
extern int general_json_from_db_time_share(server_package_t *);
extern unsigned long format_json_to_client_time_share(server_package_t *);

typedef struct
{

	char m_head[4];           //�����"2010"    4���ֽ�      32 30 31 30
	int  m_length;           //�������ݵĳ��ȣ����ĳ��ȼ�ȥ8��4���ֽ�  1c 00 00 00
	unsigned short 		m_nType;	     // ��������          2���ֽ�  01 03
	char				m_nIndex;     	 // ����������        1���ֽ� 00
	char				m_No;            //��ʱ����           1���ֽ�   00
	long				m_lKey;		 	 // һ����ʶ��ͨ��Ϊ���ھ�� 4���ֽ�    00 00 00 00
	unsigned short		m_cCodeType;	//֤ȯ����                   2���ֽ�    00 81
	char				m_cCode[6];		//֤ȯ����                   6���ֽ�    45 55 52 55 53 44
	short     			m_nSize;         //����֤ȯ����              2���ֽ�    00 00
	unsigned short		m_nOption;       // Ϊ��4�ֽڶ������ӵ��ֶ� 2���ֽ�   80 00
	unsigned short		m_cCodeType2;	//֤ȯ����                    2���ֽ�   00 81
	char				m_cCode2[6];		// ֤ȯ����               6���ֽ�   45 55 52 55 53 44 
}TrendPack;

typedef struct
{
	long			    m_lNewPrice;	// ���¼�
	unsigned long		m_lTotal;		// �ɽ���(�����ʱ����������)
}PriceVolItem2;
struct AnsTrendData2
{
	unsigned short		m_nType;         // �������ͣ����������ݰ�һ��
	char				m_nIndex;     	 // �������������������ݰ�һ��					
	char			    m_cSrv;          // ������ʹ��
	long				m_lKey;		 	 // һ����ʶ��ͨ��Ϊ���ھ��
	short	            m_cCodeType;	// ֤ȯ����
	char				m_cCode[6];		// ֤ȯ����
	short				m_nHisLen;		// ��ʱ���ݸ���
	short 			    m_nAlignment;	// Ϊ��4�ֽڶ������ӵ��ֶ�
	char				m_othData[24];			// ʵʱ��������
	char  			  	m_otnerdata2[112];
};
#endif
