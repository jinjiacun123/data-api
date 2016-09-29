/**
data deal
**/
#include<mysql.h>
#include "cJSON.h"
#include "data.h"
#include "d_login.h"
#include "do_json.h"
//#include <zlib.h>

#include "d_init.h"
#include "d_heart.h"
#include "d_realtime.h"
#include "d_history.h"
#include "d_time_share.h"
#include "d_auto_push.h"

static void unpack();
static void clean_buff();
static void parse_default();
static void request_default(int sclient, t_base_c_request_head * head);
static void json_to_reqest_of_default();
extern buff_t my_buff;

#define DEAL_LEN 5
t_deal deal[] = {
	{REQUEST,                 TYPE_LOGIN_EX,     
	REQUEST_FUNC(login),      PARSE_FUNC(login),    TO_JSON_FUNC(login)},
	{REQUEST,                 TYPE_INIT_EX,      
	REQUEST_FUNC(init),       PARSE_FUNC(init),     TO_JSON_FUNC(init)}, 
	{REQUEST,                 TYPE_HEART_EX,     
	REQUEST_FUNC(heart),      PARSE_FUNC(heart),    TO_JSON_FUNC(heart)},
	{REQUEST,                 TYPE_REALTIME_EX,  
	REQUEST_FUNC(realtime),   PARSE_FUNC(realtime), TO_JSON_FUNC(realtime)},	
	{REQUEST,  TYPE_HISTORY_EX,   
	REQUEST_FUNC(history),    PARSE_FUNC(history),  TO_JSON_FUNC(history)},		
	{REQUEST,  TYPE_TIME_SHARE_EX,
	REQUEST_FUNC(time_share), PARSE_FUNC(time_share), TO_JSON_FUNC(time_share)},
	/*
	{REQUEST,  TYPE_AUTO_PUSH_EX, 
	REQUEST_FUNC(auto_push),  PARSE_FUNC(auto_push),  TO_JSON_FUNC(auto_push)}, 
*/

//	{RESPONSE,  TYPE_SERVERINFO_EX, NULL, parse_default},
//	{RESPONSE,  TYPE_DAY_CURPOS_EX, NULL, parse_default}, 

	{EMPTY, 0, NULL, NULL}
};

//客户端请求类型-sql模板(映射表)
typedef struct
{
  char type[CLI_REQ_TYPE_LEN];
  char sql_template[100];
  type_general_sql func_sql;//处理函数指针(生成sql)
  type_general_json func_json;//处理函数指针(生成json)
  type_format_json_to_client func_buff;//生成发送数据包
}request_t;
/**
   00010001xxxx realtime
   00010002xxxx auto_push
   00010003xxxx time_share
   00010004xxxx history
*/
request_t  req_u[]={
  {"000100010001", 
   "select * from hr_realtime where %s",
   SQL_TEMPLATE_FUN(realtime),
   DB_TO_JSON_FUN(realtime),
   format_json_to_client
  },
  {"000100020001", 
   "",
   SQL_TEMPLATE_FUN(auto_push),
   DB_TO_JSON_FUN(auto_push),
   format_json_to_client
  },  
  {"000100030001", 
   "select new_price from hr_time_share_%s order by id asc",
   SQL_TEMPLATE_FUN(time_share),
   DB_TO_JSON_FUN(time_share),
   GENERAL_SEND_BUFF(time_share)
   },  
  {"000100040001", 
   "select * from hr_history_%s limit %d,%d",
   SQL_TEMPLATE_FUN(history),
   DB_TO_JSON_FUN(history),
   format_json_to_client
   },

  {"","",NULL,NULL,NULL}
};

void request_server(int sclient, int type)
{
	int i = 0;
	for(i = 0; deal[i].d_type != EMPTY; i++){
		if(deal[i].d_type == REQUEST
		&& deal[i].type == type){
			if(deal[i].func_request){
				deal[i].func_request(sclient);
			}
		}
	}
}

//解析
void parse(buff_t * my_buff)
{	
	int i;
	if(my_buff->is_direct)
		my_buff->p_res_media_h = (p_response_meta_header)(my_buff->buff+ my_buff->buff_parse_off);
	else//
		my_buff->p_res_media_h = (p_response_meta_header)(my_buff->unpack_buff);

        //解压
	if(my_buff->p_res_media_h->type == TYPE_ZIB_EX){
		unpack(my_buff);
		my_buff->is_direct = false;
		parse(my_buff);
	}

	if(my_buff->p_res_media_h){
		for(i=0; deal[i].d_type != EMPTY; i++){
			if(deal[i].d_type == RESPONSE
				&& deal[i].type == my_buff->p_res_media_h->type){
				if(deal[i].func_response){
					deal[i].func_response();
					//my_buff[t_index] = 0;
//					clean_buff();
					return;
				}
			}
		}
	}
}

static void parse_default(buff_t * my_buff)
{
	printf("类型ÐÍ:%x\n", my_buff->p_res_media_h->type);
	printf("默认");
}

static 
void clean_buff(buff_t * my_buff)
{
	my_buff->p_res_h = NULL;
	my_buff->p_res_media_h = NULL;
	my_buff->result_type = 0;

	if(my_buff->is_direct)
	{
		my_buff->buff_len = 0;
		my_buff->buff_off = 0;
		my_buff->buff_parse_off = 0;
		my_buff->is_direct = true;
		my_buff->result_type = 0;
	}
	else
	{
		memset(my_buff->unpack_buff, 0, my_buff->unpack_buff_len);
		my_buff->unpack_buff_len = 0;
	}
}

static
void unpack(buff_t *my_buff){
  /*
	TransZipData2   * zheader;//·µ»ØÐÅÏ¢Í·œá¹¹(Ñ¹Ëõ°üžñÊœ)

	zheader = (TransZipData2 *)(my_buff->p_res_media_h);


	uLongf pRetLen = (uLongf)zheader->m_lOrigLen;
	memset(my_buff->unpack_buff, 0 , my_buff->unpack_buff_len);
	
	int unzip =  uncompress((Bytef *)my_buff->unpack_buff, &pRetLen,
				(Bytef*)zheader->m_cData, (uLongf)zheader->m_lZipLen);
	if(unzip == Z_OK
	&& pRetLen == zheader->m_lOrigLen){
		my_buff[t_index].is_direct = false;	
		my_buff->p_res_media_h = (p_response_meta_header)my_buff->unpack_buff;
		parse(t_index);
	}
	printf("status:%d\n", unzip);
*/
}

//作为服务器端，处理客户端请求
void
do_client_request(package)
     server_package_t * package;
{
  cJSON * entity, * result, * list;
  server_request_t   * req;
  server_response_t  * resp;
  
  req  = package->request;
  resp = package->response;

  //解析客户段请求类型，获取json对象及其类型
  assert(parse_client_request(package) == 0);

  printf("out parse_cilent_request...\n");
  printf("type:%s\n", req->type);

  //查找sql模板,通过json对象并形成sql
  int i;
  //char *tmp = cJSON_Print(entity);
  //printf("entity:%s\n", tmp);
  for(i=0; req_u[i].func_sql ;i++){
    if(!strcmp(req_u[i].type, req->type)){
      if(req_u[i].func_sql){
	printf("get req_tem_u...\n");
	package->sql_template = req_u[i].sql_template;
	assert(req_u[i].func_sql(package) == 0);
	break;
      }
    }
  }
  printf("find sql...\n");

  //发送sql数据库请求,返回数据库结果对象或者bool
  package->db_back = do_mysql_select(package->sql_buffer);
  assert(package->db_back);
  printf("db back...\n");

  //解析数据库返回,如果是结果数据库结果对象,形成Json对象
  resp->json = cJSON_CreateObject();
  strcpy(resp->type, req->type);
  cJSON_AddStringToObject(resp->json, "type", req->type);
  cJSON_AddItemToObject(resp->json, "list", resp->list = cJSON_CreateArray());
  for(i=0; req_u[i].func_sql; i++){
    if(!strcmp(req_u[i].type, req->type)){
      if(req_u[i].func_json){
	assert(req_u[i].func_json(package) == 0);
	break;
      }
    }
  }
  printf("parse db...\n");

  //解析json对象为字符串  
  unsigned long send_buff_len  = 0;  
  
  for(i = 0; req_u[i].func_sql; i++){
    if(!strcmp(req_u[i].type, req->type)){
      if(req_u[i].func_buff){
	send_buff_len = req_u[i].func_buff(package);
	break;
      }
    }
  }
  /*
  if(strcmp(req->type, "000100030001")){
   send_buff_len = format_json_to_client(package);
  }
  else{
    send_buff_len = 669 * 4 + PACKAGE_HEAD_LEN;
    resp->send_buff_len = send_buff_len;    
    memcpy(resp->send_buff, "{\"type\":\"000100030001\",\"length\":2676}", PACKAGE_HEAD_LEN);

    //  strcpy(resp->package_head ,cJSON_Print(head));
  }
  */
  printf("format out...\n");

  printf("send_buff_head:%s\n", resp->package_head);
  //printf("send_buff_body:%s\n", resp->package_body);
  printf("send length:%d\n",    resp->send_buff_len);
  //通过上面获取到的json字符串，发送客户端
  if(write(package->client_fd, resp->send_buff, send_buff_len)<0){
    printf("send error");
  }
}
