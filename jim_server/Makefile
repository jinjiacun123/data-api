MYSQL := ./../jim_lib/libjim.so -I /usr/include/mysql -L/usr/lib/i386-linux-gnu/ -lmysqlclient -lm 
INCLUDE = -I ./../include
LIB = -L ./../jim_lib
jim_server:server.c main.c
	cc -g -w server.c main.c -o jim_server $(LIB) -ljim $(MYSQL) 
server_ex:server_ex.c
	cc -g -o server_ex server_ex.c

server_ex1:server_ex1.c
	gcc -g server_ex1.c -o server_ex1 $(MYSQL)
server_mul_thread:server_mul_thread.c
	gcc -g -o server_mul_thread server_mul_thread.c -ljim $(MYSQL)

clean:
	rm jim_server
