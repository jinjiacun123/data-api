#include<stdio.h>
#include<stdlib.h>
#include<mysql.h>
#include "./../include/cJSON.h"
#include "./../include/common.h"
#include "./../include/data.h"
#include "./../include/d_realtime.h"

/*
typedef t_base_c_request_head * (json_to_request_func)(char *);

struct{
	char client_request_type[10];
	json_to_request_func * func;
}map_request[]={
  {"00010001", TO_JSON_FUNC(realtime)},//realtime

	
  {"000000",}//empty
};
*/


//send package to client
char *
struct_to_json()
{
	cJSON *root,*fmt;   
  	root=cJSON_CreateObject();     
  	cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));   
  	cJSON_AddItemToObject(root, "format", fmt=cJSON_CreateObject());   
  	cJSON_AddStringToObject(fmt,"type",     "rect");   
  	cJSON_AddNumberToObject(fmt,"width",        1920);   
  	cJSON_AddNumberToObject(fmt,"height",       1080);   
  	cJSON_AddFalseToObject (fmt,"interlace");   
  	cJSON_AddNumberToObject(fmt,"frame rate",   24);
  	char *  out =cJSON_Print(root);
  	printf("%s\n",out); 
  	cJSON_Delete(root);
	return out;
}

/*
{"type":"00010001","length":800,"md5":"xxx"}(pass test)
*/
unsigned long
get_c_request_package_length(char * head){
	unsigned long package_length = 0;
	cJSON * json;
	cJSON * child;
	char *out;
	json = cJSON_Parse(head);
	child = cJSON_GetObjectItem(json, "length");
	if(!child){return 0;}
	else{
		out = cJSON_Print(child);
		package_length = child->valueint;
		cJSON_Delete(json);
		free(out);
	}	        
	
	return package_length;
}

/**
	parse request package
{"type":"00010001", "list":[{"code":"EURUSD","code_type":33024}]}
*/
/*
t_base_c_request_head *
json_to_request(char * app_request_type,char * package_body){
	int i;
	for(i=0; map_request[i].func; i++){
	  if(!strcmp(map_request[i].client_request_type, app_request_type)){
	    return map_request[i].func(package_body);
	  }
	}
	return NULL;
}
*/

//调整要发送的json对象，形成{"type":"00010001","length":xxxxx}{xxxx}
//返回目标字符串长度,send_buff内容(test-pass)
unsigned long
format_json_to_client(json, send_buff, type)
  cJSON * json;
  char * send_buff;
  char * type;
{
  char * result = cJSON_Print(json);
  unsigned long package_len = strlen(result);
  //  printf("type:%s,package_len:%d,result:%s\n", type, package_len, result);

  //包装头部Json对象
  cJSON * head = cJSON_CreateObject();     
  cJSON_AddItemToObject(head, "type", cJSON_CreateString(type));
  cJSON_AddItemToObject(head, "length", cJSON_CreateNumber(package_len));
  char * head_str = cJSON_Print(head);
  
  // printf("head_str:%s\n", head_str);
  //copy head to head of package
  memcpy(send_buff, head_str, PACKAGE_HEAD_LEN);
  free(head_str);

  //copy result to body of body of package
  memcpy(send_buff+PACKAGE_HEAD_LEN, result, strlen(result));
  free(result);

  return (signed long)0;
}


//解析客户端类型，并返回json对象及其类型(test pass)
int
parse_client_request(str_request, entity, type)
     char * str_request;
     cJSON * entity;
     char * type;
{
  int result = 0;
  cJSON *child;
  char * out;

  entity = cJSON_Parse(str_request);
  child  = cJSON_GetObjectItem(entity, "type");
  out    = cJSON_Print(child);
  //printf("out:%s\n", out);
  strcpy(type, out);
  free(out);
  //printf("type:%s\n", type);
  return result;
}


//通过对象获取值-----being
int
json_get_string(json, name, value)//字符串
  cJSON * json;
  char * name;
  char * value;
{
  int result = 0;
  cJSON * child;  
  child = cJSON_GetObjectItem(json, name);
  assert(child);
  if(!child) return -1;
  else{
    printf("child!\n");
    //strcpy(value, cJSON_Print(child));
    strcpy(value, child->valuestring);
    assert(value);
  }
  return result;
}

int
json_get_int(json, name)//整数
  cJSON * json;
  char * name;
{
  char * out;
  int value;
  cJSON * child;
  child = cJSON_GetObjectItem(json, name);
  if(!child)return -1;
  else{
    out = cJSON_Print(child);
    value = child->valueint;
    cJSON_Delete(json);
    free(out);
  }
  return value;
}

float
json_get_float(json, name)//浮点数
  cJSON * json;
  char * name;
{
  float value;

  return value;
}
//通过对象获取值-----end

