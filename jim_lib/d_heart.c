#include"./../include/d_heart.h"

/**
	����
*/
void 
client_request_heart(sclient, head)
  int sclient;
t_base_c_request_head * head;
{
	char request[1024];
	
	TestSrvData2 data ;
	memset(&data,0x00,sizeof(TestSrvData2));
	memcpy(data.head, HEADER,4);
	data.length      = sizeof(TestSrvData2) -8;    
	data.m_nType     = TYPE_HEART_EX;
	data.m_nIndex	= 1;

	memset(request, 0, 1024);
	memcpy(request, &data, sizeof(data));
	send(sclient, request, sizeof(data), 0);
	printf("�������������\n");
}

void 
client_parse_heart(buff_t * my_buff){
	printf("������������\n");
}


/*******��Ϊ������������***************/

t_base_c_request_head *
json_to_request_of_heart(char * json_str){
  t_base_c_request_head * head;

  return head;
}
