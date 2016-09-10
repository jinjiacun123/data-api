CC = gcc-4.4
LIST_O = do_json.o cJSON.o d_login.o data.o d_init.o d_heart.o d_realtime.o do_mysql.o
libjim.so:$(LIST_O)
	cc -g -w $(LIST_O) -fpic -shared -o libjim.so -lm
do_json.o:do_json.c
	cc -g -c do_json.c -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient
cJSON.o:cJSON.c 
	cc -g -w -c cJSON.c
data.o:data.c
	cc -g -w -c data.c -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient 
d_login.o:d_login.c 
	cc -g -w -c d_login.c -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient
d_init.o:d_init.c
	cc -g -w -c d_init.c -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient
d_heart.o:d_heart.c
	cc -g -w -c d_heart.c -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient
d_realtime.o:d_realtime.c
	cc -g -w -c d_realtime.c -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient
do_mysql.o:do_mysql.c
	gcc -g -w -c do_mysql.c -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient	
#gcc -o connect -g connect.c  -I /usr/local/mysql/include/ -L /usr/local/mysql/lib/ -l mysqlclient	
#gcc -g -o do_mysql -g do_mysql.c  -I /home/jim/source_code/mysql-5.5.51/include/ -L /home/jim/source_code/mysql-5.5.51/libmysql/ -l mysqlclient
#$(CC) -o do_mysql -g do_mysql.c -ldl	
#gcc -o do_mysql -g do_mysql.c  -I /home/jim/source_code/mysql-5.5.51/include/ -L /home/jim/source_code/mysql-5.5.51/libmysql/ -l mysqlclient


clean:
	rm *.o libjim.so
