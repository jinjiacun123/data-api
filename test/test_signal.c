#include<signal.h>
#include<stdio.h>
#include<unistd.h>

void ouch(int sig){
	printf("OUT -I got signal %d\n", sig);
//	(void) signal(SIGINT, SIG_DFL);
}

int
main()
{
	//(void)signal(SIGINT, ouch);
	(void)signal(SIGUSR2, ouch);
	
	while(1){
		printf("Hello Wrld!\n");
		sleep(1);
	}
}
