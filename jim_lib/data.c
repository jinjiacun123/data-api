/**
data deal
**/
#include<mysql.h>
#include "./../include/cJSON.h"
#include "./../include/data.h"
#include "./../include/d_login.h"
//#include <zlib.h>

#include "./../include/d_init.h"
#include "./../include/d_heart.h"
#include "./../include/d_realtime.h"
/*
#include "./../include/d_history.h"
#include "./../include/d_time_share.h"
#include "./../include/d_auto_push.h"
*/



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
	
	/*	
	{REQUEST,  TYPE_HISTORY_EX,   
	REQUEST_FUNC(history),    PARSE_FUNC(history),  TO_JSON_FUNC(history)},	
	{REQUEST,  TYPE_TIME_SHARE_EX,
	REQUEST_FUNC(time_share), PARSE_FUNC(time_share), TO_JSON_FUNC(time_share)},	
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
do_client_request(client_fd, str_request)
     int client_fd;
     char * str_request;
{
  cJSON * entity,* result;
  char type[CLI_REQ_TYPE_LEN];
  db_back_t * db_back;
 
  memset(type, 0, CLI_REQ_TYPE_LEN);
  //解析客户段请求类型，获取json对象及其类型
  parse_client_request(str_request, entity, type);

  //查找sql模板,通过json对象并形成sql
  int i;
  memset(sql_buffer, 0, SQL_BUFF_MAX_LEN);
  for(i=0; req_tem_u[i].func ;i++){
    if(req_tem_u[i].func){
      req_tem_u[i].func(type, 
			entity, 
                        req_tem_u[i].sql_template, 
			sql_buffer);
      break;
    }
  }

  //发送sql数据库请求,返回数据库结果对象或者bool
  //解析数据库返回,如果是结果数据库结果对象,形成Json对象
  for(i=0; req_db_p_u[i].func; i++){
    if(req_db_p_u[i].func){
      req_db_p_u[i].func(type, db_back, result);
      break;
    }
  }

  //解析json对象为字符串
  char send_buff[SEND_BUFF_LEN];
  memset(send_buff, 0, SEND_BUFF_LEN);
  unsigned long send_buff_len  = format_json_to_client(result, send_buff, type);

  //通过上面获取到的json字符串，发送客户端
  if(write(client_fd, send_buff, send_buff_len)){
    printf("send error");
  }
}
