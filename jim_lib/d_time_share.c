#include "d_time_share.h"
#include "d_time_share.h"
extern buff_t my_buff;
/**
 分时走势
*/

void request_time_share(int sclient){
	char request[1024];

	int test = sizeof(TrendPack);
	TrendPack data ;
	memset(&data,0x00,sizeof(TrendPack));
	memcpy(data.m_head,HEADER,4);
	data.m_length =  sizeof(TrendPack) - 8;
	data.m_nType = TYPE_TIME_SHARE_EX;//0x0301
	data.m_nSize = 0;
	data.m_nOption	= 0x0080;
	memcpy(data.m_cCode,"EURUSD",6);
	data.m_cCodeType = 0x8100;
	memcpy(data.m_cCode2,"EURUSD",6);
	data.m_cCodeType2 = 0x8100;

	memset(request, 0, 1024);
	memcpy(request, &data, sizeof(data));
	send(sclient, request, sizeof(data), 0);
	printf("分时请求发送完毕\n");
}

void parse_time_share(){
	printf("解析分时数据\n");
	PriceVolItem2* pPriceVolItem = (PriceVolItem2*)my_buff.p_res_media_h + sizeof(AnsTrendData2);
	AnsTrendData2* pHisData222 = (AnsTrendData2 *)my_buff.p_res_media_h;
	for(int i=0;i<pHisData222->m_nHisLen;i++)
	{
		char name[7]={0};
		memcpy(name,pHisData222->m_cCode,6);
		printf("第%d条数据 收到code:%s  new:%ld \n",i+1,name,pPriceVolItem->m_lNewPrice);
		pPriceVolItem ++;
	}
}
