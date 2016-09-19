/**
data deal
**/
#include<mysql.h>
#include "./../include/cJSON.h"
#include "./../include/data.h"
#include "./../include/d_login.h"
#include <zlib.h>

#include "./../include/d_init.h"
#include "./../include/d_heart.h"
#include "./../include/d_realtime.h"
#include "./../include/d_history.h"
#include "./../include/d_time_share.h"
#include "./../include/d_auto_push.h"



static void unpack(buff_t *);
static void clean_buff(buff_t *);
static void parse_default(buff_t *);
static void request_default(int sclient, t_base_c_request_head * head);
static void json_to_reqest_of_default();
//extern buff_t my_buff;

#define DEAL_LEN 5
t_deal deal[] = {
	{REQUEST,                 TYPE_LOGIN_EX,     
	REQUEST_FUNC(login),      PARSE_FUNC(login)},
	{REQUEST,                 TYPE_INIT_EX,      
	REQUEST_FUNC(init),       PARSE_FUNC(init)}, 
	{REQUEST,                 TYPE_HEART_EX,     
	REQUEST_FUNC(heart),      PARSE_FUNC(heart)},
	{REQUEST,                 TYPE_REALTIME_EX,  
	REQUEST_FUNC(realtime),   PARSE_FUNC(realtime)},	
	{REQUEST,                 TYPE_HISTORY_EX,   
	REQUEST_FUNC(history),    PARSE_FUNC(history)},	
	{REQUEST,                 TYPE_TIME_SHARE_EX,
	REQUEST_FUNC(time_share), PARSE_FUNC(time_share)},	
	{REQUEST,                  TYPE_AUTO_PUSH_EX, 
	REQUEST_FUNC(auto_push),  PARSE_FUNC(auto_push)}, 

//	{RESPONSE,  TYPE_SERVERINFO_EX, NULL, parse_default},
//	{RESPONSE,  TYPE_DAY_CURPOS_EX, NULL, parse_default}, 

	{EMPTY, 0, NULL, NULL}
};

//客户端请求类型-sql模板(映射表)
typedef struct
{
  char type[CLI_REQ_TYPE_LEN];
  char sql_template[100];
  type_general_sql func;//处理函数指针
}request_template_t;
request_template_t  req_tem_u[]={
  {"00010010001", 
   "select * from hr_realtime where %s",
   SQL_TEMPLATE_FUN(realtime)
  },

   {"","",}
};

//客户端请求类型-数据库结果解析(映射表)
typedef struct{
  char type[CLI_REQ_TYPE_LEN];
  type_general_json func;//处理函数指针
}request_db_parse_t;
request_db_parse_t req_db_p_u[] = {
  {"00010001001", DB_TO_JSON_FUN(realtime)},

  {"",NULL}
};

void request_server(int sclient, int type){
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
void parse(buff_t * my_buff){	
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
			if(deal[i].d_type == REQUEST
				&& deal[i].type == my_buff->p_res_media_h->type){
				if(deal[i].func_response){
					deal[i].func_response(my_buff);
					clean_buff(my_buff);
					return;
				}
			}
		}
	}
}

static void parse_default(buff_t * my_buff){
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
 	TransZipData2   * zheader;//·µ»ØÐÅÏ¢Í·œá¹¹(Ñ¹Ëõ°üžñÊœ)

	zheader = (TransZipData2 *)(my_buff->p_res_media_h);


	uLongf pRetLen = (uLongf)zheader->m_lOrigLen;
	memset(my_buff->unpack_buff, 0 , my_buff->unpack_buff_len);
	
	int unzip =  uncompress((Bytef *)my_buff->unpack_buff, &pRetLen,
				(Bytef*)zheader->m_cData, (uLongf)zheader->m_lZipLen);
	if(unzip == Z_OK
	&& pRetLen == zheader->m_lOrigLen){
		my_buff->is_direct = false;	
		my_buff->p_res_media_h = (p_response_meta_header)my_buff->unpack_buff;
		parse(my_buff);
	}
	printf("status:%d\n", unzip);
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
  
  //printf("request_head:%s\n", request_str);
  //printf("request_body:%s\n", request_str+PACKAGE_HEAD_LEN);

  //解析客户段请求类型，获取json对象及其类型
  assert(parse_client_request(package) == 0);
  //  entity = cJSON_Parse(str_request+PACKAGE_HEAD_LEN);

  printf("out parse_cilent_request...\n");
  printf("type:%s\n", req->type);

  //查找sql模板,通过json对象并形成sql
  int i;
  //char *tmp = cJSON_Print(entity);
  //printf("entity:%s\n", tmp);
  for(i=0; req_tem_u[i].func ;i++){
    if(req_tem_u[i].func){
      printf("get req_tem_u...\n");
      package->sql_template = req_tem_u[i].sql_template;
      assert(req_tem_u[i].func(package) == 0);
      break;
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
  for(i=0; req_db_p_u[i].func; i++){
    if(req_db_p_u[i].func){
      assert(req_db_p_u[i].func(package) == 0);
      break;
    }
  }
  printf("parse db...\n");

  //解析json对象为字符串
  unsigned long send_buff_len  = format_json_to_client(package);
  printf("format out...\n");

  printf("send_buff_head:%s\n", resp->package_head);
  printf("send_buff_body:%s\n", resp->package_body);
  printf("send length:%d\n",    resp->send_buff_len);
  //通过上面获取到的json字符串，发送客户端
  if(write(package->client_fd, resp->send_buff, resp->send_buff_len)<0){
    printf("send error");
  }
}
