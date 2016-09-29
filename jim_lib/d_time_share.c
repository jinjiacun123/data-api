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
    //free(req->data);
    //free(req->json);
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
  //unsigned int index=1;
  //unsigned int size=20;
  
  req = package->request;
  memset(code_type, 0, 100);
  assert(json_get_string(req->data, "code_type", code_type) == 0);
  assert(code_type);
  lower_string(code_type);
  memset(code, 0, 100);
  assert(json_get_string(req->data, "code", code) == 0);
  assert(code);
  lower_string(code);
  /*
  index = json_get_int(req->data, "index");
  assert(index != -1);
  size = json_get_int(req->data, "size");
  assert(size != -1);
  */
  char table_ex_template_sql[] = "%s_%s";
  char table_ex[20];
  memset(table_ex, 0, 20);
  assert(strcat(table_ex, code_type));
  assert(strcat(table_ex, "_"));
  assert(strcat(table_ex, code));
  /*
  assert(strcat(table_ex, tolower(code_type)));
  assert(strcat(table_ex, "_"));
  assert(strcat(table_ex, tolower(code)));
  */
  // assert(sprintf(table_ex, table_ex_template_sql,
  //		 tolower(code_type),tolower(code)));
  assert(sprintf(package->sql_buffer, package->sql_template, 
		 table_ex));
  assert(package->sql_buffer);
  return result;
}

/**
   from db result to struct

   |type|code_type|code|new_price|new_price|...|new_price
*/
int
general_json_from_db_time_share(package)
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

  resp->send_buff = (char *)malloc(669*4+PACKAGE_HEAD_LEN);
  memset(resp->send_buff, 0, 669*4+PACKAGE_HEAD_LEN);
  int off = PACKAGE_HEAD_LEN;
  unsigned long new_price = 0;
  int new_price_len = sizeof(unsigned long);
  while((row = mysql_fetch_row(package->db_back)) != NULL){
    assert(row);
    new_price = atol(row[i]);
    memcpy(resp->send_buff+off, &new_price, new_price_len);
    off += 4;
    // item = cJSON_CreateObject();
    //assert(item);
    /*
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
    */
    // cJSON_AddItemToArray(resp->list, item); 
    // printf("\n");
  }

  return result;
}

//调整要发送的json对象，形成{"type":"00010001","length":xxxxx}{xxxx}
//返回目标字符串长度,send_buff内容(test-pass)
unsigned long
format_json_to_client_time_share(package)
     server_package_t * package;
{
  server_response_t * resp;
  server_request_t * req;
  resp = package->response;
  req  = package->request; 
  
  resp->send_buff_len = 669 * 4 + PACKAGE_HEAD_LEN;
  memcpy(resp->send_buff, "{\"type\":\"000100030001\",\"length\":2676}", PACKAGE_HEAD_LEN);


  return resp->send_buff_len;
}
