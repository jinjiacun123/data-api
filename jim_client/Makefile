jim_client:main.o client.o 
	cc -g -w main.o client.o -o jim_client -L./../jim_lib -ljim
main.o:main.c
	cc -g -w -c main.c
client.o:client.c
	cc -g -w -c client.c

clean:
	rm *.o jim_market
