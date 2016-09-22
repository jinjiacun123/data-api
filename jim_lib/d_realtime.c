#include "./../include/d_realtime.h"
/*
  实时
**/

extern  buff_t my_buff;
static int general_sql_from_simple(server_package_t *);
static int general_sql_from_multi(server_package_t *);

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
general_sql_of_realtime(package)
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
  
  req->data = cJSON_GetObjectItem(req->json, "data_list");
  //多个品种请求
  if(req->data){
    general_sql_from_multi(package);
    free(req->data);
    free(req->json);    
  }  

  
  return result;
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
  
  req = package->request;
  memset(code_type, 0, 100);
  assert(json_get_string(req->data, "code_type", code_type) == 0);
  assert(code_type);
  memset(code, 0, 100);
  assert(json_get_string(req->data, "code", code) == 0);
  assert(code);
  char tmp_template_sql[] = " code_type = '%s' and code = '%s' ";
  char tmp[1024];
  memset(tmp, 0, 1024);
  assert(sprintf(tmp, tmp_template_sql, code_type, code));
  printf("tmp:%s\n", tmp);
  assert(sprintf(package->sql_buffer, package->sql_template, tmp));
  assert(package->sql_buffer);
  return result;
}

//多个品种请求
static int
general_sql_from_multi(package)
     server_package_t * package;
{
  int result = 0;

  return result;
}

/**
  from db result to json object


  {'list':[{xxx},{xxx},...,{xxx}]}
*/
int
general_json_from_db_realtime(package)
     server_package_t * package;
{
  int result = 0;
  
  cJSON * item;
  MYSQL_ROW row;
  MYSQL_FIELD * field;
  int i=0;
  int num_fields = mysql_num_fields(package->db_back);
  server_response_t * resp;
  char * str_null = "NULL";

  resp = package->response;
  assert(num_fields);
  //printf("num_fields:%d\n", num_fields);

  
  while((row = mysql_fetch_row(package->db_back)) != NULL){
    assert(row);
    item = cJSON_CreateObject();
    assert(item);
    for(i=0; i<num_fields; i++){
      //创建json对象
      field = mysql_fetch_field_direct(package->db_back, i);
      assert(field);
      if(row[i] != 0){
	cJSON_AddStringToObject(item, field->name, row[i]);
      }
      else{
	cJSON_AddStringToObject(item, field->name, str_null);
      }
      //printf("%s:%s,", field->name, row[i]); 
    }
    cJSON_AddItemToArray(resp->list, item); 
    // printf("\n");
  }

  return result;
}
