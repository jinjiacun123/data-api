#include <stdio.h>
#include <stdlib.h>

#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>
#pragma pack (4)
/*客户端*/
//char * host_name = "127.0.0.1";
char * host_name ="192.168.1.131";//需要搜寻服务端IP地址
//char * host_name = "221.6.167.245";
int port = 8001;//代理服务器端口号
//int port = 8881;
//int port = 3333;
#define USERNAME "jrjvip_android"
#define PASSWORD "zjw_android"
#define HEADER   "ZJHR"

#define TYPE_EMPTY 0x0D03 //empty
#define TYPE_INIT  0x0101 //init
#define TYPE_LOGIN 0X0102 //login
#define TYPE_HEART 0x0905 //heart tick
#define TYPE_ZIB        0x8001 //
#define TYPE_REALTIME   0x0201 //
#define TYPE_HISTORY    0x0402 //
#define TYPE_TIME_SHARE 0x0301 //
#define TYPE_AUTO_PUSH  0x0A01 //
#define TYPE_SERVERINFO 0x0103 //
#define TYPE_DAY_CURPOS 0x020c //

/*history bean type*/
#define PERIOD_TYPE_DAY 0x0010     //周期:日
#define PERIOD_TYPE_SECOND1 0x00E0 //周期:1秒
#define PERIOD_TYPE_MINUTE1 0X00c0 //周期:1分钟
#define PERIOD_TYPE_MINUTER5 0X0030 //周期:5分钟

//单品种请求

typedef struct
{
  char m_head[4]; 
  int  m_length;  
  unsigned short m_nType;
  char  m_nIndex;   
  char  m_Not;   
  long  m_lKey;
  short m_cCodeType;
  char  m_cCode[6];
  short m_nSize;
  unsigned short m_nOption; 
}RealPack;
/*
typedef struct
{
  char head[4]; 
  int  length;
}RealPack;

typedef struct
{
  unsigned short type;
  int size;
}RealPack_ex;
*/

typedef struct
{
  unsigned short code_type;
  char code[6];
}entity_t;

typedef struct
{
  char head[4];           //ÀïÃæ·Å"2010"4žö×ÖœÚ  32 30 31 30
  int  length;           //ºóÃæÊýŸÝµÄ³€¶È£š°üµÄ³€¶ÈŒõÈ¥8£©4žö×ÖœÚ    94 00 00 00
  unsigned short m_nType;     //ÇëÇóÀàÐÍ2žö×ÖœÚ   02 01
  char m_nIndex;      //ÇëÇóË÷Òý£¬ÓëÇëÇóÊýŸÝ°üÒ»ÖÂ   1žö×ÖœÚ  00
  char m_No;            //ÔÝÊ±²»ÓÃ 1žö×ÖœÚ 00 
  long m_lKey;  //Ò»Œ¶±êÊ¶  4žö×ÖœÚ 03 00 00 00 
  short m_cCodeType; //Ö€È¯ÀàÐÍ 2žö×ÖœÚ 00  00 
  char m_cCode[6]; //Ö€È¯ŽúÂë                     6žö×ÖœÚ 00 00 00 00 00 00
  short     m_nSize;         //ÇëÇóÖ€È¯×ÜÊý                 2žö×ÖœÚ 00 00 
  unsigned short m_nOption;       //ÎªÁË4×ÖœÚ¶ÔÆë¶øÌíŒÓµÄ×Ö¶Î    2žö×ÖœÚ  00 00
  char m_szUser[64];     //ÓÃ»§Ãû     64žö×ÖœÚ  
  char m_szPWD[64];     //ÃÜÂë            64žö×ÖœÚ
}t_login;

typedef struct
{
  char m_head[4];
  int  m_length; 
  unsigned short m_nType;
  char m_nIndex;
  char m_No;
  int m_lKey;
  unsigned short m_cCodeType;
  char m_cCode[6];
  short  m_nSize;   
  unsigned short m_nOption;        
  short m_nPeriodNum;
  unsigned short m_nSize2;
  int m_lBeginPosition;
  unsigned short m_nDay;
  short m_cPeriod;
  unsigned short m_cCodeType2;   
  char m_cCode2[6];
}TeachPack;

typedef struct
{
  unsigned short m_cCodeType2;
  char m_cCode2[6];
}CodeInfo;
void send_realtime(int sclient);
void send_login(int sclient);
void send_history(int sclient);
void send_time_share(int sclient);

int main(int argc, char** argv)
{
  char buf[8192];
  char message[256];
  int socket_descriptor;
  struct sockaddr_in pin;//处理网络通信的地址
  struct hostent *server_host_name;

  char *str = "A default test string";

  if(argc<2)//运行程序时送给main函数到命令行参数个数
    {
      printf("Usage:test \"Any test string\"\n");
      printf("we will send a default test string. \n");
    }
  else
    {
      str =argv[1];
    }
  /*
   * gethostbyname()返回对应于给定主机名的包含主机名字和地址信息的
   * hostent结构指针。结构的声明与gethostaddr()中一致。*/
  if((server_host_name = gethostbyname(host_name))==0)
    {
      perror("Error resolving local host \n");
      exit(1);
    }

  bzero(&pin,sizeof(pin));
  pin.sin_family =AF_INET;
  //htonl()将主机的无符号长整形数转换成网络字节顺序
  pin.sin_addr.s_addr=htonl(INADDR_ANY);//s_addr按照网络字节顺序存储IP地址
  //in_addr 32位的IPv4地址  h_addr_list中的第一地址
  pin.sin_addr.s_addr=((struct in_addr *)(server_host_name->h_addr))->s_addr;// 跟书上不一样 必须是h_addr

  pin.sin_port=htons(port);
  /*申请一个通信端口*/
  if((socket_descriptor =socket(AF_INET,SOCK_STREAM,0))==-1){
      perror("Error opening socket \n");
      exit(1);
  }
  //pin 定义跟服务端连接的 IP 端口
  if(connect(socket_descriptor,(void *)&pin,sizeof(pin))==-1){
      perror("Error connecting to socket \n"); ////
      exit(1);
  }
  //printf("Sending message %s to server \n",str);

  /*
  while(1){
    if(send(socket_descriptor,str,strlen(str),0) == -1){
      perror("Error in send\n");
      exit(1);
    }

    printf("..sent message.. wait for response...\n");

    if(recv(socket_descriptor,buf,8192,0) == -1){
      perror("Error in receiving response from server \n");
      exit(1);
    }
    sleep(3);
  }
  */
  //send_login(socket_descriptor);
  //send realtime request
  //send_realtime(socket_descriptor);
  //send history request
  send_history(socket_descriptor);
  
  //parse reealtime request
  int length = 8;
  char * buff;
  buff = (char *)malloc(length+1);
  if(!buff){
    perror("alloc momeny error!\n");
    exit(-1);
  } 
  memset(buff, 0x00, length+1);
  int n = read(socket_descriptor, buff, length);
  printf("receive n:%d\n", n);
  if(n<=0){
    perror("error or close!\n");
    exit(-1);
  }
  if(n == length){
    length = *((int *)(buff+4));    
    printf("recive body of package length:%d\n", length);
    free(buff);
    buff = (char *)malloc(length+1);
    if(!buff){
      perror("malloc momeny error!\n");
      exit(-1);
    }
    memset(buff, 0x00, length+1);
    n = read(socket_descriptor, buff, length);
    if(n <= 0){
      perror("recive body of package error!\n");
      exit(-1);
    }
    if(n == length){
      printf("recive complete body of package!\n");
    }
  }

  printf("\n Response from server:\n\n%s\n",buf);
	
	while(1){
	printf("sleep 3 second...\n");
	sleep(3);
}

//  close(socket_descriptor);


  return (EXIT_SUCCESS);
}

/**
   head length type size entity1 entity2 ... entityn
*/
void send_realtime(int sclient)
{
  char request[1024];
  RealPack data ;
  memset(&data,0x00,sizeof(RealPack));
  memcpy(data.m_head, HEADER, 4);
  data.m_length =  sizeof(RealPack) - 8+ sizeof(entity_t)*1;
  data.m_nType = TYPE_REALTIME;
  //data.size = 1;
  data.m_nSize =1;
  data.m_nOption= 0x0080;
  
  #define SIZE 1
  entity_t entity[1];
  memset(entity, 0x00, sizeof(entity_t));
  
  /*非外汇*/
  /*
      CodeInfo ary[1];
        memset(ary, 0, sizeof(CodeInfo));
	  memcpy(ary[0].m_cCode2, "XAU", 6);
	    ary[0].m_cCodeType2 = 0x5b00;
  */

  /*外汇:pass*/
  /*
    memcpy(ary[0].m_cCode2,"AUDUSD",6);
    ary[0].m_cCodeType2 = 0xffff8100;
  */
  
  //指数:pass
  //memcpy(ary[0].m_cCode2, "2A01", 6);
  //ary[0].m_cCodeType2 = 0x1200;
  
  
  //股票:pass
  memcpy(entity[0].code, "600000", 6);
  entity[0].code_type = 0x1101;
  
  memset(request, 0, 1024);
  memcpy(request, &data, sizeof(RealPack));
  memcpy(request+sizeof(RealPack), &entity, sizeof(entity_t));
  int r = send(sclient, request, sizeof(RealPack)+sizeof(entity_t)*SIZE, 0);
  r = read(sclient, request, 1024);
  printf("recive length:%d\n", r);
}


void send_login(int sclient)
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

  assert(send(sclient, request, sizeof(data), 0));  
  
  char * buff;
  buff = (char *)malloc(1024+1);
  if(buff){
    int n = read(sclient, buff, 1024);
    if(n>0){
      printf("success login!\n");
    }
  }

  printf("request of login...\n");
}

void send_history(int sclient)
{
  char request[1024];

  TeachPack data;
  memset(&data,0x00,sizeof(TeachPack));
  memcpy(data.m_head, HEADER,4);
  data.m_length =  sizeof(TeachPack) - 8;
  data.m_nType = TYPE_HISTORY;
  data.m_nSize =1;
  data.m_nIndex= 0;
  data.m_nOption= 0x0080;
  memcpy(data.m_cCode, "600000", 6);
  data.m_cCodeType = 0x1101;

  //日线请求
  data.m_cPeriod = PERIOD_TYPE_DAY;      //请求周期类型
  data.m_lBeginPosition = 0; //请求开始位置
  data.m_nPeriodNum = 7;
  data.m_nDay = 100*data.m_nPeriodNum;//请求个数(500个)
  data.m_nSize2 =0;
  memcpy(data.m_cCode2,"600000", 6);
  data.m_cCodeType2 = 0x1101;

  //周线

  //月线

  //5分钟

  //15分钟

  //30分钟

  //60分钟

  //120分钟

  /*
    data.m_cPeriod = 0x00c0;
    data.m_lBeginPosition = 0;
    data.m_nPeriodNum = 60;
    data.m_nDay = 15*data.m_nPeriodNum;
    data.m_nSize2 = 0;
    memcpy(data.m_cCode2, "600006", 6);
    data.m_cCodeType2 = 0x1101;
  */
   

  memset(request, 0, sizeof(data));
  memcpy(request, &data, sizeof(data));
  int r = send(sclient, request, sizeof(data), 0);  
}

void send_time_share(int sclient)
{
  
}
