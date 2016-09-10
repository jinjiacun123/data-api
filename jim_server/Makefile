jim_server:server.c main.c
	cc -g -w server.c main.c -o jim_server -ljim -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient -lm 
server_ex:server_ex.c
	cc -g -o server_ex server_ex.c

server_ex1:server_ex1.c
	cc -g -o server_ex1 server_ex1.c -ljim -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient -lm 

clean:
	rm jim_server
