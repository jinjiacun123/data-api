CC = gcc-4.4
INCLUDE =-I ./../include
MYSQL = -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient
LIST_O = do_json.o cJSON.o d_login.o data.o d_init.o d_heart.o d_realtime.o d_auto_push.o d_time_share.o d_history.o do_mysql.o common.o
libjim.so:$(LIST_O)
	cc -g -w $(LIST_O) -fpic -shared -o libjim.so -lm
do_json.o:do_json.c
	cc -g -c do_json.c $(MYSQL) 
cJSON.o:cJSON.c 
	cc -g -w -c cJSON.c
config.o:config.c
	cc -g -w -c config.c
data.o:data.c
	cc -g -w $(INCLUDE) -c data.c $(MYSQL)
d_login.o:d_login.c 
	cc -g -w -c d_login.c $(MYSQL)
d_init.o:d_init.c
	cc -g -w -c d_init.c $(MYSQL)
d_heart.o:d_heart.c
	cc -g -w -c d_heart.c $(MYSQL)
d_realtime.o:d_realtime.c
	cc -g -w -c d_realtime.c $(MYSQL)
d_auto_push.o:d_auto_push.c
	cc -g -w $(INCLUDE) -c d_auto_push.c $(MYSQL)
d_time_share.o:d_time_share.c
	cc -g -w -c d_time_share.c $(MYSQL)
d_history.o:d_history.c
	cc -g -w -c d_history.c $(MYSQL)
do_mysql.o:do_mysql.c 
	gcc -g -w -c do_mysql.c $(MYSQL)
common.o:common.c
	gcc -g -w -c common.c $(MYSQL)
#gcc -o connect -g connect.c  -I /usr/local/mysql/include/ -L /usr/local/mysql/lib/ -l mysqlclient	
#gcc -g -o do_mysql -g do_mysql.c  -I /home/jim/source_code/mysql-5.5.51/include/ -L /home/jim/source_code/mysql-5.5.51/libmysql/ -l mysqlclient
#$(CC) -o do_mysql -g do_mysql.c -ldl	
#gcc -o do_mysql -g do_mysql.c  -I /home/jim/source_code/mysql-5.5.51/include/ -L /home/jim/source_code/mysql-5.5.51/libmysql/ -l mysqlclient


clean:
	rm *.o libjim.so
