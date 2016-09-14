MYSQL = -I /usr/include/mysql -L /usr/lib/i386-linux-gnu/ -l mysqlclient
jim_client:main.o client.o 
	cc -g -w main.o client.o -o jim_client -L./../jim_lib -ljim $(MYSQL)
main.o:main.c
	cc -g -w -c main.c $(MYSQL)
client.o:client.c
	cc -g -w -c client.c $(MYSQL)

clean:
	rm *.o jim_client
