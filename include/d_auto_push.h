#ifndef __D_AUTO_PUSH_H__
#define __D_AUTO_PUSH_H__
#include "data.h"
/**
	����
*/
extern void request_auto_push(int sclient);
extern void parse_auto_push();
extern int general_sql_of_auto_push(server_package_t *);
extern int general_json_from_db_auto_push(server_package_t *);

/*
struct AskData2
{
	unsigned short 		m_nType;	     // ��������
	char				m_nIndex;     	 // �������������������ݰ�һ��

	char m_not;

	long				m_lKey;		 	 // һ����ʶ��ͨ��Ϊ���ھ��
	short	m_cCodeType;	// ֤ȯ����
	char				m_cCode[6];		// ֤ȯ����

	short     			m_nSize;         // ����֤ȯ������С����ʱ��

	// �����ֵ���������ֽ���
	unsigned short		m_nOption;       // Ϊ��4�ֽڶ������ӵ��ֶ�

	short	m_cCodeType2;	// ֤ȯ����
	char				m_cCode2[6];		// ֤ȯ����
};
*/

typedef struct
{					
	unsigned short m_nTime;
	unsigned short m_nSecond;

	unsigned long  m_lCurrent;    // ��������

	unsigned long  m_lOutside;    // ����
	unsigned long  m_lInside;     // ����

	unsigned long  m_lPreClose;   // �������ʱ������������		
					

	long		   m_lSortValue;  // ����ʱ��Ϊ������ֵ
}StockOtherData2;

/*
struct CommRealTimeData2
{  	
	short	m_cCodeType;	// ֤ȯ����
	char				m_cCode[6];		// ֤ȯ����


	char	m_othData[24];			// ʵʱ��������
	int			m_cNowData[1];		// ָ��ShareRealTimeData������һ��
};
*/

/*
struct HSQHRealTime2 
{

	long				 m_lOpen;         	// ����
	long				 m_lMaxPrice;     	// ��߼�
	long				 m_lMinPrice;     	// ��ͼ�
	long				 m_lNewPrice;     	// ���¼�

	unsigned long		 m_lTotal;		   	// �ɽ���(��λ:��Լ��λ)
	long				 m_lChiCangLiang;    // �ֲ���(��λ:��Լ��λ)

	long				 m_lBuyPrice1;		// ��һ��
	long				 m_lBuyCount1;		// ��һ��
	long				 m_lSellPrice1;		// ��һ��
	long				 m_lSellCount1;		// ��һ��
	long				 m_lPreJieSuanPrice; // ������ //44

	long				m_lBuyPrice2;			// �����
	unsigned short		m_lBuyCount2;			// �����
	long				m_lBuyPrice3;			// ������
	unsigned short		m_lBuyCount3;			// ������
	long				m_lBuyPrice4;			// ���ļ�
	unsigned short		m_lBuyCount4;			// ������
	long				m_lBuyPrice5;			// �����
	unsigned short		m_lBuyCount5;			// ������

	long				m_lSellPrice2;			// ������
	unsigned short		m_lSellCount2;			// ������
	long				m_lSellPrice3;			// ������
	unsigned short		m_lSellCount3;			// ������
	long				m_lSellPrice4;			// ���ļ�
	unsigned short		m_lSellCount4;			// ������
	long				m_lSellPrice5;			// �����
	unsigned short		m_lSellCount5;			// ������

	long				m_lPreClose1;			// ���� 9*4 +8*2 +8
	long		m_nHand;				// ÿ�ֹ���
	long 		m_lPreCloseChiCang;		// ��ֲ���(��λ:��Լ��λ) 
};
*/

typedef struct 
{
	unsigned short m_nTime;
	unsigned short m_nSecond;
}StockOtherDataDetailTime;

// ����Ʊ��������
typedef struct
{
	union
	{
		unsigned long					 m_nTimeOld;	  // ����ʱ��	
		unsigned short					 m_nTime;		  // ����ʱ��	
		StockOtherDataDetailTime		 m_sDetailTime;
	};

	unsigned long  m_lCurrent;    // ��������

	unsigned long  m_lOutside;    // ����
	unsigned long  m_lInside;     // ����

	union
	{
		unsigned long  m_lKaiCang;    // �񿪲�,�����Ʊ���ʳɽ���,�۹ɽ�������

		unsigned long  m_lPreClose;   // �������ʱ������������		
	};

	union
	{
		unsigned long  m_rate_status; // �������ʱ������״̬
									  // ���ڹ�Ʊ����Ϣ״̬��־,
									  // MAKELONG(MAKEWORD(nStatus1,nStatus2),MAKEWORD(nStatus3,nStatus4))

		unsigned long  m_lPingCang;   // ��ƽ��

		long		   m_lSortValue;  // ����ʱ��Ϊ������ֵ
	};
	
}StockOtherData;
#endif
