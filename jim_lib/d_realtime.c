#include "./../include/d_realtime.h"

static int general_sql_from_simple_of_realtime(server_package_t *);
static int general_sql_from_multi_of_realtime(server_package_t *);
static int check_code_type_category(buff_t *);//���code type�������
static void do_foreign_exchange(char *,buff_t *,int);//�������
static void do_no_foreign_exchange(char *,buff_t *,int);//��������
static void do_norm(char *,buff_t *,int);//����ָ��
static void do_stock(char *,buff_t *,int);//�����Ʊ

/*
	ʵʱ���鴦��
*/
/**********�ͻ�������******************************/
char	g_cPacketIndex = 10;
#define PACKET_FLAG_NUM	4

//��������
void 
client_request_realtime(sclient, head)
  int sclient;
  t_base_c_request_head * head;
{
        printf("realtime request\n");
  
	char request[1024];

	RealPack data ;
	memset(&data,0x00,sizeof(RealPack));
	memcpy(data.m_head, HEADER, 4);
	data.m_length =  sizeof(RealPack) - 8+ sizeof(CodeInfo)*1;
	data.m_nType = TYPE_REALTIME_EX;
	data.m_nSize =1;
	data.m_nOption	= 0x0080;
	
	#define SIZE 1
	CodeInfo ary[1];
	memset(ary, 0, sizeof(CodeInfo));
	
	/*�����*/
	/*
	  CodeInfo ary[1];
	  memset(ary, 0, sizeof(CodeInfo));
	  memcpy(ary[0].m_cCode2, "XAU", 6);
	  ary[0].m_cCodeType2 = 0x5b00;
	*/

	/*���:pass*/
	/*
	memcpy(ary[0].m_cCode2,"AUDUSD",6);
	ary[0].m_cCodeType2 = 0xffff8100;
	*/
	
	//ָ��:pass	
	//memcpy(ary[0].m_cCode2, "2A01", 6);
	//ary[0].m_cCodeType2 = 0x1200;
	
	
	//��Ʊ:pass
	memcpy(ary[0].m_cCode2, "600000", 6);
	ary[0].m_cCodeType2 = 0x1101;
	
	memset(request, 0, sizeof(data));
	memcpy(request, &data, sizeof(data));
	memcpy(request+sizeof(data), &ary, sizeof(CodeInfo));
	int r = send(sclient, request, sizeof(data)+sizeof(CodeInfo), 0);  
	printf("r:%d\n", request);
	printf("ʵʱ����\n");
}

//��������
void 
client_request_realtime_m(sclient, head)
  int sclient;
t_base_c_request_head * head;
{
  
}

void 
client_parse_realtime(my_buff)
  buff_t * my_buff;
{
  printf("parse realtime\n");
  /*
  int category = check_code_type_category(my_buff);
  
  switch(category){
  case 1:
    //�������
    {
      do_foreign_exchange(my_buff);
    }
    break;
  case 2:
    //��������
    {
      do_no_foreign_exchange(my_buff);
    }
    break;
  case 3:
    //����ָ��
    {
      do_norm(my_buff);
    }
    break;
  case 4:
    //�����Ʊ
    {
      do_stock(my_buff);
    }
    break;
  default:
    break;
  }
  */
  
  AskData2 * data_head = (AskData2 *)(my_buff->buff+8);
  char code[7]={0};
  int i=0;

  for(i=0; i< data_head->m_nSize; i++){
    CommRealTimeData * data_type = (CommRealTimeData *)(my_buff->buff
							+ 28
							+ i*(sizeof(CommRealTimeData)+sizeof(HSQHRealTime2)));
    memcpy(code, data_type->m_cCode, 6);
    if(data_type->m_cCodeType == 0x5b00)//�����
      {
	do_no_foreign_exchange(code, my_buff, i);
      }
    else if(data_type->m_cCodeType == 0xffff8100)//���
      {
	do_foreign_exchange(code, my_buff, i);
      }
    //ָ�����ɽ�����λ���������ڣ��������ֻ������λС������ӵ�λ(�����)
    else if(data_type->m_cCodeType == 0x1200)
      {
	do_norm(code, my_buff, i);
      }
    else if(data_type->m_cCodeType == 0x1101)//��Ʊ
      {
	do_stock(code, my_buff, i);
      }
  }
}

//����г����
/*
  1-���
  2-�����
  3-ָ��
  4-��Ʊ
*/
static int
check_code_type_category(my_buff)
     buff_t * my_buff;
{
  //���code_type�����������
  printf("���code type ����...\n");
  return 2;
}

//�������
static void
do_foreign_exchange(code, my_buff, i)
     char * code;
     buff_t * my_buff;
     int i;
{
  printf("do_foreign_exchange...\n");
  HSWHRealTime * entity = (HSWHRealTime*)(my_buff->buff
					  + 28
					  + sizeof(CommRealTimeData)
					  + i*(sizeof(CommRealTimeData)+sizeof(HSWHRealTime)));
}

//��������
static void
do_no_foreign_exchange(code,my_buff, i)
     char * code;
     buff_t * my_buff;
     int i;
{
  printf("do_no_foreign_exchange...\n");
  
  HSQHRealTime2 * entity = (HSQHRealTime2*)(my_buff->buff
						+28
						+sizeof(CommRealTimeData2)
						+i*(sizeof(CommRealTimeData2)+sizeof(HSQHRealTime2)));
      printf("Ʒ��:%s, ���¼�:%d\n", code, entity->m_lNewPrice);
  
}

//����ָ��
static void
do_norm(code, my_buff, i)
     char * code;
     buff_t * my_buff;
     int i;
{
  printf("����ָ��...\n");
  HSIndexRealTime * tmp = (HSIndexRealTime*)(my_buff->buff
					     +28
					     +sizeof(CommRealTimeData)
					     + i*(sizeof(CommRealTimeData)+sizeof(HSIndexRealTime)));
}

//�����Ʊ
static void
do_stock(code, my_buff, i)
    char * code;
    buff_t * my_buff;
    int i;
{
  printf("�����Ʊ...\n");
  HSStockRealTime * tmp = (HSStockRealTime *)(my_buff->buff
					      +28
					      +sizeof(CommRealTimeData)
					      +i*(sizeof(CommRealTimeData)+sizeof(HSStockRealTime)));
}

void parse_realtime_pack(buff_t * my_buff){
	
}


/*********������������***********************************/
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
t_base_c_request_head *
json_to_request_of_realtime(json_str)
char * json_str;
{
	t_base_c_request_head  head;

	return &head;
}

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
  //����Ʒ������
  if(req->data){
    printf("simple mode!\n");
    general_sql_from_simple_of_realtime(package);
    free(req->data);
    free(req->json);
    return 0;
  }
  
  req->data = cJSON_GetObjectItem(req->json, "data_list");
  //���Ʒ������
  if(req->data){
    general_sql_from_multi_of_realtime(package);
    free(req->data);
    free(req->json);    
  }  

  
  return result;
}

//����Ʒ������(test_pass)
static int
general_sql_from_simple_of_realtime(package)
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

//���Ʒ������
static int
general_sql_from_multi_of_realtime(package)
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

  resp = package->response;
  assert(num_fields);
  //printf("num_fields:%d\n", num_fields);

  
  while((row = mysql_fetch_row(package->db_back)) != NULL){
    assert(row);
    item = cJSON_CreateObject();
    assert(item);
    for(i=0; i<num_fields; i++){
      //����json����
      field = mysql_fetch_field_direct(package->db_back, i);
      assert(field);      
      cJSON_AddStringToObject(item, field->name, row[i]);
      //printf("%s:%s,", field->name, row[i]); 
    }
    cJSON_AddItemToArray(resp->list, item); 
    // printf("\n");
  }

  return result;
}
