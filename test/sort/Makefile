#CC = gcc-4.4
main:main.o cJSON.o
	$(CC) -w -g -fprofile-arcs -ftest-coverage main.o cJSON.o  -o main -pg -lm -lpthread -lz
main.o:main.c
	$(CC) -w -g -fprofile-arcs -ftest-coverage -c main.c -pg

cJSON.o:cJSON.o
	$(CC) -g -w -fprofile-arcs -ftest-coverage -c cJSON.c -pg

test_sort:test_sort.c
	$(CC) -g -w test_sort.c -o test_sort

clean:
	rm *.o main
