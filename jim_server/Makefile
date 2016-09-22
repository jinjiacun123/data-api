MYSQL := -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -lmysqlclient -lm -ljim
jim_server:server.c main.c
	cc -g -w server.c main.c -o jim_server -ljim $(MYSQL) 
server_ex:server_ex.c
	cc -g -o server_ex server_ex.c

server_ex1:server_ex1.c
	gcc -g -o server_ex1 server_ex1.c -ljim $(MYSQL)

clean:
	rm jim_server
