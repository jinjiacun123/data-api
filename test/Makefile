CC = gcc-4.4
libmyadd.so:so.c
	$(CC) -shared -o libmyadd.so so.c
test_so:test_so.c
	$(CC) -ldl -o test_so test_so.c

test_json:test_json.c
	$(CC) -g -o test_json test_json.c  -ljim -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient -lm     
test_realtime:test_realtime.c
	$(CC) -g -o test_realtime test_realtime.c -ljim  -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient -lm     
test_history:test_history.c
	$(CC) -g -o test_history test_history.c -ljim -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient -lm

test_do_mysql:test_do_mysql.c
	$(CC) -g -o test_do_mysql test_do_mysql.c -ljim -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient

test_data:test_data.c
	$(CC) -g -o test_data test_data.c -ljim -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient

test_server:test_server.c
	$(CC) -g -o test_server test_server.c ./../jim_lib/libjim.so -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient

test_pthread:test_pthread.c
	$(CC) -g -o test_pthread test_pthread.c -lpthread

test_struct:test_struct.c
	$(CC) -g -o test_struct test_struct.c

test_proxy:test_proxy.c
	$(CC) -g -o test_proxy test_proxy.c

test_poll:test_poll.c
	$(CC) -g -o test_poll test_poll.c

test_byte:byte.c
	$(CC) -g -o test_byte byte.c

test_point_param:test_point_param.c
	$(CC) -g -o test_point_param test_point_param.c

test_sort:test_sort.c
	$(CC) -g -o test_sort test_sort.c -lpthread
