#CC = gcc-4.4
SOURCE_LIST = main.o cJSON.o sort.c market.o
main:$(SOURCE_LIST)
	$(CC) -w -g -fprofile-arcs -ftest-coverage $(SOURCE_LIST)  -o main -pg -lm -lpthread -lz
main.o:main.c
	$(CC) -w -g -fprofile-arcs -ftest-coverage -c main.c -pg

cJSON.o:cJSON.c
	$(CC) -g -w -fprofile-arcs -ftest-coverage -c cJSON.c -pg

sort.o:sort.c
	$(CC) -g -w -fprofile-arcs -ftest-coverage -c sort.c -pg

market.o:market.c
	$(CC) -g -w -fprofile-arcs -ftest-coverage -c market.c -pg

test_sort:test_sort.c
	$(CC) -g -w test_sort.c -o test_sort

clean:
	rm *.o main
