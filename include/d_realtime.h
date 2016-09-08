#ifndef __D_REALTIME_H__
#define __D_REALTIME_H__
#include "common.h"
#include "data.h"


/**
	ʵʱ���鴦��
*/
extern void request_realtime(int sclient, t_base_c_request_head *);
extern void parse_realtime(buff_t *);
extern void parse_realtime_pack(buff_t *);
extern t_base_c_request_head * json_to_request_of_realtime(char *);
extern int general_sql_of_realtime(char *, cJSON *, char *, char*);
extern int general_json_from_db_realtime(char *, db_back_t *, cJSON *);

//��Ʒ������
struct RealPack
{
	char m_head[4];           //�����"2010"       4���ֽ�   32 30 31 30
	int  m_length;           //�������ݵĳ��ȣ����ĳ��ȼ�ȥ8��        4���ֽ�   1c 00 00 00
	unsigned short 		m_nType;	//��������     2���ֽ�   01 02
	char				m_nIndex;   // �������������������ݰ�һ�� 1���ֽ� 00
	char				m_Not;     //��ʱ����   1���ֽ�    00
	long				m_lKey;	   // һ����ʶ��ͨ��Ϊ���ھ��    4���ֽ� 00 00 00 00
	short		        m_cCodeType;	//֤ȯ����                2���ֽ� 00 00
	char				m_cCode[6];		// ֤ȯ����               6���ֽ�  00 00 00 00 00 00
	short     			m_nSize;        // ����֤ȯ������С����ʱ,�����ֵ���������ֽ��� 2���ֽ�  01 00
	unsigned short		m_nOption;      //Ϊ��4�ֽڶ������ӵ��ֶ� 2���ֽ�   80 00 
	short		        m_cCodeType2;	//֤ȯ����                  2���ֽ�   00 81
	char				m_cCode2[6];		//֤ȯ����              6���ֽ�   45 55 52 55 53 44 
};
//��չ��Ϊ��Ʒ������ʹ��
typedef struct 
{
	unsigned short		m_cCodeType;	//֤ȯ����     2���ֽ� �����������Ʒ��Ϊ���ڴ�����Ϊ 	        00 57 4f 49 4c 46 00 00 00 57 4f 49 4c 4b 00 00 00 5b 58 41 50 00 00 00
	char				m_cCode[6];		// ֤ȯ���� 6���ֽ� 
}CodeInfo;
typedef struct
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
}AskData;

//const int ASKDATA_HEAD_COUNT = sizeof(AskData) - sizeof(CodeInfo);

//��������ṹͼ
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

struct HSQHRealTime 
{

	//	long				 m_lPreClose; // ������
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

	long				 m_lPreJieSuanPrice; // ������

	union
	{
		struct  
		{
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

//			long				m_lPreClose1;			// ����
		};

		struct
		{
			long		m_lJieSuanPrice;    // �ֽ����
			long		m_lCurrentCLOSE;	// ������
			long		m_lHIS_HIGH;		// ʷ���
			long		m_lHIS_LOW;	 		// ʷ���
			long		m_lUPPER_LIM;		// ��ͣ��
			long		m_lLOWER_LIM;		// ��ͣ��

			long 		m_lLongPositionOpen;	// ��ͷ��(��λ:��Լ��λ)
			long 		m_lLongPositionFlat;	// ��ͷƽ(��λ:��Լ��λ)
			long 		m_lNominalOpen;			// ��ͷ��(��λ:��Լ��λ)	
			long 		m_lNominalFlat;			// ��ͷƽ(��λ:��Լ��λ)
//			long		m_lPreClose1;			// ǰ������????
		};
	};
	long            m_lPreClose1;

	long		m_nHand;				// ÿ�ֹ���
	long 		m_lPreCloseChiCang;		// ��ֲ���(��λ:��Լ��λ)
};
//end add by robert 2015.8.27

struct CommRealTimeData2
{  	
	short	m_cCodeType;	// ֤ȯ����
	char	m_cCode[6];		// ֤ȯ����


	char	m_othData[24];		// ʵʱ��������
	int		m_cNowData[1];		// ָ��ShareRealTimeData������һ��
};

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

//���������ṹ�� codetype=0x8100��0x8200
struct HSWHRealTime 
{
	long		m_lOpen;         	// ����(1/10000Ԫ)
	long		m_lMaxPrice;     	// ��߼�(1/10000Ԫ)
	long		m_lMinPrice;     	// ��ͼ�(1/10000Ԫ)
	long		m_lNewPrice;     	// ���¼�(1/10000Ԫ)
	long		m_lBuyPrice;		// ���(1/10000Ԫ)
	long		m_lSellPrice;		// ����(1/10000Ԫ)
};
#endif
