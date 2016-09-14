#include "./../include/d_init.h"

//初始化请求
void 
client_request_init(sclient, head)
  int sclient;
t_base_c_request_head * head;
{
  char request[1024];
	
  InitPack data ;
  memset(&data,0x00,sizeof(InitPack));
  memcpy(data.head, HEADER,4);
  data.length      = sizeof(InitPack);    
  data.m_nType     = TYPE_INIT;
  data.m_nSize     = 0;
  data.m_nIndex	= 1;

  memset(request, 0, 1024);
  memcpy(request, &data, sizeof(data));
  send(sclient, request, sizeof(data), 0);
  printf("初始化发送完毕\n");
}

//初始化解析
void 
client_parse_init(buff_t * my_buff){
	int i;
	printf("解析初始化数据包\n");
	AnsInitialData2 * pAnsInitialData = (AnsInitialData2 * )my_buff->p_res_media_h;
	char* ppData = (char*)pAnsInitialData+sizeof(AnsInitialData2);
	OneMarketData2* pOneMarketData = (OneMarketData2*)ppData;
	for(i=0; i<pAnsInitialData->m_nSize; i++){
		printf("市场%d:%s(%x), Date:%d, Count:%d, CRC:%x\n", i+1, pOneMarketData->m_szName, 
			pOneMarketData->m_nMarketType, pOneMarketData->m_lDate, 
			pOneMarketData->m_cCount, pOneMarketData->m_dwCRC);
	}
}

t_base_c_request_head *
json_to_request_of_init(char * json_str){
  t_base_c_request_head  head;

  return &head;
}
