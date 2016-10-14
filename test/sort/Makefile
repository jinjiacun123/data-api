CC = gcc-4.4
jim_sort:main.o cJSON.o
	$(CC) -w -g main.o cJSON.o  -o jim_sort -lm -lpthread
main.o:main.c
	$(CC) -w -g -c main.c

cJSON.o:cJSON.o
	$(CC) -g -w -c cJSON.c

clean:
	rm jim_sort
