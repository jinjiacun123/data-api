#include <stdlib.h>  
#include <stdio.h>  
//#include <dlfcn.h>
#include<mysql.h>
//#include "./../include/mysql.h"  
#include "./../include/do_mysql.h"
#include "./../include/cJSON.h"

static MYSQL * conn_ptr;

//connection
int
do_mysql_connect(){
  conn_ptr = mysql_init(NULL);
  if(!conn_ptr){
    printf("mysql_init failed\n");
    return EXIT_FAILURE;
  }
  conn_ptr = mysql_real_connect(conn_ptr, MYSQL_HOST, MYSQL_USER, MYSQL_PASSWD, MYSQL_DB, 0, NULL, 0);
  if(conn_ptr){
    printf("connection success\n");
  }
  else{
    printf("connection failed\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int
do_mysql_close(){
  mysql_close(conn_ptr);
}

//查询
/*
int 
do_mysql_select(char * sql, cJSON * json_result, int type){
  do_mysql_connect();

  if (mysql_query(conn_ptr, sql)){
    printf("select error\nx");
  }
  
  MYSQL_RES *result = mysql_store_result(conn_ptr);
  
  if (result == NULL){
    printf("select result error\n");
  }

  int num_fields = mysql_num_fields(result);

  MYSQL_ROW row;
  int i;
  
  while ((row = mysql_fetch_row(result))){ 
    for(i = 0; i < num_fields; i++){ 
      printf("%s ", row[i] ? row[i] : "NULL"); 
    } 
    printf("\n"); 
  }
  
  mysql_free_result(result);

  do_mysql_close();
  return 0;
} 
*/ 
//查询返回结果集
db_back_t *
do_mysql_select(sql)
     char * sql;
{
  do_mysql_connect();

  if (mysql_query(conn_ptr, sql)){
    printf("select error\nx");
  }
  
  MYSQL_RES *result = mysql_store_result(conn_ptr);
  
  do_mysql_close();
  
  return result;
}
  
//插入 
int
do_mysql_insert(char * sql){
  //do_mysql_connect();
  
  if(mysql_query(conn_ptr, sql)){
    printf("insert error\n");
  }

  //do_mysql_close();
  return 0;
}

//修改
int
do_mysql_update(char * sql){
  do_mysql_connect();
  
  if(mysql_query(conn_ptr, sql)){
    printf("update error\n");
  }

  do_mysql_close();
  return 0;
}


/*
int main(){
  //测试插入
  
  char * sql = "insert into hr_realtime(code_type, code, price) values('0x00','0x00',1)";
  int result = do_mysql_insert(sql);
  if(result == 0){
    printf("插入成功");
  }

  //修改
  char * sql = "update hr_realtime set price=2";
  int result = do_mysql_update(sql);

  //查询
  sql = "select * from hr_realtime";
  cJSON * root;
  do_mysql_select(sql, root, 1);
}
*/
