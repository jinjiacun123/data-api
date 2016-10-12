#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<assert.h>
#include<signal.h>
#include "./../include/cJSON.h"
//#include "./../include/do_json.h"

#define SERVER_HOST "192.168.1.131"
//#define SERVER_HOST "127.0.0.1"
//#define SERVER_PORT 8888
//#define SERVER_PORT 9999
#define MAX_HEAD_LEN 80
int client;
void send_request(int signal_num);

int
main()
{
  //	int client=0;

        signal(SIGUSR2, send_request);
	struct sockaddr_in cli;
	
	cli.sin_family = AF_INET;
	cli.sin_port = htons(SERVER_PORT);
	cli.sin_addr.s_addr = inet_addr(SERVER_HOST);

	client = socket(AF_INET, SOCK_STREAM, 0);
	if(client < 0){
		printf("socket() failrue!\n");
		return -1;
	}	

	if(connect(client, (struct sockaddr*)&cli, sizeof(cli)) < 0){
        	printf("connect() failure!\n");
		return -1;
	}

	

	printf("connect success...\n");       	
        //send_request(1);
	 
	//while(1){
	  sleep(3);
	  send_request(1);
	  //}
	
	
	
//	close(client);
	
	return 0;
}

void send_request(int signal_number){
	//send package to server
	char package[1024];
        //char * package_body = "{\"type\":\"000100040001\", \"data\":{\"code_type\":\"0x5b00\",\"code\":\"xau\",\"index\":1,\"size\":30,\"circle\":\"day\"}}";//history
	char * package_body = "{\"type\":\"000100030001\", \"data\":{\"code_type\":\"0x5b00\",\"code\":\"XAG\",\"index\":2,\"size\":200}}";//time_share
	//char * package_body = "{\"type\":\"000100010001\", \"data\":{\"code_type\":\"0x5b00\",\"code\":\"xau\"}}";//realtime
	
	int package_body_length = strlen(package_body);
	char * tmp = "{\"type\":\"000100030001\", \"length\":%d}";
        char package_head[MAX_HEAD_LEN];
	
	memset(package_head, 0, MAX_HEAD_LEN);
	assert(sprintf(package_head, tmp, package_body_length));
	
	memset(package, 0, 1024);
        memcpy(package, package_head, MAX_HEAD_LEN);
	memcpy(package+MAX_HEAD_LEN, package_body, package_body_length);	
	assert(write(client, package, MAX_HEAD_LEN+package_body_length));

	//char * package_body;
	memset(package_head, 0, MAX_HEAD_LEN);
	int ret = read(client, package_head, MAX_HEAD_LEN);
	if(ret < 0){
		printf("read err\n");
	}
	printf("back length:%d\n", ret);
	printf("read result head:%s\n", package_head);
        cJSON * head = cJSON_Parse(package_head);
	assert(head);
	unsigned int package_body_len = json_get_int(head, "length"); 
	//if(ret < body_length+MAX_HEAD_LEN){
	//继续结构
	package_body = (char*)malloc(package_body_len);
	memset(package_body, 0, package_body_len);
	
	int off = package_body_len;
	int index = 0;
	unsigned long value = 0;
	int i=0;
	int number =0;
	while(ret = read(client, package_body, off)){	
	  if(ret <0){
	    printf("read err\n");
	  }
	  if(ret == -1){
	    printf("finish!\n");
	    break;
	  }
	  printf("ret:%d\n", ret);	
	  printf("read result body:%s\n", package_body);
	  for(index=0; index<ret;index+=4){
	    number++;
	    value = (unsigned long)(package_body+index);
	    printf("number:%d,value:%ld\n", number,value);
	  }	  
	  off -= ret;
	  if(off <=0)
	    break;
	}
}
