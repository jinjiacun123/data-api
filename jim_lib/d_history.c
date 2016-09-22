#include "./../include/d_history.h"
/*
  历史
**/
static int general_sql_from_simple(server_package_t *);

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
	/*	
	AnsDayDataEx2 * g = (AnsDayDataEx2* )my_buff.p_res_media_h;//历史数据对上了这个结构体
	StockCompDayDataEx2 *stockData = (StockCompDayDataEx2*)g->m_sdData;
	int lengthaa = sizeof(AnsDayDataEx2)-4+sizeof(StockCompDayDataEx2)*g->m_nSize;
	printf("数据包有效长度%d",lengthaa);
	
	int i = 0;
	for (i =0;i<g->m_nSize;i++)
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
	*/
}

t_base_c_request_head *
json_to_request_of_history(json)
char * json;
{
  return NULL;
}



/**
   from json object general to sql sentence 

   code type
   code 
   
   simple:
   {'type':'xxx',
   'data':{
          'code_type':'xxx',
	  'code':'xxx',
	  'index':'xxx',
	  'size':'xxx'
	  }
   }
*/
int
general_sql_of_history(package)
     server_package_t * package;
{
  server_request_t * req;
  int result = 0;
  char tmp[100];
  req = package->request;

  printf("enter general_sql_of_history...\n");
  
  req->data = cJSON_GetObjectItem(req->json, "data");
  if(req->data){
    printf("simple mode!\n");
    general_sql_from_simple(package);
    free(req->data);
    free(req->json);
  }

  return result;
}

//单个品种请求
static int
general_sql_from_simple(package)
     server_package_t * package;
{
  server_request_t * req;
  int result = 0;
  char code_type[100];
  char code[100];
  char circle[20];
  unsigned int index;
  unsigned int size;
  
  req = package->request;
  memset(code_type, 0, 100);
  assert(json_get_string(req->data, "code_type", code_type) == 0);
  assert(code_type);
  memset(code, 0, 100);
  assert(json_get_string(req->data, "code", code) == 0);
  assert(code); 
  memset(circle, 0, 20);
  assert(json_get_string(req->data, "circle", circle) == 0);
  assert(circle);
  size = json_get_int(req->data, "size"); 
  assert(size != -1);
  index = json_get_int(req->data, "index");
  assert(index != -1); 
  char table_ex_template_sql[] = "%s_%s_%s";
  char where[1024];
  char table_ex[20];
  memset(where, 0, 1024);
  memset(table_ex, 0, 20);
  assert(sprintf(table_ex, table_ex_template_sql, 
		 circle,tolower(code_type), tolower(code)));
   
  assert(sprintf(package->sql_buffer, 
		 package->sql_template,
		 table_ex,
		 index,
		 size));
  printf("sql:%s\n", package->sql_buffer);
  assert(package->sql_buffer);
  return result;
}
