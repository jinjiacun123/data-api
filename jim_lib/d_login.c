#include "./../include/d_login.h"
 
void request_login(int sclient, t_base_c_request_head * head){
  char request[1024];
  memset(request, 0, head->length+8);
  memcpy(request, head, head->length+8);
  send(sclient, request, head->length+8, 0);  
  free(head);
  printf("request of login...\n");
}

//µÇÂŒœâÎö
void parse_login(buff_t * my_buff){
	printf("œâÎöµÇÂŒ\n");
}


t_base_c_request_head *
json_to_request_of_login(char * json_str){
  t_base_c_request_head * head;

  t_login data;
  memset(&data,0x00,sizeof(t_login));
  memcpy(data.head, HEADER,4);
  data.length =  sizeof(t_login) -8;
  data.m_nType = TYPE_LOGIN;
  data.m_lKey = 3;
  data.m_nIndex	= 0;
  strncpy(data.m_szUser, USERNAME, 64);
  strncpy(data.m_szPWD, PASSWORD, 64);
  
  head = (t_base_c_request_head *)&data;

  return head;
}
