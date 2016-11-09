jim_proxy:proxy.o
	cc -g -w -o jim_proxy proxy.o -lpthread
proxy.o:proxy.c
	cc -g -w -c proxy.c -lpthread

clean:
	rm jim_proxy *.o
