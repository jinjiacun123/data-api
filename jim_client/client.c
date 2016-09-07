#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "./../include/common.h"
#include "./../include/data.h"

extern int sclient;
char tmp_buff[MAX_BUFF];
unsigned int tmp_buff_len;

void init_socket(){
	int c_len = 0;
	struct sockaddr_in cli;
	
	cli.sin_family = AF_INET;
	cli.sin_port = htons(REMOTE_PORT);
	cli.sin_addr.s_addr = inet_addr(REMOTE_SERVER);

	sclient = socket(AF_INET, SOCK_STREAM, 0);
	if(sclient < 0){
		printf("socket() failrue!\n");
		return -1;
	}	

	if(connect(sclient, (struct sockaddr*)&cli, sizeof(cli)) < 0){
        	printf("connect() failure!\n");
		return -1;
	}       	
}

void send_socket(){
	
}

void recv_socket(buff_t * my_buff){
	int ret_count = 0;	
	while((ret_count = recv(sclient, my_buff->buff+my_buff->buff_off, MAX_BUFF, 0)) > 0){
			if(my_buff->buff_len == 0){			
			my_buff->p_res_h = (p_response_header)my_buff->buff;
			if(my_buff->p_res_h->length + sizeof(struct response_header)  == ret_count)
			{
				/*
				|---------------------------------------------|
				1.һ��ȫ�������굱ǰ�����ݰ����ҽ�Ϊһ�����ݰ�;
				*/
				printf("1.һ��ȫ�������굱ǰ�����ݰ����ҽ�Ϊһ�����ݰ�\n");
				my_buff->is_direct = true;
				my_buff->buff_parse_off = 8;
				my_buff->buff_len = ret_count;
				//������ǰ��
//				parse(my_buff);
				//�˳�
				break;
			}
			else if(my_buff->p_res_h->length + 8  > ret_count)//��ǰ��δ��������
			{
				/*
				|----------------------------------------------
				2.һ�ν��ܲ��굱ǰҪ�������ݰ�����Ҫ���̼��λ��߶��ν��ܲ�����;
				*/
				printf("һ�ν��ܲ��굱ǰҪ�������ݰ�����Ҫ���̼��λ��߶��ν��ܲ�����");
				my_buff->buff_off = ret_count;
			}
			else if(my_buff->p_res_h->length + 8  < ret_count)
			{
				/*
				|------------------------------------------------|----
				|----------------------|-------------------------|...
				5.һ�ν������ݰ���������ֹһ��Ԫ������.
				*/
				printf("5.һ�ν������ݰ���������ֹһ��Ԫ������.\n");
				//�����´�Ԫ���ݵ���Ϣ
				my_buff->buff_len = my_buff->p_res_h->length + 8;
				memset(tmp_buff, 0, tmp_buff_len);
				tmp_buff_len = ret_count - my_buff->buff_len;				
				memcpy(tmp_buff, my_buff->buff+my_buff->buff_len, tmp_buff_len);
				memset(tmp_buff+my_buff->buff_len, 0, tmp_buff_len);
				my_buff->is_direct = true;
				my_buff->buff_parse_off = 8;
//				parse(my_buff);

				//�ж��Ƿ��ɽ���
				while(tmp_buff_len>8)//�ɽ���Ԫ��ͷ��
				{	
					//�ж��Ƿ��ɽ���Ԫ��
					p_response_header p_tmp = (p_response_header)tmp_buff;
					
					int len = p_tmp->length+8;

					if(tmp_buff_len>=len){		
						//�������ݵ�������������
						memset(my_buff->buff, 0, my_buff->buff_len);
						memcpy(my_buff->buff, tmp_buff , len);
						my_buff->buff_len = len;
						//ת��ʣ�µĴ�ת�ƵĻ�����
						memcpy(tmp_buff, tmp_buff+len, tmp_buff_len-len);
						tmp_buff_len = tmp_buff_len-len;
						//����Ԫ��
						my_buff->buff_parse_off = 8;
						my_buff->is_direct = true;
						//��������
						//parse();
					}
				}
				
				//��������ʣ�࣬���ݵ��´μ�������
				if(tmp_buff_len>0){
					my_buff->buff_off = tmp_buff_len;
					memset(my_buff->buff, 0, my_buff->buff_len);
					memcpy(my_buff->buff, tmp_buff, tmp_buff_len);
					memset(tmp_buff, 0, tmp_buff_len);
				}

				break;
			}
		}

	}
}

void close_socket(){
	close(sclient);
}
