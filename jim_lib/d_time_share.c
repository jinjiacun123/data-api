#include "./../include/d_time_share.h"
#include "./../include/data.h"
/**
 分时走势
*/

void 
client_request_time_share(sclient, head)
  int sclient;
t_base_c_request_head * head;
{
	char request[1024];

	int test = sizeof(TrendPack);
	TrendPack data ;
	memset(&data,0x00,sizeof(TrendPack));
	memcpy(data.m_head,HEADER,4);
	data.m_length =  sizeof(TrendPack) - 8;
	data.m_nType = TYPE_TIME_SHARE_EX;//0x0301
	data.m_nSize = 0;
	data.m_nOption	= 0x0080;
	memcpy(data.m_cCode,"XAG",6);
	data.m_cCodeType = 0x5b00;
	memcpy(data.m_cCode2,"XAG",6);
	data.m_cCodeType2 = 0x5b00;

	memset(request, 0, 1024);
	memcpy(request, &data, sizeof(data));
	send(sclient, request, sizeof(data), 0);
	printf("分时请求发送完毕\n");
}

void 
client_parse_time_share(my_buff)
     buff_t * my_buff;
{
  printf("解析分时数据\n");	
  char * template_sql = "insert into hr_time_share_%s(code_type,code,new_price) values('0x%x','%s',%d)";
  char sql[100];
  char table[20];
  char code[7];
  memset(table, 0, 20);  
  memset(sql, 0, 100);
  memset(code, 0, 7);
  //PriceVolItem2* pPriceVolItem = (PriceVolItem2*)(my_buff->p_res_media_h + sizeof(AnsTrendData2)+1);  
  AnsTrendData2* pHisData222 = (AnsTrendData2 *)my_buff->p_res_media_h;
  memcpy(code, pHisData222->m_cCode, 6);
  int code_len = strlen(code);
  int index = 0;
  for(index = 0; index < code_len; index++){
    code[index] = tolower(code[index]);
  }
  if(pHisData222->m_nType == TYPE_TIME_SHARE_EX){
    PriceVolItem2 * pPriceVolItem = (PriceVolItem2 *)pHisData222->m_pHisData;
    do_mysql_connect();
    assert(sprintf(table, "0x%x_%s", 
		   pHisData222->m_cCodeType, 
		   code));
    int i = 0;
    for(i=0;i<pHisData222->m_nHisLen;i++){
      //char name[7]={0};
      //memcpy(name,pHisData222->m_cCode,6);
      printf("第%d条数据 收到code:%s  new:%d \n",i+1,
	     pHisData222->m_cCode,
	     pPriceVolItem->m_lNewPrice);
      
      assert(sprintf(sql, template_sql,
		     table,
		     pHisData222->m_cCodeType,
		     code,
		     pPriceVolItem->m_lNewPrice));
      assert(do_mysql_insert(sql) == 0);
      pPriceVolItem ++;
    }
    do_mysql_close();
  }
}

/**********服务器处理函数*********************************/
int
json_to_request_of_time_share(package)
     server_package_t * package;
{
  return 0;
}
