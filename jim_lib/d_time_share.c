#include "./../include/d_time_share.h"
/**
 分时走势
*/
static int general_sql_from_simple(server_package_t *);


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

void parse_time_share()
{
	printf("解析分时数据\n");
	/*
	PriceVolItem2* pPriceVolItem = (PriceVolItem2*)my_buff.p_res_media_h + sizeof(AnsTrendData2);
	AnsTrendData2* pHisData222 = (AnsTrendData2 *)my_buff.p_res_media_h;
	for(int i=0;i<pHisData222->m_nHisLen;i++)
	{
		char name[7]={0};
		memcpy(name,pHisData222->m_cCode,6);
		printf("第%d条数据 收到code:%s  new:%ld \n",i+1,name,pPriceVolItem->m_lNewPrice);
		pPriceVolItem ++;
	}
	*/
}

/**
   from json object general to sql sentence

   code_type
   code

   simple:
   {'type':'xxx',
   'data':{
          'code_type':'xxx',
	  'code':'xxx',
	  'index':xxx,
	  'size':xxx
	  }
    }
*/
int
general_sql_of_time_share(package)
     server_package_t * package;
{
  server_request_t * req;
  int result = 0;
  char tmp[100];
  req = package->request;

  printf("enter general_sql_of_realtime...\n");

  req->data = cJSON_GetObjectItem(req->json, "data");
  //单个品种请求
  if(req->data){
    printf("simple mode!\n");
    general_sql_from_simple(package);
    free(req->data);
    free(req->json);
    return 0;
  }
}

//单个品种请求(test_pass)
static int
general_sql_from_simple(package)
     server_package_t * package;
{
  server_request_t * req;
  int result = 0;
  char code_type[100];
  char code[100];
  // unsigned int index=1;
  //unsigned int size=20;
  
  req = package->request;
  memset(code_type, 0, 100);
  assert(json_get_string(req->data, "code_type", code_type) == 0);
  assert(code_type);
  memset(code, 0, 100);
  assert(json_get_string(req->data, "code", code) == 0);
  assert(code);
  //index = json_get_int(req->data, "index");
  //assert(index != -1);
  //size = json_get_int(req->data, "size");
  //assert(size != -1);
  char table_ex_template_sql[] = "%s_%s";
  char table_ex[20];
  memset(table_ex, 0, 20);
  assert(sprintf(table_ex, table_ex_template_sql,
		 tolower(code_type),tolower(code)));
  assert(sprintf(package->sql_buffer, package->sql_template, 
		 table_ex));
  assert(package->sql_buffer);
  return result;
}
