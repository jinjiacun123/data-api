#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
//#include <sys/termio.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <errno.h>
#include <poll.h>

#ifndef _SCO_DS
#include <fcntl.h>
#endif
#include <signal.h>
#define LISTENMAX   1024
#define PROXY_SERVER "127.0.0.1"
#define PROXY_PORT   8001
#define SERVER       "127.0.0.1"
#define SERVER_PORT  8000

int g_iCltNum=0;
extern int errno;
void WriteErrLog(const char * i_sFormat,...);
int PassiveSock();
//char *cftime(char *s, const char *format, time_t *time);
int ConnectSock();
void deal_proxy(int proxyClientSocketId, int clientSocketId);

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

    //WriteTest("Accepted connection(%d) From host(%s) port(%d)/n",isSockId,inet_ntoa(sAddrIn.sin_addr),ntohs(sAddrIn.sin_port));

    //进程数量大于iPNum时 1秒后继续服务
    if(g_iCltNum > iPNum - 1){
      WriteErrLog("g_iCltNum is too much\n");
      close(clientSocketId);
      sleep(1); 
      continue;
    }
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
    
    /*WriteTest(" %d to  %d/n",isSockId,icSockId);*/

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

/*
char *cftime(char *s, const char *format, time_t *time)
{
  strftime(s, 256, format, localtime(time));
  return s;
}
*/

//proxy client connect to server
int
ConnectSock(){
  int proxy_client_fd;
  struct sockaddr_in proxy_client_addr;

  proxy_client_addr.sin_family = AF_INET;
  proxy_client_addr.sin_addr.s_addr = inet_addr(SERVER);
  proxy_client_addr.sin_port = htons(SERVER_PORT);

  if((proxy_client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("error proxy_client_addr!\n");
    exit(1);
  }

  if(connect(proxy_client_fd, (void *)&proxy_client_addr ,sizeof(proxy_client_addr)) == -1){
    perror("error proxy connection to server!\n");
    exit(1);
  }

  return proxy_client_fd;
}

void deal_proxy(int proxyClientSocketId, int clientSocketId){
  struct pollfd client[2];
  unsigned char cDataBuf[1024];
  int n;
  fd_set rset;
  int iRet=0;
  int maxi=0;
  int nready;
  int i;
  
  
  client[0].fd = proxyClientSocketId;
  client[0].events = POLLIN;
  client[1].fd = clientSocketId;
  client[1].events = POLLIN;
  
  while(1){
    nready = poll(client, 2, -1);    

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
	/*
	else if(n == 0){
	  close(client[0].fd);
	  client[0].fd = -1;
	}
	*/
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
	/*
	else if(n == 0){
	  close(client[1].fd);
	  client[1].fd = -1;
	}
	*/
	else if(n >0){
	  //send to server
	  WriteErrLog("send to server\n");
	  write(client[0].fd, cDataBuf, n);
	}
      }  
    }  
    
  }

  /*
  while(1){
    FD_ZERO(&rset);
    FD_SET(proxyClientSocketId,&rset);
    FD_SET(clientSocketId,&rset);
    iRet=select(clientSocketId+1,&rset,NULL,NULL,NULL);
    
    //如果连接在客户机的套接字可读
    if(FD_ISSET(clientSocketId,&rset)){
      //读取客户请求
      iRet=recv(clientSocketId,cDataBuf,1024,0);
      if(iRet == 0 ){
	close(proxyClientSocketId);
	close(clientSocketId);
	exit(0);
      }
      WriteErrLog("recive client request!\n");
      WriteErrLog("proxyClientSocketId:%d\n", proxyClientSocketId);
      //WriteTest("U recv(%d)=%s/n",iRet,cDataBuf);
      //转发到服务器
      if(send(proxyClientSocketId,cDataBuf,iRet,0)<0){
	close(proxyClientSocketId);
	close(clientSocketId);
	exit(0);
      }
      WriteErrLog("send to server!\n");
    }

    //如果连接在服务器的套接字可读
    if(FD_ISSET(proxyClientSocketId,&rset)){
      //读服务器的应答
      iRet = recv(proxyClientSocketId,cDataBuf,1024,0);
      if(iRet == 0 ){
	close(proxyClientSocketId);
	close(clientSocketId);
	exit(0);
      }
      printf("recive server!\n");
      //WriteTest("D recv(%d)=%s/n",iRet,cDataBuf);
      //转发到客户
      if(send(clientSocketId,cDataBuf,iRet,0)<0){
	close(proxyClientSocketId);
	close(proxyClientSocketId);
	exit(0);
      }
      printf("send to client!\nx");
    } 
  }
  */
}
