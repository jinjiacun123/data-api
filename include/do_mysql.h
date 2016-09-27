#ifndef __DO_MYSQL_H__
#define __DO_MYSQL_H__
#include "cJSON.h"
#include "data.h"
//extern int do_mysql_select(char *, cJSON * json_result, int type);
extern int do_mysql_connect();
extern int do_mysql_close();
extern db_back_t * do_mysql_select(char *);
extern int do_mysql_insert(char *);
extern int do_mysql_update(char *);


/**mysql连接配置信息*/
#define MYSQL_HOST   "192.168.1.233"
//#define MYSQL_HOST "127.0.0.1"
#define MYSQL_PORT   "3306"
#define MYSQL_USER   "root"
#define MYSQL_PASSWD "123456"
//#define MYSQL_PASSWD ""
#define MYSQL_DB     "market"

#endif
