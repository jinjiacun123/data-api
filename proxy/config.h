#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <errno.h>
#include <poll.h>
#include <assert.h>

#ifndef _SCO_DS
#include <fcntl.h>
#endif
#include <signal.h>
#define LISTENMAX    1024
#define PROXY_SERVER "127.0.0.1"
#define PROXY_PORT   8001
#define SERVER       "221.6.167.245"
//#define SERVER "127.0.0.1"
#define SERVER_PORT  8881
//#define SERVER_PORT 8000
#define USERNAME "jrjvip_android"
#define PASSWORD "zjw_android"
#define HEADER   "ZJHR"

#define TYPE_EMPTY 0x0D03
#define TYPE_INIT  0X0101
#define TYPE_LOGIN 0X0102
#define TYPE_HEART 0X0905
#define TYPE_ZIB 0X8001
#define TYPE_REALTIME 0X0201
#define TYPE_HISTORY 0X0402
#define TYPE_TIME_SHARE 0X0301
#define TYPE_AUTO_PUSH 0X0A01
#define TYPE_SERVERINFO 0X0103
#define TYPE_DAY_CURPOS 0X020C

//history circle type
#define PERIOD_TYPE_DAY 0x0010
#define PERIOD_TYPE_SECOND1 0X00E0
#define PERIOD_TYPE_MINUTE1 0x00C0
#define PERIOD_TYPE_MINUTERS 0x0030

//response---------------------------------------------------------
#define RESPONSE_DEAL(param) deal_response_of_##param
//response of server
typedef struct{
  //header
  char header_name[4];
  int  body_len;
  
  //body
  unsigned short type;//data type
  char data;
}response_s_t;
typedef response_s_t * p_response_s_t;
//response--------------------------------------------------------

//request---------------------------------------------------------
#define REQUEST_T(param) request_##param##_t
#define REQUEST_DEAL(param) deal_request_of_##param
typedef struct{
  char index;
  char no;
  long key;
  short code_type;
  char  code[6];
  short size;
  unsigned short option;
  char username[64];
  char password[64];
}request_login_t;

typedef struct{
  char index;
  char operator;
}request_heart_t;
  
typedef struct{
  //header
  char header_name[4];
  int body_len;
  
  //body
  unsigned short type;
}request_s_t;

request_s_t request_data[] = {
  {HEADER, sizeof(REQUEST_T(login))+4, TYPE_LOGIN},//login
  {HEADER, sizeof(REQUEST_T(heart))+4, TYPE_HEART},//heart
  {HEADER,0, TYPE_REALTIME},//realtime
  {HEADER,0, TYPE_AUTO_PUSH},//auto_push
  {HEADER,0, TYPE_TIME_SHARE},//time_share
  {HEADER,0, TYPE_HISTORY},//history

  //empty
  {}
};
//request-----------------------------------------------------------
#endif
