#include<stdio.h>
#include "./../include/cJSON.h"
#include "./../include/data.h"
#include<mysql.h>
#include<assert.h>
void test_general_sql_of_history();

int
main(){
  test_general_sql_of_history();
}

void test_general_sql_of_history(){
  char * type = "000100040001";
  char * json_str = "{\"type\":\"000100040001\",\"data\":{\"code_type\":\"0x5b00\",\"code\":\"XAU\",\"index\":1,\"size\":100}}";
  cJSON * json;
  char * template_sql = "select * from hr_history_day_%s limit %d,%d";
  char * sql_buffer[SQL_BUFF_MAX_LEN];
  json = cJSON_Parse(json_str);
  
  memset(sql_buffer, 0, SQL_BUFF_MAX_LEN);
  if(json){
    int ret = general_sql_of_history(type, json, template_sql, sql_buffer);
    printf("ret:%d sql_buffer:%s\n", ret, sql_buffer);
  }
}
