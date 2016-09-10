#ifndef __DATA_H__
#define __DATA_H__
#include <mysql.h>
#include <stdbool.h>
#include "common.h"
#include "cJSON.h"
#define STOCK_NAME_SIZE 16
#define BUFF_MAX_LEN 10
#define PACKAGE_HEAD_LEN 50
#define PACKAGE_MAX_LEN 1024*1024
#define CLI_REQ_TYPE_LEN 50
#define SEND_BUFF_LEN 1024
#define SQL_BUFF_MAX_LEN 4096
char * sql_buffer[SQL_BUFF_MAX_LEN];

#define REQUEST_FUNC(a) request_##a 
#define PARSE_FUNC(a) parse_##a
#define TO_JSON_FUNC(a) json_to_request_of_##a

/*number of type as hexadicimal*/
#define TYPE_EMPTY 	0x0D03 //empty
#define TYPE_INIT  	0x0101 //init
#define TYPE_LOGIN 	0X0102 //login
#define TYPE_HEART 	0x0905 //heart tick
#define TYPE_ZIB        0x8001 //
#define TYPE_REALTIME   0x0201 //
#define TYPE_HISTORY    0x0402 //
#define TYPE_TIME_SHARE 0x0301 //
#define TYPE_AUTO_PUSH  0x0A01 //
#define TYPE_SERVERINFO 0x0103 //
#define TYPE_DAY_CURPOS 0x020c //


/*number of type as decimal system*/
#define TYPE_EMPTY_EX 		3331  //empty
#define TYPE_INIT_EX  		257   //init
#define TYPE_LOGIN_EX 		258   //login
#define TYPE_HEART_EX	     	2309  //
#define TYPE_ZIB_EX		32769 //
#define TYPE_REALTIME_EX     	513   //
#define TYPE_HISTORY_EX      	1026  //
#define TYPE_TIME_SHARE_EX   	769   //
#define TYPE_AUTO_PUSH_EX    	2561  //
#define TYPE_SERVERINFO_EX   	259   //
#define TYPE_DAY_CURPOS_EX   	524   //

//back result of database
typedef union{
  int i_result;
  MYSQL_RES * result;
}db_back_t;


//type of request,json package of reqeust
typedef int (*type_general_sql)(char *, cJSON *, char *, char *);
//type of request,back of db, to general json object
typedef int (*type_general_json)(char *, db_back_t *, cJSON *);
#define SQL_TEMPLATE_FUN(a) general_sql_of_##a 
#define DB_TO_JSON_FUN(a) general_json_from_db_##a


//deal business logic
typedef void (*request_type)(int);
typedef void (*response_type)();
typedef enum {
	REQUEST,      
	RESPONSE,   //parse and deal
	PRESPONSE,  //unpack
	EMPTY
}deal_type;

typedef struct{
	deal_type d_type;             //request or response
	int type;                   
	request_type func_request;    //function point of request
	response_type func_response;  //function point of response
}t_deal;

//compress package info
typedef struct 
{
	unsigned short m_nType;
	short m_nAlignment;
	long m_lZipLen;
	long m_lOrigLen;
	char m_cData[1];
}TransZipData2;


//result of back include infomation of package(direct parse not compress package)
struct response_header{
	char str[4];
	int length;
};
typedef struct response_header * p_response_header;

//tuple data
struct response_meta_header{
	unsigned short type;
	char index;
};
typedef struct response_meta_header * p_response_meta_header;

typedef struct {
	char buff[MAX_BUFF];		  
	unsigned int buff_len;		  
	unsigned int buff_off;        

	unsigned int buff_parse_off;      
	char unpack_buff[MAX_BUFF];		  
	unsigned int unpack_buff_len;     

	p_response_header p_res_h;
	
	p_response_meta_header p_res_media_h;

	
	unsigned int result_type;

	bool is_direct;
}buff_t;


//public view function
extern void request_server(int c_client, int type);//作为客户端请求服务器
extern void do_client_request(int,char *);//作为服务器处理客户端请求(sockfd, string of request)
extern void parse(buff_t *);
#endif
