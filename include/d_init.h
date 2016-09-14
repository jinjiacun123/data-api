#ifndef __INIT_H__
#define __INIT_H__
#include "common.h"
#include "data.h"

extern void client_request_init(int sclient, t_base_c_request_head * head);
extern void client_parse_init(buff_t *);
extern void client_parse_init_pack(buff_t *);
extern t_base_c_request_head * json_to_request_of_init(char *);

//��ʼ������
typedef struct 
{
	char				head[4];		   // �����"2010"		4���ֽ�		32 30 31 30
	int 				length;			   // �������ĳ���		4���ֽ�		24 00 00 00 
	unsigned short 		m_nType;	       // �������� 		    2���ֽ�		01 01
	char				m_nIndex;     	   // �������������������ݰ�һ�� 1���ֽ�         00  		
	char			    m_cNot;			   // ��ʱ����                  1���ֽ�         00
	long				m_lKey;		 	   // һ����ʶ��ͨ��Ϊ���ھ��  4���ֽ�           00 00 00 00
	short				m_cCodeType;	   // ֤ȯ����  				   2���ֽ�	         00 00
	char				m_cCode[6];		   // ֤ȯ����                  6���ֽ�           00 00 00 00 00 00
	short     			m_nSize;           // ����֤ȯ������С����ʱ,�����ֵ���������ֽ��� 2���ֽ� 00 00
	unsigned short		m_nOption;         // Ϊ��4�ֽڶ������ӵ��ֶ�  2���ֽ�           00 00
	short	            m_cCodeType2;	   // ֤ȯ����       			   2���ֽ�           00 00
	char				m_cCode2[6];       // ֤ȯ����                   6���ֽ�           00 00 00 00 00 00	
}InitPack;

//����Ϊ������ʼ��
typedef struct 
{
	short	m_nOpenTime;	// ǰ����ʱ��
	short	m_nCloseTime;	// ǰ����ʱ��
}HSTypeTime;

//���г���Ϣ
typedef struct  
{
	unsigned short		m_nType;         // �������ͣ����������ݰ�һ��
	char				m_nIndex;     	 // �������������������ݰ�һ��
	char				m_cSrv;          // ������ʹ��														
	long				m_lKey;		 	 // һ����ʶ��ͨ��Ϊ���ھ��
	short				m_cCodeType;	// ֤ȯ����
	char				m_cCode[6];		// ֤ȯ����
	short		   	    m_nSize;		// �г�����	
	short 				m_nAlignment;   // �Ƿ�Ϊ���Ƴ�ʼ����(0�������ʼ��������0�����Ƴ�ʼ����)
}AnsInitialData2;

/* �����г���������Ϣ */
typedef struct
{
	//struct CommBourseInfo2 begin
	char m_szName[20];				// ��Ʊ��������
	short			m_nMarketType;	// �г����(�����λ)
	short			m_cCount;		// ��Ч��֤ȯ���͸���
	long			m_lDate;		// �������ڣ�19971230��
	unsigned int	m_dwCRC;		// CRCУ���루���ࣩ
		
	//CommBourseInfo2���struct StockType begin      �˽ṹ������m_cCount��CommBourseInfo2�ṹ�壬Ϊ�˲��ýṹ�廥��Ǳ����˲𿪳�������ʽ������ʱ��Ҫע��
	char m_szName2[20];			// ��Ʊ��������
	short   m_nStockType;		// ֤ȯ����
	short   m_nTotal;			// ֤ȯ����
	short   m_nOffset;			// ƫ����
	short   m_nPriceUnit;		// �۸�λ
	short   m_nTotalTime;		// �ܿ���ʱ�䣨���ӣ�
	short   m_nCurTime;			// ����ʱ�䣨���ӣ�
	HSTypeTime		 m_nNewTimes[12];
		
	short           m_nSize;         // ��Ʊ������
	short 		    m_nAlignment;    // Ϊ��4�ֽڶ������ӵ��ֶ�

	char		m_cStockName[16];	// ��Ʊ����
	short	m_cCodeType;	// ֤ȯ����
	char				m_cCode[6];		// ֤ȯ����
	long		m_lPrevClose;		// ����
	long		m_l5DayVol;			// 5����(�Ƿ���ڴ˼���ɽ���λ����������
}OneMarketData2;
	
//�������������汻��ֵĽṹ�壬Ϊ�˷����ȡ�����ڴ˷�װ
struct CommBourseInfo2
{
	char m_szName[20];				// ��Ʊ��������
	short			m_nMarketType;	// �г����(�����λ)
	short			m_cCount;		// ��Ч��֤ȯ���͸���
	long			m_lDate;		// �������ڣ�19971230��
	unsigned int	m_dwCRC;		// CRCУ���루���ࣩ
	char m_szName2[20];          	// ��Ʊ��������
	short   m_nStockType;			// ֤ȯ����
	short   m_nTotal;				// ֤ȯ����
	short   m_nOffset;				// ƫ����
	short   m_nPriceUnit;			// �۸�λ
	short   m_nTotalTime;			// �ܿ���ʱ�䣨���ӣ�
	short   m_nCurTime;				// ����ʱ�䣨���ӣ�
	HSTypeTime		 m_nNewTimes[12];
};

struct StockType2
{
	char m_szName[20];			// ��Ʊ��������
	short   m_nStockType;		// ֤ȯ����
	short   m_nTotal;			// ֤ȯ����
	short   m_nOffset;			// ƫ����
	short   m_nPriceUnit;		// �۸�λ
	short   m_nTotalTime;		// �ܿ���ʱ�䣨���ӣ�
	short   m_nCurTime;			// ����ʱ�䣨���ӣ�
	HSTypeTime		 m_nNewTimes[12];
};

struct StockInitInfo2
{
	char		m_cStockName[16];		// ��Ʊ����
	short	m_cCodeType;				// ֤ȯ����
	char				m_cCode[6];		// ֤ȯ����
	long		m_lPrevClose;			// ����
	long		m_l5DayVol;				// 5����
};
#endif
