#include<stdio.h>
#include "./../include/do_mysql.h"
#include "./../include/data.h"
extern db_back_t * do_mysql_select(char *);

int
main(){
	db_back_t * db_back;
	char * sql = "select * from hr_realtime";
	cJSON * root;  
	db_back = do_mysql_select(sql);

	//parse
	MYSQL_ROW row;
	int i;
	int num_fields = mysql_num_fields(db_back);
	
	while((row = mysql_fetch_row(db_back))){
		for(i=0; i< num_fields; i++){
			printf("%s ", row[i]?row[i]: "NULL");
		}
		printf("\n");
	}

	mysql_free_result(db_back);	
	
//	do_mysql_close();
  	printf("success!\n");
}
