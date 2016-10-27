#CC = gcc-4.4
SOURCE_LIST = main.o cJSON.o sort.c market.o
CCFLAGS = -fno-omit-frame-pointer
main:$(SOURCE_LIST)
	$(CC) -w -g -fprofile-arcs -ftest-coverage $(SOURCE_LIST)  -o main -pg -lm -lpthread -lz -lprofiler
main.o:main.c
	$(CC) -w -g -fprofile-arcs -ftest-coverage -c main.c -pg -lprofiler

cJSON.o:cJSON.c
	$(CC) -g -w -fprofile-arcs -ftest-coverage -c cJSON.c -pg -lprofiler

sort.o:sort.c
	$(CC) -g -w -fprofile-arcs -ftest-coverage -c sort.c -pg -lprofiler

market.o:market.c
	$(CC) -g -w -fprofile-arcs -ftest-coverage -c market.c -pg -lprofiler

test_sort:test_sort.c
	$(CC) -g -w test_sort.c -o test_sort

clean:
	rm *.o main
