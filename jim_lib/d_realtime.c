#include "./../include/d_realtime.h"

extern  buff_t my_buff;
static int general_sql_from_simple_of_realtime(cJSON *,char * ,char *);
static int general_sql_from_multi_of_realtime(cJSON *,char *,char *);

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
json_to_request_of_realtime(json_str)
char * json_str;
{
	t_base_c_request_head  head;

	return &head;
}


void parse_realtime(my_buff)
  buff_t * my_buff;
{
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

/**
   from json object general to sql sentence
   
   code_type
   code

   simple:
   {'type':'xxx', 
    'data':{
           'code_type':'xxx',
           'code':'xxx'
	   }
   }

   multi:
   {'type':'xxx',
    'data_list':[
       {'code_type':'xxx','code':'xxx'},
       {'code_type':'xxx','code':'xxx'},
       ...
       {'code_type':'xxx','code':'xxx'}
    ]
   }
*/
int
general_sql_of_realtime(type, json, template_sql, sql)
     char  * type;
     cJSON * json;
     char  * template_sql;
     char  * sql;
{
  int result = 0;
  cJSON * data;
  cJSON * data_list;
  char tmp[100];

  //tmp = cJSON_Print(json);
  //assert(tmp);
  memset(tmp, 0, 100);
  assert(json_get_string(json, "type", tmp)==0);
  printf("type:%s", tmp);

  data = cJSON_GetObjectItem(json, "data");
  //单个品种请求
  if(data){
    printf("simple mode!\n");
    //    tmp = cJSON_Print(data);
    printf("data:%s\n", data);
    general_sql_from_simple_of_realtime(data, template_sql, sql);
    free(data);
  }
  
  data_list = cJSON_GetObjectItem(json, "data_list");
  //多个品种请求
  if(data_list){
    general_sql_from_multi_of_realtime(data_list, template_sql, sql);
    free(data_list);
  }  

  return result;
}

//单个品种请求
static int
general_sql_from_simple_of_realtime(data, template_sql, sql)
     cJSON * data;
     char * template_sql;
     char * sql;
{
  int result = 0;
  char code_type[100];
  char code[100];
  memset(code_type, 0, 100);
  assert(json_get_string(data, "code_type", code_type) == 0);
  assert(code_type);
  memset(code, 0, 100);
  assert(json_get_string(data, "code", code) == 0);
  assert(code);
  char tmp_template_sql[] = " code_type = '%s' and code = '%s' ";
  char tmp[1024];
  memset(tmp, 0, 1024);
  assert(sprintf(tmp, tmp_template_sql, code_type, code));
  printf("tmp:%s\n", tmp);
  assert(sprintf(sql, template_sql, tmp));
  assert(sql);
  return result;
}

//多个品种请求
static int
general_sql_from_multi_of_realtime(json, template_sql, sql)
     cJSON * json;
     char * template_sql;
     char * sql;
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
