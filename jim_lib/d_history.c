#include "./../include/d_history.h"
#include "./../include/d_realtime.h"
#include "./../include/d_time_share.h"
/*
	历史
**/
static int
insert_into(unsigned short,char * ,long, long, long,long, long, long, long);
extern CodeInfo my_request_code_info;
   
void 
client_request_history(sclient, head)
  int sclient;
t_base_c_request_head * head;
{
	char request[1024];

	TeachPack data;
	memset(&data,0x00,sizeof(TeachPack));
	memcpy(data.m_head, HEADER,4);
	data.m_length =  sizeof(TeachPack) - 8;
	data.m_nType = TYPE_HISTORY_EX;
	data.m_nSize =1;
	data.m_nIndex	= 0;
	data.m_nOption	= 0x0080;
	memcpy(data.m_cCode, my_request_code_info.m_cCode2,6);
	data.m_cCodeType = my_request_code_info.m_cCodeType2;

	//日线请求
	data.m_cPeriod = PERIOD_TYPE_DAY;  //请求周期类型
	data.m_lBeginPosition =0;          //请求开始位置
        data.m_nPeriodNum =1;
	data.m_nDay = 1000*data.m_nPeriodNum;//请求个数(1000个)
	data.m_nSize2 =0;
	memcpy(data.m_cCode2,my_request_code_info.m_cCode2, 6);
	data.m_cCodeType2 = my_request_code_info.m_cCodeType2;

	//周线

	//月线

	//5分钟

	//15分钟

	//30分钟

	//60分钟

	//120分钟

	/*
	data.m_cPeriod = 0x00c0;
	data.m_lBeginPosition = 0;
	data.m_nPeriodNum = 60;
	data.m_nDay = 15*data.m_nPeriodNum;
	data.m_nSize2 = 0;
	memcpy(data.m_cCode2, "600006", 6);
	data.m_cCodeType2 = 0x1101;
	*/
	 

	memset(request, 0, sizeof(data));
	memcpy(request, &data, sizeof(data));
	int r = send(sclient, request, sizeof(data), 0);  
	printf("r:%d\n", r);
	printf("历史请求\n");
}

void 
client_parse_history(my_buff)
     buff_t * my_buff;
{
  unsigned short code_type;
  char my_code[7];
  memset(my_code, 0, 7);
  printf("client_parse_history\n");		
  AnsTrendData2 * pHisData222 = (AnsTrendData2*)my_buff->p_res_media_h;
	//	AnsTrendData * aaaaa = (AnsTrendData *)pTemp;
	//	AnsDayDataEx2 * g = (AnsDayDataEx2* )my_buff->p_res_media_h;//历史数据对上了这个结构体
	//StockCompDayDataEx2 *stockData = (StockCompDayDataEx2*)g->m_sdData;
	//int lengthaa = sizeof(AnsDayDataEx2)-4+sizeof(StockCompDayDataEx2)*g->m_nSize;
	//printf("数据包有效长度%d",lengthaa);
  if(pHisData222->m_nType == TYPE_HISTORY){
    AnsDayDataEx *g = (AnsDayDataEx*)(pHisData222);
    StockCompDayDataEx *stockData = g->m_sdData;
    int code_len = strlen(g->m_dhHead.m_nPrivateKey.m_pCode.m_cCode2);
    memcpy(my_code,g->m_dhHead.m_nPrivateKey.m_pCode.m_cCode2, code_len);
    code_type = g->m_dhHead.m_nPrivateKey.m_pCode.m_cCodeType2;
    int i = 0;
    // do_mysql_connect();
    for (i =0;i<g->m_nSize;i++){				
	    /*
	    printf("index:%d code:%s  date:%ld open:%ld, max:%ld min:%ld close:%d,money:%d,total:%ld\n",
		   i+1,
		   code,
		   stockData->m_lDate,
		   stockData->m_lOpenPrice,
		   stockData->m_lMaxPrice,
		   stockData->m_lMinPrice,
		   stockData->m_lClosePrice,
		   stockData->m_lMoney,
		   stockData->m_lTotal);
	    */
      insert_into(code_type,
		  my_code,
		  stockData->m_lDate,
		  stockData->m_lOpenPrice,
		  stockData->m_lMaxPrice,
		  stockData->m_lMinPrice,
		  stockData->m_lClosePrice,
		  stockData->m_lMoney,
		  stockData->m_lTotal);
      stockData++;
    }
  }
  //	do_mysql_close();
	
  printf("success\n");
}

//插入数据库
static int
insert_into(code_type, code ,date, open, max, min, close, money, total)
     unsigned short code_type;
     char * code;
     long date;
     long open;
     long max;
     long min;
     long close;
     long money;
     long total;
{
  char * template_sql = "insert into hr_history_day(code_type,code,m_date, open_price,close,max_price,min_price,money, total) values(%ld,'%s',%ld,%ld,%ld,%ld,%ld,%ld,%ld);";
  char sql[1024];
  memset(sql, 0, 1024);
  assert(sprintf(sql, template_sql, 
		 code_type,
		 code,
		 date,
		 open,
		 close,
		 max,
		 min,
		 money,
		 total));
  // printf("sql:%s\n", sql);
  assert(do_mysql_insert(sql)== 0);
}

/**********服务器端处理函数***************************/
int 
json_to_request_of_history(package)
     server_package_t * package;
{

  return 0;
} 
