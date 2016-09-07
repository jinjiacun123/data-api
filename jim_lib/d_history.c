#include "d_history.h"
/*
	历史
**/

extern buff_t my_buff;

void request_history(int sclient){
	char request[1024];

	TeachPack data;
	memset(&data,0x00,sizeof(TeachPack));
	memcpy(data.m_head, HEADER,4);
	data.m_length =  sizeof(TeachPack) - 8;
	data.m_nType = TYPE_HISTORY_EX;
	data.m_nSize =1;
	data.m_nIndex	= 0;
	data.m_nOption	= 0x0080;
	memcpy(data.m_cCode,"EURUSD",6);
	data.m_cCodeType = 0x8100;

	data.m_cPeriod =0x0010;  //请求类型
	data.m_lBeginPosition =0;
	data.m_nDay=10;  //请求个数
	data.m_nPeriodNum =1;
	data.m_nSize2 =0;
	memcpy(data.m_cCode2,"EURUSD",6);
	data.m_cCodeType2 = 0x8100;


	memset(request, 0, sizeof(data));
	memcpy(request, &data, sizeof(data));
	int r = send(sclient, request, sizeof(data), 0);  
	printf("r:%d\n", r);
	printf("历史请求\n");
}

void parse_history(){
	printf("解析历史\n");	
	AnsDayDataEx2 * g = (AnsDayDataEx2* )my_buff.p_res_media_h;//历史数据对上了这个结构体
	StockCompDayDataEx2 *stockData = (StockCompDayDataEx2*)g->m_sdData;
	int lengthaa = sizeof(AnsDayDataEx2)-4+sizeof(StockCompDayDataEx2)*g->m_nSize;
	printf("数据包有效长度%d",lengthaa);
				
	for (int i =0;i<g->m_nSize;i++)
	{
		char name[7]={0};
		memcpy(name,g->m_cCode,6);
					
		printf("收到第%d条历史报价 code:%s  date：%ld    max:%ld  min:%ld\n",
			   i+1,
			   name,
			   stockData->m_lDate,
			   stockData->m_lOpenPrice,
			   stockData->m_lMaxPrice,
			   stockData->m_lMinPrice);
		stockData++;
	}
}
