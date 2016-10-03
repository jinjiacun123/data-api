#include "config.h"

int g_iCltNum=0;
extern int errno;
void WriteErrLog(const char * i_sFormat,...);
int PassiveSock();
//char *cftime(char *s, const char *format, time_t *time);
int ConnectSock();
void deal_proxy(int proxyClientSocketId, int clientSocketId);
//init login
int init_login(int proxy_client_fd);
//send heart to server
int send_heart_to_server(int proxy_client_fd, int * heart_times);
//from client to server
int deal_from_client_to_server(int proxy_client_fd, const char * buf, unsigned long buf_len);
//from server to client
int deal_from_server_to_client(int client_fd, const char * buf, unsigned long buf_len);

//request of realtime
int deal_request_of_realtime();
//request of auto_push
int deal_request_of_auto_push();
//request of time_share
int deal_request_of_time_share();
//request of history
int deal_request_of_history();
//request of zib
int deal_request_of_zlib();

//response of realtime
int deal_response_of_realtime();
//response of auto_push
int deal_response_of_auto_push();
//response of time_share
int deal_response_of_time_share();
//response of history
int deal_response_of_history();
//response of zlib
int deal_response_of_zlib();

void catchcld(int sig)
{
  int  iStat;
  /*等待进程结束*/
  wait(&iStat);
  /*子进程结束后进程数量减一*/
  g_iCltNum--;
}

/*
函数：tcp代理服务
功能：创建一个Socket服务，等待客户端连接，在客户段和服务器端建立代理连接
参数：       
        代理服务端口号
        服务器地址
        服务器端口号
        允许最大连接数
*/
main(int argc,char *argv[])
{
  int proxyClientSocketId;
  int proxyServerSocketId;
  int clientSocketId;
  struct sockaddr_in sAddrIn;
#ifdef _SCO_DS
  int iAddrLen;
#else
  size_t iAddrLen;
#endif
  pid_t iChildPid;
               
  char cService[20];
  char cHost[20];
  char cPort[20];
  char cNum[20];
  int  iPNum=10;

  //捕捉子进程结束的信号
  signal(SIGCHLD,catchcld);
  //忽略管道信号
  signal(SIGPIPE,SIG_IGN);
       
  //将进程设置成精灵进程 交由init管理
  if(fork()>0)
    exit(0);
  setsid();
       
  //创建代理服务端套接字
  proxyServerSocketId = PassiveSock();
  if(proxyServerSocketId<0){
    WriteErrLog("创建sock在服务%s失败/n",cService);
    exit(-1);
  }
  while(1){        
    signal(SIGCHLD,catchcld);
    signal(SIGPIPE,SIG_IGN);
    errno=0;
    iAddrLen = sizeof(sAddrIn);
               
    //等待客户端的连接
    if((clientSocketId = accept(proxyServerSocketId,(struct sockaddr *)&sAddrIn,&iAddrLen))<(long)0){
      WriteErrLog("Accept error (%d):%s/n",errno,strerror(errno));
      //连接出错 1 秒后继续服务
      sleep(1);
      continue;
    }
    WriteErrLog("accept client connection!\n");

    //进程数量大于iPNum时 1秒后继续服务
    if(g_iCltNum > iPNum - 1){
      WriteErrLog("g_iCltNum is too much\n");
      close(clientSocketId);
      sleep(1); 
      continue;
    }
    
    WriteErrLog("create child process!\n");
    //创建进程
    if((iChildPid=fork()) < 0){
      WriteErrLog("Fork error :%s!!/n",strerror(errno));
      close(clientSocketId);
      //出错1秒后 继续服务
      sleep(1); 
      continue;
    }
    //父进程继续等待客户端的连接 进程数量加一
    if(iChildPid>0){ 
      close(clientSocketId);
      g_iCltNum++;continue; 
    }
    close(proxyServerSocketId);

    //连接到服务器
    proxyClientSocketId = ConnectSock();
    if (proxyClientSocketId < 0){
      WriteErrLog("连接(%s:%s)失败(%d):%s/n",
		  cHost,
		  cPort,
		  errno,
		  strerror(errno));
      close(clientSocketId);
      exit(0);
    }
    WriteErrLog("connection to server!\n");
    
    //init login
    assert(init_login(proxyClientSocketId) != 0);

    deal_proxy(proxyClientSocketId, clientSocketId);
  }
} 

void WriteErrLog(const char *i_sFormat,...)
{
  va_list args;
  char  *sLogFile="/home/jim/source_code/data_api/test/proxy/err.log";
  FILE  *fLogFile;
  time_t tWriteTime;
  char  sWriteTime[20];
  pid_t ThisPid;

  ThisPid=getpid();
  time(&tWriteTime);
  //  cftime(sWriteTime,"%Y/%m/%d %H:%M:%S",&tWriteTime);
  //sLogFile=getenv("ERRFILE");
  // printf("log:%s\n", sLogFile);
  

  if(sLogFile != NULL) 
    fLogFile=fopen(sLogFile,"a+");
  
  if(sLogFile != NULL && fLogFile !=NULL)
    fprintf(fLogFile,"[%6d]%s : ",ThisPid,sWriteTime);
  va_start(args,i_sFormat);
  if(sLogFile != NULL && fLogFile !=NULL)
    vfprintf(fLogFile,i_sFormat,args);
  va_end(args);
  if(sLogFile != NULL && fLogFile != NULL) fclose(fLogFile);
}

/*        passivesock        allocate & bind a server socket using tcp or ucp */
int 
PassiveSock()
{
  struct servent *pse;
  struct protoent *ppe;
  struct sockaddr_in sin;
  int s,type,optval,optlen;

  memset(&sin,0,sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;//?(u_long)0x00000000
  sin.sin_port = htons(PROXY_PORT);

  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s<0)
    WriteErrLog("Can't create socket/r/n");

  optval = 1;
  optlen = sizeof(optval);
  //  if(setsockopt(s,SOL_SOCKET,SO_REUSEPORT,(char *)&optval,optlen) == -1)
  //  WriteErrLog("Warning:setsockopt error:%s/n",strerror(errno));


  /*        bind the socket        */
  if (bind(s,(struct sockaddr *)&sin,sizeof(sin))<0)
    WriteErrLog("Warning:Can't bind to %s port: %s!!\n", "127.0.0.1", PROXY_PORT);

  if(listen(s, LISTENMAX)<0)
    WriteErrLog("Warning:Can't listen on port!!:%s/n",strerror(errno));
  WriteErrLog("SockRecvServer:Listening on port %s.......\n","8001");

  return s;
}

//proxy client connect to server
int
ConnectSock()
{
  int proxy_client_fd;
  struct sockaddr_in proxy_client_addr;

  proxy_client_addr.sin_family = AF_INET;
  proxy_client_addr.sin_addr.s_addr = inet_addr(SERVER);
  proxy_client_addr.sin_port = htons(SERVER_PORT);

  if((proxy_client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    WriteErrLog("error proxy_client_addr!\n");
    exit(1);
  }

  if(connect(proxy_client_fd, (void *)&proxy_client_addr ,sizeof(proxy_client_addr)) == -1){
    WriteErrLog("error proxy connection to server!\n");
    exit(1);
  }

  return proxy_client_fd;
}

void deal_proxy(int proxyClientSocketId, int clientSocketId)
{
  struct pollfd client[2];
  unsigned char cDataBuf[1024];
  int n;
  fd_set rset;
  int iRet=0;
  int maxi=0;
  int nready;
  int i;
  int heart_times = 0;
    
  client[0].fd = proxyClientSocketId;
  client[0].events = POLLIN;
  client[1].fd = clientSocketId;
  client[1].events = POLLIN;
  

  //init server's login
  assert(init_login(proxyClientSocketId) != 0);

  while(1){
    nready = poll(client, 2, 5000);    
    
    if(nready == 0){//timeout
      //send heart to server
      assert(send_heart_to_server(proxyClientSocketId, &heart_times) != 0);
    }

    //recive server info
    if(client[0].fd > 0){
      if(client[0].revents & (POLLIN|POLLERR)){
	if((n = read(client[0].fd, cDataBuf, 1024))<0){
	  if(errno == ECONNRESET){
	    close(client[0].fd);
	    client[0].fd = -1;
	  }
	  else{
	    WriteErrLog("read client err!\n");
	  }
	}
	else if(n == 0){
	  WriteErrLog("server connection close\n");
	  exit(-1);
	}
	else if(n >0){
	  //send to client
	  WriteErrLog("send to client\n");
	  write(client[1].fd, cDataBuf, n);
	}
      }  
    }
    
    //recive client info
    if(client[1].fd > 0){
      if(client[1].revents & (POLLIN|POLLERR)){
	if((n = read(client[1].fd, cDataBuf, 1024))<0){
	  if(errno == ECONNRESET){
	    close(client[1].fd);
	    client[1].fd = -1;
	  }
	  else{
	    WriteErrLog("read client err!\n");
	  }
	}
	else if(n == 0){
	  WriteErrLog("client connection close\n");
	  exit(-1);
	}
	else if(n >0){
	  //send to server
	  WriteErrLog("send to server\n");
	  write(client[0].fd, cDataBuf, n);
	}
      }  
    }  
    
  }
}

//send heart to server
int 
send_heart_to_server(int proxy_client_fd, int * heart_times)
{
  //send heart to server
  char * package;
  int request_len  = sizeof(request_s_t);
  int package_len = request_len +sizeof(REQUEST_T(heart));
  package = (char *)malloc(package_len + 1);
  memset(package, 0x00, package_len);

  request_s_t * data;
  data = (request_s_t *)package;
  memcpy(data->header_name, HEADER, 4);
  data->body_len = package_len -8;
  data->type = TYPE_HEART;
  
  REQUEST_T(heart) * data_ex;
  data_ex = (REQUEST_T(heart) *)(package + request_len);
  data_ex->index = 1;
  
  write(proxy_client_fd, package, package_len);

  free(package);
  return 0;
}

//init login
int 
init_login(int proxy_client_fd)
{
  char * package;
  int request_len = sizeof(request_s_t);
  int package_len = request_len + sizeof(REQUEST_T(login));
  package = (char *)malloc(package_len+1);  
  memset(package, 0x00, package_len);

  //request login
  request_s_t * data;
  data = (request_s_t *)package;
  memcpy(data->header_name, HEADER, 4);
  data->body_len = package_len - 8;
  data->type = TYPE_LOGIN;
  
  REQUEST_T(login) * data_ex;
  data_ex = (REQUEST_T(login) *)(package + request_len);
  data_ex->key = 3;
  data_ex->index = 0;
  strncpy(data_ex->username, USERNAME, 64);
  strncpy(data_ex->password, PASSWORD, 64);

  write(proxy_client_fd, package, package_len);
  free(package);
  WriteErrLog("send login info!\n");

  //response login
  p_response_s_t  p_response_s;
  char * buff;
  int buff_len = 8;
  unsigned short type = 0x00;
  assert(buff = (char *)malloc(buff_len+1));
  memset(buff, 0x00, buff_len);
  int n = read(proxy_client_fd, buff, buff_len);
  WriteErrLog("recive login info!\n");
  if(n == 8){
    p_response_s = (p_response_s_t)buff;
    //parse head
    if(strcmp(p_response_s->header_name, HEADER)){
      WriteErrLog("recive login head of info err!\n");
      exit(-1);
    }
    buff_len = p_response_s->body_len;
    free(buff);
    assert(buff = (char *)malloc(buff_len+1));
    
    memset(buff, 0x00, buff_len+1);
    n = read(proxy_client_fd, buff, buff_len);
    if(n != buff_len){
      WriteErrLog("recive login body's length of info err!\n");
      exit(-1);
    }
    
    type = (unsigned short *)buff;
    if(type != TYPE_LOGIN){
      WriteErrLog("recive login body's type of info err!\n");
      exit(-1);
    }

    return 0;
  }
  else{
    WriteErrLog("recive login info err!\n");
    exit(-1);
  }
  return -1;
}

//from client to server
int
deal_from_client_to_server(proxy_client_fd, buf, buf_len)
     int proxy_client_fd;
     const char * buf;
     unsigned long buf_len;
{
  //parse client's request
  unsigned short type = 0;
  //deal request of client to request of server
  if(type == TYPE_ZIB){
    REQUEST_DEAL(zlib)();
  }
  switch(type){    
  case TYPE_REALTIME:
    REQUEST_DEAL(realtime);
    break;
  case TYPE_AUTO_PUSH:
    REQUEST_DEAL(auto_push);
    break;
  case TYPE_TIME_SHARE:
    REQUEST_DEAL(time_share);
    break;
  case TYPE_HISTORY:
    REQUEST_DEAL(history);
    break;
  }
  
  //send request of server
  
  return -1;
}

//from server to client
int
deal_from_server_to_client(client_fd, buf, buf_len)
     int client_fd;
     const char * buf;
     unsigned long buf_len;
{
  //recive from server
  unsigned short type = 0;
  //parse and second deal
  if(type == TYPE_ZIB){

  }
  
  switch(type){
  case TYPE_REALTIME:
    RESPONSE_DEAL(realtime)();
    break;
  case TYPE_AUTO_PUSH:
    RESPONSE_DEAL(auto_push)();
    break;
  case TYPE_TIME_SHARE:
    RESPONSE_DEAL(time_share)();
    break;
  case TYPE_HISTORY:
    RESPONSE_DEAL(history)();
    break;
  }

  //send to client


  return -1;
}

//request of realtime
int deal_request_of_realtime()
{
  return 0;
}

//request of auto_push
int deal_request_of_auto_push()
{
  return 0;
}

//request of time_share
int deal_request_of_time_share()
{
  return 0;
}

//request of history
int deal_request_of_history()
{
  return 0;
}

//request of zib
int deal_request_of_zlib()
{
  return 0;
}

//response of realtime
int deal_response_of_realtime()
{
  return 0;
}

//response of auto_push
int deal_response_of_auto_push()
{
  return 0;
}

//response of time_share
int deal_response_of_time_share()
{
  return 0;
}

//response of history
int deal_response_of_history()
{
  return 0;
}

//response of zlib
int deal_response_of_zlib()
{
  return 0;
}
