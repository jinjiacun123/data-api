#include<stdio.h>
#include "./../include/common.h"
#include "./../include/cJSON.h"
#include "./../include/data.h"
void test_get_c_request_package_length();
void test_json_to_request();
void test_parse_client_request();
void test_format_json_to_client();
void test_json_get_string();

extern unsigned long get_c_request_package_length(char * head);
extern t_base_c_request_head * json_to_request(char * app_request_type, char * package_body);
extern int parse_client_request(char *, cJSON *, char *);
unsigned long format_json_to_client1(cJSON *, char *, char *);



int
main(){
       // test_get_c_request_package_length();
       //test_json_to_request();
//  test_parse_client_request();
  //test_format_json_to_client();
    //test_json_get_string();
  /*
  char * old_str = "aa";
  char * new_str = "aa";
  int result = strcmp(old_str, new_str);
  if(result)printf("this is test\n");
  printf("result %d\n", result);
	   	test_json_to_request();
        printf("please entre any key to continue...\n");
  */
}

/*
void test_get_c_request_package_length(){
	char * head = "{\"type\":\"00010001\", \"length\":800}";        
	unsigned long length = 0;
	length = get_c_request_package_length(head);
	printf("length:%d\n", length);
}
*/

/*
void test_json_to_request(){
	char * app_request_type = "00010002";
	char * package_body = "{\"code\":\"EURUSD\",\"type\":33024}";
	t_base_c_request_head * head;
	head = json_to_request(app_request_type, package_body);
}
*/


void test_parse_client_request(){
  char * str_request = "{\"type\":\"0001001001\",\"entity\":{\"code_type\":\"000\",\"code\":\"001\"}}";
  
  char type[CLI_REQ_TYPE_LEN];
  cJSON * entity;
  
  memset(type, 0 ,CLI_REQ_TYPE_LEN);
  int ret = parse_client_request(str_request, entity, type);
  printf("result %d\n", ret);
  printf("type is %s\n", type);
}

void test_format_json_to_client(){
  char * out = "{\"type\":\"000100010001\",\"entity\":{\"aa\":\"aa_value\"}}";
  cJSON *json;
  char send_buff[SEND_BUFF_LEN];
  char * type="000100010001";
  json = cJSON_Parse(out);

  memset(send_buff, 0, SEND_BUFF_LEN);
  unsigned long send_length = format_json_to_client(json, send_buff, type);
  printf("send_buff head:%s\n", send_buff);
  printf("send_buff body:%s\n", send_buff+PACKAGE_HEAD_LEN);
}

void test_json_get_string(){
	char * str = "{\"type\":\"000100010001\"}";
	cJSON * json;
	char type[100];

	json = cJSON_Parse(str);
	int ret = json_get_string(json, "type", type);	
	printf("ret:%d\n", ret);
	printf("type:%s\n", type);
	/*
	char * tmp;
	cJSON * child = cJSON_GetObjectItem(json, "type");
	if(child){
		tmp = cJSON_Print(child);
		printf("tmp:%s\n", tmp);
		free(tmp);
	}
	*/
}
