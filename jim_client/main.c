/**
#include <stdio.h>
#nclude <unistd.h>

extern char *optarg;   //选项的参数指针
extern int optind,    //下一次调用getopt的时，从optind存储的位置处重新开始检查选项。 
extern int opterr,   //当opterr=0时，getopt不向stderr输出错误信息。
extern int optopt;   //当命令行选项字符不包括在optstring中或者选项缺少必要的参数时，该选项存储在optopt中，getopt返回'？’、
int getopt(int argc, char * const argv[], const char *optstring);

使用：
$ ./a.out -Wall -o hello.c
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdbool.h>
#include "./../include/data.h"


extern void request_server();

buff_t client_buff={
	{NULL,0,0,	0,NULL,0,	NULL,NULL,	0,true}
};
/*int index[BUFF_MAX_LEN] = {0};*/
int sclient = 0;

int debug_level = 0;

#define _AUTHOR "Late Lee"
#define _VERSION_STR "1.0"
#define _DATE ""

// 默认打印error等级
enum
{
    MSG_ERROR = 0,
    MSG_WARNING,
    MSG_INFO,
    MSG_DEBUG,
    MSG_MSGDUMP,
    MSG_EXCESSIVE, 
};

void ll_printf(int level, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

void ll_printf(int level, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    if (debug_level >= level)
    {
	#ifdef CONFIG_DEBUG_SYSLOG
        if (wpa_debug_syslog) {
            vsyslog(syslog_priority(level), fmt, ap);
        } else {
	#endif /* CONFIG_DEBUG_SYSLOG */
        //debug_print_timestamp();
	#ifdef CONFIG_DEBUG_FILE
        if (out_file) {
            vfprintf(out_file, fmt, ap);
            fprintf(out_file, "\n");
        } else {
	#endif /* CONFIG_DEBUG_FILE */
        vprintf(fmt, ap);
        printf("\n");
	#ifdef CONFIG_DEBUG_FILE
        }
	#endif /* CONFIG_DEBUG_FILE */
	#ifdef CONFIG_DEBUG_SYSLOG
        }
	#endif /* CONFIG_DEBUG_SYSLOG */
    }
    va_end(ap);
}

void show_version(char* name)
{
    printf("%s by %s, version: %s\n", name, _AUTHOR, _VERSION_STR);
}

void usage(char* name)
{
	show_version(name);

	printf("    -h,    short help\n");
	printf("    -v,    show version\n");
	printf("    -d,    debug level\n");
	printf("    -r,    request socket\n");
    
    //exit(0);
}

const char* my_opt = "hOo:W:r:c:d:";

void sighandler(int);

/*init*/
void init(){
	//init socket
        init_socket();

	//init signal deal
	(void)signal(SIGUSR1, sighandler);
}

int 
main(int argc, char *argv[])
{
	int c;
	const char* p1 = NULL;
	const char* p2 = NULL;
	const char* p3 = NULL;
	
	init();

    while(1)
    {
	c = getopt(argc, argv, my_opt);
	printf("optind: %d\n", optind);
	if (c < 0)
	{
	    break;
	}
	printf("option char: %x %c\n", c, c);
	switch(c)
	{
	case 'd':
	        debug_level = atoi(optarg);
	        printf("debug level: %d\n", debug_level);
	        break;
	case 'O':
	        printf("optimization flag is open.\n\n");
	        break;
	case 'o':
                printf("the obj is: %s\n\n", optarg);
                p1 = optarg;
                break;
        case 'W':
                printf("optarg: %s\n\n", optarg);
                p2 = optarg;
                break;
	case 'r':
		printf("socket request\n\n", optarg);
		p2 = optarg;	
		switch(atoi(p2)){
			case 1:
				{
					printf("request of login\n");
					request_login(sclient);
				}
				break;
		}	
		break;
	case 'c':
		printf("close socket\n\n", optarg);
		p2 = optarg;
		break;
        case ':':
                fprintf(stderr, "miss option char in optstring.\n");
                break;
        case '?':
        case 'h':
        default:
                usage(argv[0]);
                break;
                //return 0;
        }
    }
    if (optind == 1)
    {
        usage(argv[0]);
    }

	while(1){
		sleep(2);
		recv_socket();
	}
	//pause();	 
    
    ll_printf(MSG_ERROR, "p1: %s p2: %s\n", p1, p2);
    
    return 0;
}

//read file
int
my_read(){
	int input_fd;
    	ssize_t ret_in, ret_out;    /* Number of bytes returned by read() and write() */
    	char buffer[2];      /* Character buffer */ 
	int ret;
 
	/* Create input file descriptor */
	input_fd = open ("type", O_RDONLY);
	if (input_fd == -1) {
		perror ("open");
	        return 2;
	}

	/* Copy process */
	if((ret_in = read (input_fd, &buffer, 2)) > 0){
		ret = atoi(buffer);
	}
	close (input_fd);
	return ret;
}

void sighandler(int signum)
{
   	/**
	1-login 
	2-init
	3-heart	
	4-
	5-
	6-
	*/
	int type=0;
	if (signum == SIGUSR1)
	{
		type = my_read();
		switch(type){
			case 1:
				printf("request of login\n");
				request_server(sclient, TYPE_LOGIN);
				break;
			case 2:
				printf("request of init\n");
				break;
			case 3:
				printf("request of heart\n");
				break;
			case 4:				
				break;
			case 5:
				break;
		}
	}
}


