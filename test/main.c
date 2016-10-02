#include<stdio.h>
void fun_a();
typedef void (type_func)();

#define FUN(c) fun_##c

struct {
	int index;
	type_func * func;	
}my_fun[] = {
	{1, FUN(a)}
};

void fun_a(){
	printf("this is fun_a\n");
	type_func * func;
}

int
main(){
	printf("hello,world\n");
        my_fun[0].func();	
	return 0;
}
