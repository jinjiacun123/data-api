#include "./../include/d_realtime.h"

extern  buff_t my_buff;



/*
	实时行情处理
*/
char	g_cPacketIndex = 10;
#define PACKET_FLAG_NUM	4
//单条请求
void request_realtime(int sclient, t_base_c_request_head * head){
  printf("realtime request\n");
  /*
	char request[1024];

	RealPack data ;
	memset(&data,0x00,sizeof(RealPack));
	memcpy(data.m_head, HEADER, 4);
	data.m_length =  sizeof(RealPack) - 8;
	data.m_nType = TYPE_REALTIME;
	data.m_nSize =1;
	data.m_nOption	= 0x0080;
	memcpy(data.m_cCode2,"EURUSD",6);
	data.m_cCodeType2 = 0x8100;

	memset(request, 0, sizeof(data));
	memcpy(request, &data, sizeof(data));
	int r = send(sclient, request, sizeof(data), 0);  
	printf("r:%d\n", r);
	printf("实时请求\n");
  */
}

//多条请求
void request_realtime_m(int sclient, t_base_c_request_head * head){
  /*
	CodeInfo ary;
	memcpy(ary.m_cCode,"XAGUSD",6);
	ary.m_cCodeType = 0x5a00;
	//单个包
	short lLen2 = ASKDATA_HEAD_COUNT + sizeof(CodeInfo);
	AskData ask2;
	memset(&ask2,0x1,sizeof(AskData));
	ask2.m_nType	= TYPE_REALTIME_EX;  // auto_push
	ask2.m_nSize	= 1;

	ask2.m_nIndex = g_cPacketIndex++;
	char chBuf[1024];
	memcpy(chBuf,(char *)&ask2,ASKDATA_HEAD_COUNT);
	memcpy(chBuf+ASKDATA_HEAD_COUNT,(char *)&ary,sizeof(CodeInfo));

	long nAlloc = ASKDATA_HEAD_COUNT +sizeof(CodeInfo) + PACKET_FLAG_NUM + sizeof(int);
	/*
	CDataBuffer *pBuffer = NULL;
	int leng = ASKDATA_HEAD_COUNT + sizeof(CodeInfo);
	int leng2 =sizeof(AskData);
	pBuffer = new CDataBuffer;
	if (pBuffer == NULL)
	{
		return FALSE;
	}
	pBuffer->Alloc(nAlloc);
	if (!pBuffer->m_pszBuffer)
	{
		return FALSE;
	}
	char *psz2 = pBuffer->m_pszBuffer;
	strncpy(psz2, PACKET_FLAG, PACKET_FLAG_NUM*sizeof(char));
	psz2 += PACKET_FLAG_NUM*sizeof(char);
	*(int*)psz2 = nAlloc;
	psz2 += sizeof(int);
	memcpy(psz2, chBuf, nAlloc);
	*/

  /*
	char request[1024];

	RealPack login ;
	memset(&login,0x00,sizeof(RealPack));
	memcpy(login.m_head, HEADER, 4);
	login.m_length =  sizeof(RealPack) - 8;
	login.m_nType = TYPE_REALTIME;
	login.m_nSize =1;
	login.m_nOption	= 0x0080;
	memcpy(login.m_cCode2,"EURUSD",6);
	login.m_cCodeType2 = 0x8100;

	memset(request, 0, sizeof(login));
	memcpy(request, &login, sizeof(login));
	int r = send(sclient, request, sizeof(login), 0);  
	printf("r:%d\n", r);
*/
}

t_base_c_request_head *
json_to_request_of_realtime(char * json_str){
	t_base_c_request_head  head;

	return &head;
}


void parse_realtime(buff_t * my_buff){
  printf("parse realtime\n");
  /*
	int c =sizeof(long);
	int d= sizeof(unsigned short);
	int a = sizeof(HSQHRealTime2);
	int b =sizeof(HSQHRealTime);
	AskData2 *test = (AskData2 *) (my_buff->buff+8);
	int pre=sizeof(AskData2);
				
	CommRealTimeData2* pRealTime1 ;
		HSQHRealTime2 *pWHRealTime ;
	for (int i =0 ;i<test->m_nSize;i++)
	{
		char name[7]={0};
					
		pRealTime1 = (CommRealTimeData2*)(my_buff->buff 
			                              + 28 
										  +i*(sizeof(CommRealTimeData2)
										  +sizeof(HSQHRealTime2)));
 		if (pRealTime1->m_cCodeType != 0x8100)
 		{	
 					
 			pWHRealTime = (HSQHRealTime2*)(my_buff->buff 
										   + 28 
				                           + sizeof(CommRealTimeData2) 
				                           +i*(sizeof(CommRealTimeData2)
										   +sizeof(HSQHRealTime2)));  //实时或主推
 			memcpy(name,pRealTime1->m_cCode,6);
 			printf("品种%s   最新价%d\n",name,pWHRealTime->m_lNewPrice);
 		}			
					
	}
			
	int bbbbbbbb = sizeof(CommRealTimeData2) - 4 +sizeof(HSWHRealTime);
  */
}

void parse_realtime_pack(buff_t * my_buff){
	
}

int
general_sql_of_realtime(type, json)
     char * type;
     cJSON * json;
{
  int result = 0;
  
  


  return result;
}

int
general_json_from_db_realtime(type, db_back, json)
     char * type;
     db_back_t * db_back;
     cJSON * json;
{
  int result = 0;
  
  return result;
}
