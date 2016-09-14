#ifndef __D_REALTIME_H__
#define __D_REALTIME_H__
#include "common.h"
#include "data.h"


/**
	ʵʱ���鴦��
*/
/*---------�ͻ��˴�����----------------*/
extern void client_request_realtime(int sclient, t_base_c_request_head *);
extern void client_parse_realtime(buff_t *);
extern void client_parse_realtime_pack(buff_t *);

//��Ʒ������
typedef struct
{
	char m_head[4]; 
	int  m_length;  
	unsigned short m_nType;	
	char  m_nIndex;   
	char  m_Not;   
	long  m_lKey;
	short m_cCodeType;
	char  m_cCode[6];
	short m_nSize;
	unsigned short m_nOption; 
}RealPack;

typedef struct{
  unsigned short m_cCodeType2;
  char m_cCode2[6];
}CodeInfo;

//����ʵʱ���ݰ�
typedef struct {
  short m_cCodeType;       //Ʒ������
  char m_cCode[6];         //Ʒ��
  unsigned short m_nTime;  //���̵����ڷ�����
  unsigned short m_nSecond;//���̵��ȵ�����
  unsigned long m_lCurrent;

  unsigned long m_lOutside;//����
  unsigned long m_lInside; //����
  unsigned long m_lPreClose; //������㣬����������
  unsigned long m_rate_status; //������㣬����״̬
}CommRealTimeData;

typedef struct{
  unsigned short m_nType;
  char m_nIndex;
  char m_not;
  long m_lKey;
  short m_cCodeType;
  char m_cCode[6];
  short m_nSize;
  unsigned short m_nOption;
  short m_cCodeType2;
  char m_cCode2[6];
}AskData2;

typedef struct{
  short m_cCodeType;
  char m_cCode[6];
  
  char m_othData[24];
  int m_cNowData[1];

}CommRealTimeData2;

//��������
typedef struct{
  long m_lOpen;        //����
  long m_lMaxPrice;    //��߼�
  long m_lMinPrice;    //��ͼ�
  long m_lNewPrice;    //���¼�
  
  unsigned long m_lTotal;  //�ɽ���
  long m_lChiCangLiang;    //�ֲ���
  
  long m_lBuyPrice1;   //��һ��
  long m_lBuyCount1;   //��һ��
  long m_lSellPrice1;  //��һ��
  long m_lSellCount1;  //��һ��
  long m_lPreJieSuanPrice;  //������
  
  long m_lBuyPrice2;            //�����
  unsigned short m_lBuyCount2;  //�����
  long m_lBuyPrice3;            //������
  unsigned short m_lBuyCount3;  //������
  long m_lBuyPrice4;            //���ļ�
  unsigned short m_lBuyCount4;  //������
  long m_lBuyPrice5;            //�����
  unsigned short m_lBuyCount5;  //������

  long m_lSellPrice2;           //������
  unsigned short m_lSellCount2; //������
  long m_lSellPrice3;           //������
  unsigned short m_lSellCount3; //������
  long m_lSellPrice4;           //���ļ�
  unsigned short m_lSellCount4; //������
  long m_lSellPrice5;           //�����
  unsigned short m_lSellCount5; //������

  long m_lPreClose1;             //����
  long m_nHead;                  //ÿ�ֹ���
  long m_lPreCloseChiCang;       //��ֲ��� 

}HSQHRealTime2;

//���ṹ��
typedef struct{
  long m_lOpen;     //����
  long m_lMaxPrice; //��߼�
  long m_lMinPrice; //��ͼ�
  long m_lNewPrice; //���¼�
  long m_lBuyPrice; //���
  long m_lSellPrice; //����
}HSWHRealTime;

//����ָ��
typedef struct{
  long m_lOpen;           //����
  long m_lMaxPrice;       //��߼�
  long m_lMinPrice;       //��ͼ�
  long m_lNewPrice;       //���¼�
  unsigned short m_lTotal;//�ɽ��� 
  float m_fAvgPrice;      //�ɽ����
  
  short m_nRiseCount;     //���Ǽ���
  short m_nFallCount;     //�µ�����
  short m_nTotalStock1;   

  unsigned long m_lBuyCount;  //ί����
  unsigned long m_lSellCount; //ί����
  short m_nType;              //ָ������:0-�ۺ�ָ����1-A�ɣ�2-B��
  short m_nLead;              //����ָ��
  short m_nRiseTrend;         //��������
  short m_nFallTrend;         //�µ�����
  short m_nNo2[5];            
  short m_nTotalStock2;             

  long m_lADL;    //adlָ��
  long m_lNo3[3]; 
  long m_nHand;   //ÿ�ֹ���
  
}HSIndexRealTime;

//�����Ʊ
typedef struct{
  long m_lOpen;            //����
  long m_lMaxPrice;        //��߼�
  long m_lMinPrice;        //��ͼ�
  long m_lNewPrice;        //���¼�
  unsigned long m_lTotal;  //�ɽ���
  float m_fAvgPrice;       //�ɽ����

  long m_lBuyPrice1;       //��һ��
  long m_lBuyCount1;       //��һ��
  long m_lBuyPrice2;       //�����
  long m_lBuyCount2;       //�����
  long m_lBuyPrice3;       //������
  long m_lBuyCount3;       //������
  long m_lBuyPrice4;       //���ļ�
  long m_lBuyCount4;       //������
  long m_lBuyPrice5;       //�����
  long m_lBuyCount5;       //������

  long m_lSellPrice1;       //��һ��
  long m_lSellCount1;       //��һ��
  long m_lSellPrice2;       //������
  long m_lSellCount2;       //������
  long m_lSellPrice3;       //������
  long m_lSellCount3;       //������
  long m_lSellPrice4;       //���ļ�
  long m_lSellCount4;       //������
  long m_lSellPrice5;       //�����
  long m_lSellCount5;       //������

  long m_nHand;             //ÿ�ֹ���
  long m_lNationalDebtRatio;//��ծ���ʣ�����ֵ

}HSStockRealTime;

/*----------������������--------------------------*/
extern t_base_c_request_head * json_to_request_of_realtime(char *);
extern int general_sql_of_realtime(server_package_t *);
extern int general_json_from_db_realtime(server_package_t *);
#endif
