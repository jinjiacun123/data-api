#include "d_auto_push.h"
#include "d_realtime.h"
/**
	主推
*/
void request_auto_push(int sclient){
	char request[1024];

	RealPack data;
	memset(&data,0x00,sizeof(RealPack));
	memcpy(data.m_head,HEADER,4);
	data.m_length =  sizeof(RealPack) - 8;
	data.m_nType = TYPE_AUTO_PUSH_EX;
	data.m_nSize =1;
	data.m_nOption	= 0x0080;
	memcpy(data.m_cCode2,"EURUSD",6);
	data.m_cCodeType2 = 0x8100;

	memset(request, 0, sizeof(data));
	memcpy(request, &data, sizeof(data));
	int r = send(sclient, request, sizeof(data), 0);  
	printf("r:%d\n", r);
	printf("主推请求\n");
}

void parse_auto_push(buff_t * my_buff)  
{
	printf("主推\n");
	AskData2 *test = (AskData2 *) my_buff->p_res_media_h;
	int pre=sizeof(AskData2);
				
	CommRealTimeData2* pRealTime1 ;
	HSQHRealTime2 *pWHRealTime ;
				
	int a = sizeof(StockOtherData);
	int b= sizeof(StockOtherData);
	StockOtherData2 * PushTime;
	int i=0;
	for (i =0 ;i<test->m_nSize;i++)
	{
		char name[7]={0};
					
		pRealTime1 = (CommRealTimeData2*)(my_buff->p_res_media_h + 20 +i*(sizeof(CommRealTimeData2)-4+sizeof(HSQHRealTime2)));
		pWHRealTime = (HSQHRealTime2*)pRealTime1->m_cNowData;  //实时或主推
		memcpy(name,pRealTime1->m_cCode,6);
		PushTime = (StockOtherData2*)pRealTime1->m_othData;
			
		printf("助推报价:  品种%s   最新价%d\n",name,pWHRealTime->m_lNewPrice);
	}
			

	int bbbbbbbb = sizeof(CommRealTimeData2) - 4 +sizeof(HSQHRealTime2);
}

int
general_sql_of_auto_push(package)
     server_package_t * package;
{
  return 0;
}

int
general_json_from_db_auto_push(package)
     server_package_t * package;
{
  return 0;
}
