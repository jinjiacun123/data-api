#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include <sys/termio.h>
#include  <sys/socket.h>
#include  <sys/times.h>
#include  <sys/select.h>
#include  <sys/wait.h>
#include  <errno.h>

#ifndef _SCO_DS
#include <fcntl.h>
#endif
#include <signal.h>
#define        LISTENMAX        2
int g_iCltNum=0;
extern int errno;

void WriteErrLog(const char *i_sFormat,...)
{
  va_list args;
  char  *sLogFile;
  FILE  *fLogFile;
  time_t tWriteTime;
  char  sWriteTime[20];
  pid_t ThisPid;

  ThisPid=getpid();
  time(&tWriteTime);
  cftime(sWriteTime,"%Y/%m/%d %H:%M:%S",&tWriteTime);
  sLogFile=getenv("ERRFILE");

  if(sLogFile != NULL) fLogFile=fopen(sLogFile,"a+");

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
PassiveSock(const char *service,const char *transport,int qlen)
{
  struct servent *pse;
  struct protoent *ppe;
  struct sockaddr_in sin;
  int s,type,optval,optlen;

  memset(&sin,0,sizeof(sin));
  sin.sin_family=AF_INET;
  sin.sin_addr.s_addr=INADDR_ANY;//?(u_long)0x00000000
  if (pse=getservbyname(service,transport))
    sin.sin_port=htons(ntohs((u_short)pse->s_port));
  else if ((sin.sin_port=htons((u_short)atoi(service)))==0)
    WriteErrLog("Warning:Can't get service!!:%s/n",service);
  /* map protocol name to protocol number        */
  if ((ppe=getprotobyname(transport))==0)
    WriteErrLog("Warning:Can't get protocol number:%d!!/n",transport);

  if (!strcmp(transport,"udp"))
    type=SOCK_DGRAM;
  else
    type=SOCK_STREAM;

  /*        allocate a socket        */
  s=socket(PF_INET,type,ppe->p_proto);
  if (s<0)
    WriteErrLog("Can't create socket/r/n");

  optval = 1;
  optlen = sizeof(optval);
  if(setsockopt(s,SOL_SOCKET,SO_REUSEPORT,(char *)&optval,optlen) == -1)
    WriteErrLog("Warning:setsockopt error:%s/n",strerror(errno));


  /*        bind the socket        */
  if (bind(s,(struct sockaddr *)&sin,sizeof(sin))<0)
    WriteErrLog("Warning:Can't bind to %s port: %s!!/n",service,strerror(errno));

  if (type==SOCK_STREAM && listen(s,qlen)<0)
    WriteErrLog("Warning:Can't listen on port!!:%s/n",strerror(errno));
  WriteErrLog("SockRecvServer:Listening on port %s......./n",service);
  return s;
}

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
  int isSockId,iSockId,icSockId;

  struct sockaddr_in sAddrIn;

  unsigned char cDataBuf[1024];
#ifdef _SCO_DS
  int iAddrLen;
#else
  size_t iAddrLen;
#endif
  pid_t iChildPid;
  int iRet=0;

  char cService[20];
  char cHost[20];
  char cPort[20];
  char cNum[20];
  int  iPNum;
  fd_set rset;

  if(argc != 5){
    printf("/n金鹰UNIX通用代理服务器/n");
    printf("版本：v1.0/n");
    printf("南京金鹰国际软件版权所有/n/n");
    printf("Usage: %s 代理服务端口号 服务器地址 服务器端口号  允许最大连接数 /n/n",argv[0]);
    exit(0);
  }
  memset(cService,0x0,sizeof(cService));
  memcpy(cService,argv[1],strlen(argv[1]));       
  memset(cHost,0x0,sizeof(cHost));
  memcpy(cHost,argv[2],strlen(argv[2]));       
  memset(cPort,0x0,sizeof(cPort));
  memcpy(cPort,argv[3],strlen(argv[3]));       
  memset(cNum,0x0,sizeof(cNum));
  memcpy(cNum,argv[4],strlen(argv[4]));       
  iPNum = atoi(cNum);

  /*捕捉子进程结束的信号*/
  signal(SIGCHLD,catchcld);
  /*忽略管道信号*/
  signal(SIGPIPE,SIG_IGN);
       
  /*将进程设置成精灵进程 交由init管理*/
  if(fork()>0)exit(0);
  setsid();
       
  /*创建被动服务套接字*/
  iSockId=PassiveSock(cService,"tcp",LISTENMAX);
  if (iSockId<0)
    {
      WriteErrLog("创建sock在服务%s失败/n",cService);
      exit(-1);
    }
  while(1)
    {        
      signal(SIGCHLD,catchcld);
      signal(SIGPIPE,SIG_IGN);
      errno=0;
      iAddrLen=sizeof(sAddrIn);
               
      /*等待客户端的连接*/
      if((isSockId=accept(iSockId,(struct sockaddr *)&sAddrIn,&iAddrLen))<(long)0)
	{
	  WriteErrLog("Accept error (%d):%s/n",errno,strerror(errno));
	  /*连接出错 1 秒后继续服务*/
	  sleep(1);continue;
	}

      /*WriteTest("Accepted connection(%d) From host(%s) port(%d)/n",isSockId,inet_ntoa(sAddrIn.sin_addr),ntohs(sAddrIn.sin_port));*/

      /*进程数量大于iPNum时 1秒后继续服务*/
      if(g_iCltNum > iPNum - 1)
	{
	  close(isSockId);
	  sleep(1); continue;
	}
      /*创建进程*/
      if((iChildPid=fork()) < 0)
	{
	  WriteErrLog("Fork error :%s!!/n",strerror(errno));
	  close(isSockId);
	  /*出错1秒后 继续服务*/
	  sleep(1); continue;
	}
      /*父进程继续等待客户端的连接 进程数量加一*/
      if(iChildPid>0) { close(isSockId);g_iCltNum++;continue; }

      close(iSockId);

      /*连接到服务器*/
      icSockId=ConnectSock(cHost,cPort,"tcp",0);
      if (icSockId<0)
	{
	  WriteErrLog("连接(%s:%s)失败(%d):%s/n",cHost,cPort,errno,strerror(errno));
	  close(isSockId);
	  exit(0);
	}
      /*WriteTest(" %d to  %d/n",isSockId,icSockId);*/

      while(1)
	{
	  FD_ZERO(&rset);
	  FD_SET(icSockId,&rset);
	  FD_SET(isSockId,&rset);
	  iRet=select(isSockId+1,&rset,NULL,NULL,NULL);
	  /*如果连接在客户机的套接字可读*/
	  if(FD_ISSET(isSockId,&rset))
	    {
	      /*读取客户请求*/
	      iRet=recv(isSockId,cDataBuf,1024,0);
	      if(iRet == 0 )
		{
		  close(icSockId);
		  close(isSockId);
		  exit(0);
		}
	      /*WriteTest("U recv(%d)=%s/n",iRet,cDataBuf);*/
	      /*转发到服务器*/
	      if(send(icSockId,cDataBuf,iRet,0)<0)
		{
		  close(icSockId);
		  close(isSockId);
		  exit(0);
		}
	    }
	  /*如果连接在服务器的套接字可读*/
	  if(FD_ISSET(icSockId,&rset))
	    {
	      /*读服务器的应答*/
	      iRet=recv(icSockId,cDataBuf,1024,0);
	      if(iRet == 0 )
		{
		  close(icSockId);
		  close(isSockId);
		  exit(0);
		}
	      /*WriteTest("D recv(%d)=%s/n",iRet,cDataBuf);*/
                                /*转发到客户*/
                                if(send(isSockId,cDataBuf,iRet,0)<0)
                                {
                                        close(icSockId);
                                        close(isSockId);
                                        exit(0);
                                }
                        }
                }
        }
} 
