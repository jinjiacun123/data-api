#include "./../include/d_login.h"
 
void 
client_request_login(sclient, head)
  int sclient;
t_base_c_request_head * head;
{
  char request[1024];

  t_login data;
  memset(&data, 0, sizeof(t_login));
  memcpy(data.head, HEADER, 4);
  data.length = sizeof(t_login) - 8;
  data.m_nType = TYPE_LOGIN;
  data.m_lKey = 3;
  data.m_nIndex = 0;
  strncpy(data.m_szUser, USERNAME, 64);
  strncpy(data.m_szPWD, PASSWORD, 64);

  memset(request, 0, sizeof(data));
  memcpy(request, &data, sizeof(data));

  send(sclient, request, sizeof(data), 0);  
  
  printf("request of login...\n");
}

//parse login
void 
client_parse_login(buff_t * my_buff){
	printf("parse login...\n");
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
