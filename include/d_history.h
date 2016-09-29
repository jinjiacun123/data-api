#ifndef __D_HISTORY_H__
#define __D_HISTORY_H__
#include "common.h"
#include "data.h"
/*
	��ʷ
**/
extern void request_history(int sclient);
extern void parse_history();
extern t_base_c_request_head * json_to_request_of_history(char *);
extern int general_sql_of_history(server_package_t *);
extern int general_json_from_db_history(server_package_t *);

typedef struct
{
	char m_head[4];                         // �����"2010"       4���ֽ�   32 30 31 30
	int  m_length;                          // �������ݵĳ��ȣ����ĳ��ȼ�ȥ8��  4���ֽ�     28 00 00 00 
	unsigned short 		m_nType;	        // ��������            2���ֽ�     02 04
	char				m_nIndex;     	    // ��������            1���ֽ�     00
	char				m_No;               // ��ʱ����             1���ֽ�     00
	long				m_lKey;		 	    // һ����ʶ��ͨ��Ϊ���ھ��   4���ֽ�  00 00 00 00
	unsigned short		m_cCodeType;	    // ֤ȯ����                     2���ֽ�  00 81
	char				m_cCode[6];		    // ֤ȯ����                    6���ֽ�  45 55 52 55 53 44
	short     			m_nSize;            // ����֤ȯ����                2���ֽ�  01 00 
	unsigned short		m_nOption;          // Ϊ��4�ֽڶ������ӵ��ֶ�  2���ֽ�  0 00
	short				m_nPeriodNum;		// ���ڳ���                2���ֽ�  01 00
	unsigned short		m_nSize2;			// �������ݵ�ǰ�Ѿ���ȡ������ʼ����   2���ֽ� 00 00
	long				m_lBeginPosition;	// ��ʼ������-1 ��ʾ��ǰλ�á� �����������Ѿ����صĸ����� 4���ֽ�   00 00 00 00
	unsigned short		m_nDay;				// ����ĸ���                       2���ֽ�  0a 00 
	short				m_cPeriod;		    // ��������                             2���ֽ� 10 00
	unsigned short		m_cCodeType2;	    // ֤ȯ����                              2���ֽ�   00 81
	char				m_cCode2[6];		// ֤ȯ����                          6���ֽ�  45 55 52 55 53 44
}TeachPack;

typedef struct
{
	unsigned short		m_nType;        // �������ͣ����������ݰ�һ��
	char				m_nIndex;     	// �������������������ݰ�һ��
	char m_Not;		
	long				m_lKey;		 	// һ����ʶ��ͨ��Ϊ���ھ��
	short	            m_cCodeType;	// ֤ȯ����
	char				m_cCode[6];		// ֤ȯ����
	long				m_nSize;		// �������ݸ���	
	int	m_sdData[1];					// ��������
}AnsDayDataEx2;

typedef struct
{
	long			m_lDate;			  // ����
	long			m_lOpenPrice;		  // ��
	long			m_lMaxPrice;		  // ��
	long	        m_lMinPrice;		  // ��
	long	        m_lClosePrice;		  // ��
	long	        m_lMoney;			  // �ɽ����
	unsigned long	m_lTotal;			  // �ɽ���
	long			m_lNationalDebtRatio; // ��ծ����(��λΪ0.1��),����ֵ(��λΪ0.1��), ������ʱ���뽫����Ϊ0 2004��2��26�ռ���
}StockCompDayDataEx2;
#endif
