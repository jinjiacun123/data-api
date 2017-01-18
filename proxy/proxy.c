#include  "config.h"
#include "comm_pipe.h"
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
  int is_used;
  int process_id;
  char ip[15];
}process_ip_t;

int g_iCltNum=0;
extern int errno;
void WriteErrLog(const char * i_sFormat,...);
int PassiveSock();
//char *cftime(char *s, const char *format, time_t *time);
int ConnectSock();
int get_client_ip(int client_fd, char * ip);
int is_connected(int socket_fd, process_ip_t process_ip_list[]);
int add_connected(int socket_fd, process_ip_t process_ip_list[]);
int remove_connected(int process_id);
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
static int deal_request_of_realtime(int proxy_client_socket_fd,
				    const char * buff,
				    unsigned int buff_len);
//request of auto_push
static int deal_request_of_auto_push(int proxy_client_socket_fd,
				     const char * buff,
				     unsigned int buff_len);
//request of time_share
static int deal_request_of_time_share(int proxy_client_socket_fd,
				      const char * buff,
				      unsigned int buff_len);
//request of history
static int deal_request_of_history(int proxy_client_socket_fd,
				   const char * buff,
				   unsigned int buff_len);
//request of zib
static int deal_request_of_zlib(int proxy_client_socket_fd,
				const char * buff,
				unsigned int buff_len);
static int init_request_sort(pid_t p_id, int * pipe_write_fd);
void * deal_request_of_sort(pid_t p_id,
			    void * p_app_request_data,
			    int pipe_write_fd,
			    int * pipe_read_fd,
			    int epfd,
			    bool * is_create_pipe);
static int make_socket_non_blocking(int sfd);

//response of realtime
static int deal_response_of_realtime();
static void do_foreign_exchange(char * code, buff_t * my_buff,int i);
static void do_no_foreign_exchange(char * code, buff_t * my_buff, int i);
static void do_exponent(char * code, buff_t * my_buff, int i);
static void do_stock(char * code, buff_t * my_buff, int i);
//response of auto_push
static int deal_response_of_auto_push();
//response of time_share
static int deal_response_of_time_share();
//response of history
static int deal_response_of_history();
//response of zlib
static int deal_response_of_zlib();
//response of heart
static int deal_response_of_heart();
static int deal_server_info(int clientSocketId,
			    int proxyClientSocketId,
			    int * alive_times,
			    char * client_ip 
			    );
static int deal_client_info(int clientSocketId,
			    int proxyClientSocketId,
			    int * alive_times,
			    char * client_ip,
			    int epfd,
			    int * pipe_write_fd,
			    int * pipe_read_fd,
			    bool * is_create_pipe
			    );
static int deal_sort_info(int clientSocketId,
			  int pipe_read_fd,
			  int * alive_times,
			  char * client_ip);

static int make_socket_non_blocking(int sfd)
{
  int flags, s;
  flags = fcntl(sfd, F_GETFL,0);
  if(flags == -1)
    {
      perror("fcntl");
      return-1;
    }

  flags|= O_NONBLOCK;
  s =fcntl(sfd, F_SETFL, flags);
  if(s ==-1)
    {
      perror("fcntl");
      return-1;
    }
  return 0;
}

void catchcld(int sig)
{
  int  iStat;
  int p_id;
  while((p_id = waitpid(-1, NULL, WNOHANG)) >0){
    remove_connected(p_id);
    g_iCltNum--;
  }
  /*
  //等待进程结束
  wait(&iStat);
  //子进程结束后进程数量减一
  g_iCltNum--;
  */
}

#define MAX_CONNECTS 10
/*
函数：tcp代理服务
功能：创建一个Socket服务，等待客户端连接，在客户段和服务器端建立代理连接
参数
        代理服务端口号
        服务器地址
        服务器端口号
        允许最大连接数
*/
process_ip_t process_ip_list[MAX_CONNECTS]={};
pthread_key_t key_sort;
void main(int argc,char *argv[])
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
  int  iPNum = MAX_CONNECTS;

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

  int i = 0;
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
    /*
#define SOCKET_ERROR (-1)
    int keepAlive = 1;   //设定keepalive
    int keepIDie = 5;    //首次探测开始前的tcp无数手法空闲实际
    int keepInterval = 3;//每次侦探的间隔实际
    int keepCount = 2;   //侦探次数


    if(setsockopt(clientSocketId, SOL_SOCKET, SO_KEEPALIVE,(void*)&keepAlive, sizeof(keepAlive)) == SOCKET_ERROR){
      WriteErrLog("Call setsockopt error,error is %d\n", errno);
      exit(-1);
    }
    if(setsockopt(clientSocketId, SOL_TCP, TCP_KEEPIDLE, (void*)keepInterval, sizeof(keepInterval)) == SOCKET_ERROR){
      WriteErrLog("call setsockopt error,error is %d\n", errno);
      exit(-1);
    }
    if(setsockopt(clientSocketId, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval)) == SOCKET_ERROR){
      WriteErrLog("call setsocket error, error is%d\n", errno);
      exit(-1);
    }
    if(setsockopt(clientSocketId, SOL_TCP, TCP_KEEPCNT,(void *)&keepCount, sizeof(keepCount)) == SOCKET_ERROR){
      WriteErrLog("call setsocketopt error, error is %d\n", errno);
      exit(-1);
    }
    */

    //进程数量大于iPNum时 1秒后继续服务
    if(g_iCltNum > iPNum - 1){
      WriteErrLog("g_iCltNum is too much\n");
      close(clientSocketId);
      sleep(1); 
      continue;
    }
    
    //check ip is connected
    /*
    if((i=is_connected(clientSocketId, process_ip_list)) < 0){
      WriteErrLog("this client is connected!\n");
      close(clientSocketId);
      continue;
    }
    */
    i = add_connected(clientSocketId, process_ip_list);
    
    
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
    if(iChildPid > 0){
      process_ip_list[i].process_id = iChildPid;
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

    deal_proxy(proxyClientSocketId, clientSocketId);
  }
}

void WriteErrLog(const char *i_sFormat,...)
{
  va_list args;
  char  *sLogFile="/tmp/err.log";
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
  int opt = 1;

  memset(&sin,0,sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;//?(u_long)0x00000000
  sin.sin_port = htons(PROXY_PORT);

  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s<0)
    WriteErrLog("Can't create socket/r/n");

  optval = 1;
  optlen = sizeof(optval);
  if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) == -1)
     WriteErrLog("Waring:setsockopt so_reuseaddr error:%s\n", strerror(errno));
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



void deal_proxy(proxyClientSocketId, clientSocketId)
  int proxyClientSocketId;
  int clientSocketId;
{
  int n;
  fd_set rset;
  int iRet = 0;
  int maxi = 0;
  int i;
  int heart_times = 0;
  int length = 8;
  p_response_header p_header;
  bool is_custom = false;
  int ret = -1;
  int alive_times = 0;
  pthread_t sort_p_id = -1;
  app_request_data *my_app_request_data = NULL;
  int status = -1;
  int sort_fd = -1;
  pid_t p_id = getpid();
  int pipe_read_fd = -1;
  int pipe_write_fd = -1;
  int epfd, nfds;
  struct epoll_event ev, events[3]={0};
  epfd = epoll_create(256);
  bool is_create_pipe = false;

  //init server's login
  ret = init_login(proxyClientSocketId);
  assert(ret == 0);

  ret = make_socket_non_blocking(proxyClientSocketId);
  assert(ret == 0);
  ret = make_socket_non_blocking(clientSocketId);
  assert(ret == 0);

  ev.data.fd = proxyClientSocketId;
  ev.events = EPOLLIN | EPOLLRDHUP | EPOLLERR| EPOLLET;
  epoll_ctl(epfd, EPOLL_CTL_ADD, proxyClientSocketId, &ev);
  ev.data.fd = clientSocketId;
  ev.events = EPOLLIN | EPOLLRDHUP | EPOLLERR| EPOLLET;
  epoll_ctl(epfd, EPOLL_CTL_ADD, clientSocketId, &ev);

  signal(SIGPIPE, SIG_IGN);

  char client_ip[15];
  memset(&client_ip, 0x00, 15);
  assert(get_client_ip(clientSocketId, &client_ip) == 0);

  while(1){
    nfds = epoll_wait(epfd, events, 3, -1);
    is_custom = false;

    if((nfds == -1) && (errno == EINTR))
      continue;
    else if(nfds == -1){
      break;
    }

    if(nfds == 0){//timeout
      WriteErrLog("%s\tprocess timeout\n", client_ip);
      close(proxyClientSocketId);
      shutdown(proxyClientSocketId,2);
      close(clientSocketId);
      shutdown(clientSocketId,2);
      exit(-1);
      //send heart to server
      //assert(send_heart_to_server(proxyClientSocketId, &heart_times) == 0);
    }
/*
    if(alive_times>20){
      WriteErrLog("%s\tclient is die\n", client_ip);
      exit(-1);
    }
    */
    for(i = 0; i< nfds; i++){
      //if(!(events[i].events & EPOLLIN)){
	if((events[i].events & EPOLLERR) ||
	   // (events[i].events & EPOLLHUP) ||
	   (events[i].events & EPOLLRDHUP) ||
	   (!(events[i].events & EPOLLIN))){
	  WriteErrLog("%s\tepoll err!\n", client_ip);
	  close(clientSocketId);
	  close(proxyClientSocketId);
	  exit(-1);
	}
	//}
      if(events[i].events & EPOLLIN){
	//check is client info
	if(events[i].data.fd == clientSocketId){
	  ret = deal_client_info(clientSocketId,
				 proxyClientSocketId,
				 &alive_times,
				 client_ip,
				 epfd,
				 &pipe_write_fd,
				 &pipe_read_fd,
				 &is_create_pipe);
	  assert(ret == 0);
	  if(ret != 0){
	    WriteErrLog("%s\tdeal client info err!\n", client_ip);
	    exit(-1);
	  }
	}
	//check is server info
	else if(events[i].data.fd == proxyClientSocketId){
	  ret = deal_server_info(clientSocketId,
				 proxyClientSocketId,
				 &alive_times,
				 client_ip);
	  assert(ret == 0);
	  if(ret != 0){
	    WriteErrLog("%s\tdeal server info err!\n", client_ip);
	    exit(-1);
	  }
	}
	//check is sort info
	else if(events[i].data.fd == pipe_read_fd){
	  ret = deal_sort_info(clientSocketId,
			       pipe_read_fd,
			       &alive_times,
			       client_ip);
	  if(ret != 0){
	    WriteErrLog("%s\tdeal sort info err!\n", client_ip);
	    exit(-1);
	  }
	}
      }
    }
  }
}

//deal server info with proxy
static int deal_server_info(client_socket_fd,
			    proxy_client_socket_fd,
			    alive_times,
			    client_ip)
  int client_socket_fd;
  int proxy_client_socket_fd;
  int * alive_times;
  char * client_ip;
{
  char header_buff[8];
  int length, len, n;
  p_response_header p_header;
  char tmp_buff[BUFSIZ]={0};
  int ret = -1;
  int nread = -1;
  int wret = -1;

  //recive server info
  *alive_times++;
  WriteErrLog("%s:recive info from server!\n", client_ip);
L1:  length = 8;
  while(true){
    n = 0;
    while((nread = read(proxy_client_socket_fd, tmp_buff, BUFSIZ-1))>0){
      n += nread;
      int off_len = 0;
      int write_len = nread;      	
      while(wret = write(client_socket_fd, tmp_buff+off_len, write_len)){
      	if(wret <= 0){
		if(errno == EAGAIN){
            	usleep(1000);
	    	continue;
		}else{
			WriteErrLog("%s write to client err!errno:%d\n", client_ip, errno);
			exit(-1);
		}
      	}        
	
        if(wret == write_len){
		break;
	}
	
        off_len += wret;
	write_len -= wret;
      }
      if(nread < BUFSIZ -1)
	return 0;
      memset(tmp_buff, 0x00, BUFSIZ);
    }
    if(nread == -1 && errno != EAGAIN){
      return 0;
    }
    if(nread == 0){ break;}
      /*
      if(n == -1){
	if(errno != EAGAIN){
	  WriteErrLog("%s\ttread server err!\n", client_ip);
	  return 0;
	}
	continue;
      }
      if(n == 0){
	WriteErrLog("%s\tserver deal finish!\n", client_ip);
	break;
      }
      else if(n == length){
      //send to client
      WriteErrLog("%s\tsend to client\n", client_ip);
      //write(client[1].fd, cDataBuf, n);
      //recive head from server
      p_header = (p_response_header)header_buff;
      if(strncmp(p_header->str, HEADER, 4)){
	WriteErrLog("%s\tcompare header err!\n", client_ip);
	exit(-1);
      }
      length = p_header->length;
      int off = 0;
      int last_length = length;
      while(n = read(proxy_client_socket_fd, tmp_buff+off+8, last_length)){
	if(n<0){
	  if(errno == EAGAIN && n == -1){
	    //close(proxy_client_socket_fd);
	    //proxy_client_socket_fd = -1;
	    return 0;
	  }
	  continue;
	}
	else if(n == 0){
	  WriteErrLog("%s\tserver deal finish!\n", client_ip);
	  return 0;
	}
	else if(n == last_length){
	  //reccive complete and send to server
	  memcpy(tmp_buff, HEADER, 4);
	  memcpy(tmp_buff+4, &length, 4);
	  while((n = write(client_socket_fd, tmp_buff, length+8))>0){
	    if(n == length+8){
	      goto L1;
	      return 0;
	    }
	  }
	  if(n == -1 && errno == EAGAIN){}
	  else{
	    WriteErrLog("%s\tWrite to client error!errno:%d\n", client_ip, errno);
	    exit(-1);
	    //return 0;
	  }
      }
      else if(n < last_length){
	off += n;
	last_length -= n;
      }
    }
  }
      */
    }
  //deal_from_server_to_client
  //	assert(deal_from_server_to_client(client[1].fd, buff, length) == 0);
  return 0;
}

//deal client info with proxy
static int deal_client_info(client_socket_fd,
			    proxy_client_socket_fd,
			    alive_times,
			    client_ip,
			    epfd,
			    pipe_write_fd,
			    pipe_read_fd,
			    is_create_pipe)
     int client_socket_fd;
     int proxy_client_socket_fd;
     int * alive_times;
     char * client_ip;
     int epfd;
     int * pipe_write_fd;
     int * pipe_read_fd;
     bool * is_create_pipe;
{
  bool is_custom = false;
  int length, n;
  p_response_header p_header;
  int ret;
  app_request_data * my_app_request_data = NULL;
  int status = -1;
  char tmp_buff[BUFSIZ] = {0};
  char sort_buff[1024] = {0};
  int nread = -1;
  int package_length = 0;
  char header[8] = {0};
  int app_off = 0;
  int packages = 0;

  while(true){
    is_custom = false;
    //recive client info
    *alive_times = 0;
    //head of package
  L:    length = 8;
    n = 0;
    package_length = 0;
    while((nread = read(client_socket_fd, tmp_buff,  BUFSIZ-1)) > 0){
      WriteErrLog("%s read client info!\n", client_ip);
      app_off = 0;
      packages = 0;
      //while(true){
	//check is request of sort
	if(strncmp(tmp_buff+app_off, HEADER_EX, 4) == 0){
	  	  packages ++;
	  package_length = *(int*)(tmp_buff+4+app_off) + 8;
	  memcpy(sort_buff, tmp_buff+8, package_length -8);
	  if(-1 == * pipe_write_fd){
	    //open sort pipe
	    ret = init_request_sort(getpid(), pipe_write_fd);
	    assert(ret == 0);
	  }
	  //init sort of request
	  my_app_request_data = (app_request_data*)malloc(sizeof(app_request_data));
	  if(my_app_request_data == NULL){
	    WriteErrLog("malloc my_app_request_data error!\n");
	    exit(-1);
	  }
	  memset(my_app_request_data, 0x00, sizeof(app_request_data));
	  my_app_request_data->socket_fd = client_socket_fd;
	  my_app_request_data->buff      = sort_buff;
	  my_app_request_data->buff_len  = package_length-8;
	  my_app_request_data->start     = true;
	  deal_request_of_sort(getpid(),
			       my_app_request_data,
			       *pipe_write_fd,
			       pipe_read_fd,
			       epfd,
			       is_create_pipe);
	  free(my_app_request_data);
	}
        /*
	else{
	  break;
	}
	if(nread > package_length){
	  app_off += package_length;
	}
        */
      //}

      n  += nread;
      if((nread - package_length) == 0)return 0;
      ret = write(proxy_client_socket_fd,
		  tmp_buff + package_length * packages,
		  nread - package_length * packages);
      if(ret <= 0){
	WriteErrLog("%s write to proxy err!\n", client_ip);
	exit(-1);
      }
      if(nread < BUFSIZ -1)
	return 0;
    }
    if(nread == -1 && errno != EAGAIN){
      return 0;
    }
    if(nread == 0){ break;}
    /*
    if((n = read(client_socket_fd, header_buff, length))<0){
      if(n == -1 && errno == EAGAIN){
	WriteErrLog("%s\tread client head err!\n", client_ip);
	return 0;
      }
      exit(-1);
    }
    else if(n == 0){
      WriteErrLog("%s\tclient deal finish!\n", client_ip);
      return 0;
    }
    else if(n == length){
      //send to server
      //WriteErrLog("send to server\n");
      //write(client[0].fd, cDataBuf, n);
      //send to client
      WriteErrLog("%s\tread client info!\n", client_ip);
      //write(client[1].fd, cDataBuf, n);
      //recive head from server
      p_header = (p_response_header)header_buff;
      if(strncmp(p_header->str, HEADER_EX, 4) == 0){
	is_custom = true;
      }else if(strncmp(p_header->str, HEADER, 4)){
	WriteErrLog("%s\tcompare header err!\n", client_ip);
	exit(-1);
      }

      length = p_header->length;
      int off = 0;
      int last_length = length;
      while(n = read(client_socket_fd, tmp_buff+off+8, BUFSIZ)){
	if(n<0){
	  if(n == -1 && errno == EAGAIN){
	    //close(client_socket_fd);
	    //client_socket_fd = -1;
	    WriteErrLog("%s\tclient read body error!\n", client_ip);
	    return 0;
	  }
	  continue;
	}else if(n == 0){
	  WriteErrLog("%s\tclient read finish!\n", client_ip);
	  return 0;
	}else if(n == last_length){
	  if(!is_custom){
	    memcpy(tmp_buff, HEADER, 4);
	    memcpy(tmp_buff+4, &length, 4);
	    while(1){
	      n = write(proxy_client_socket_fd, tmp_buff, length+8);
	      if(n < 0){
		if(errno == EINTR)
		  return -1;

		if(errno == EAGAIN){
		 return 0;
		}
	      }
	      if(n == length+8){
	 	//return 0;
		goto L;
	      }
	    }
	  }else{
	      //ret = deal_request_of_sort(client[1].fd, send_buff, length+8);
	      //assert(ret == 0);
	    if(-1 == * pipe_write_fd){
	      //open sort pipe
	      ret = init_request_sort(getpid(), pipe_write_fd);
	      assert(ret == 0);
	    }
	    //init sort of request
	    my_app_request_data = (app_request_data*)malloc(sizeof(app_request_data));
	    if(my_app_request_data == NULL){
	      WriteErrLog("malloc my_app_request_data error!\n");
	      exit(-1);
	    }
	    memset(my_app_request_data, 0x00, sizeof(app_request_data));
	    my_app_request_data->socket_fd = client_socket_fd;
	    my_app_request_data->buff = tmp_buff;
	    my_app_request_data->buff_len = length;
	    my_app_request_data->start = true;
	    deal_request_of_sort(getpid(), my_app_request_data, *pipe_write_fd, pipe_read_fd, epfd);
	    free(my_app_request_data);
	    free(tmp_buff);
	    break;
	  }
	}
	else if(n < last_length){
	  off += n;
	  last_length -= n;
	}
      }
    }
    */
  }
  WriteErrLog("%s\trecive client info complete!\n", client_ip);
  //body of package
  //assert(deal_from_client_to_server(client[0].fd, buff, length) == 0);

  return 0;
}

//deal sort info with proxy
static int deal_sort_info(clientSocketId,
			  pipe_read_fd,
			  alive_times,
			  client_ip)
     int clientSocketId;
     int pipe_read_fd;
     int * alive_times;
     char * client_ip;
{
  int res = -1;
  int size = 10;
  int entity_len = sizeof(sort_entity_t);
  int sort_buff_len = entity_len *size + sizeof(int);
  char * sort_buff = NULL;;
  int off = 0;

  sort_buff = (char *)malloc(sort_buff_len + 8);
  if(sort_buff == NULL){
    printf("error malloc\n");
    exit(-1);
  }
  memset(sort_buff, 0x00, sort_buff_len + 8);
  WriteErrLog("read pipe...\n");
  //read sort
  res = read(pipe_read_fd, sort_buff+8, sort_buff_len);
  if(res == -1){
    WriteErrLog("read error!\n");
    exit(-1);
  }else if(res == 0){
    WriteErrLog("read complete...\n");
    exit(-1);
  }

  //write client
  memcpy(sort_buff, HEADER_EX, 4);
  memcpy(sort_buff+4, &sort_buff_len, 4);
  assert(res >0);
  while(res = write(clientSocketId, sort_buff+off, sort_buff_len+8-off)){
    if(res == -1){
      if(errno == EAGAIN){
	continue;
      }
    }else if(res == sort_buff_len + 8 - off){
      free(sort_buff);
      break;
    }
    else{
      off += res;
    }
  }
  WriteErrLog("res=%d\n", res);
  assert(res > 0);

  return 0;
}

//send heart to server
int send_heart_to_server(int proxy_client_fd, int * heart_times)
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
int init_login(int proxy_client_fd)
{
  char * package;
  int request_len = sizeof(request_login_t);
  int package_len = request_len ;
  package = (char *)malloc(package_len);
  memset(package, 0x00, package_len);

  //request login
  request_login_t * data;
  data = (request_login_t *)package;
  memcpy(data->header_name, HEADER, 4);
  data->body_len = package_len - 8;
  data->type = TYPE_LOGIN;

  data->key = 3;
  data->index = 0;
  strncpy(data->username, USERNAME, 64);
  strncpy(data->password, PASSWORD, 64);

  write(proxy_client_fd, package, package_len);
  free(package);
  WriteErrLog("send login info!\n");

  //response login
  p_response_s_t  p_response_s;
  char * buff;
  int buff_len = 8;
  unsigned short type;
  assert(buff = (char *)malloc(buff_len+1));
  memset(buff, 0x00, buff_len);
  int n = read(proxy_client_fd, buff, buff_len);
  WriteErrLog("recive login info!\n");
  if(n == 8){
    p_response_s = (p_response_s_t)buff;
    //parse head
    if(strncmp(p_response_s->header_name, HEADER, 4)){
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

    type = *((unsigned short *)buff);
    if(type != TYPE_LOGIN){
      WriteErrLog("recive login body's type of info err!\n");
      exit(-1);
    }

    WriteErrLog("recive login info check ok!\n");
    return 0;
  }
  else{
    WriteErrLog("recive login info err!\n");
    exit(-1);
  }
  return -1;
}

//from client to server
int deal_from_client_to_server(proxy_client_fd, buff, buff_len)
     int proxy_client_fd;
     const char * buff;
     unsigned long buff_len;
{
  WriteErrLog("send message from client to server!\n");
  //parse client's request
  unsigned short type = 0;
  //get type
  type = *((unsigned short*)buff);
  //deal request of client to request of server
  if(type == TYPE_ZIB){
    REQUEST_DEAL(zlib)(proxy_client_fd,buff, buff_len);
  }
  switch(type){
  case TYPE_REALTIME:
    REQUEST_DEAL(realtime)(proxy_client_fd,buff, buff_len);
    break;
  case TYPE_AUTO_PUSH:
    REQUEST_DEAL(auto_push)(proxy_client_fd,buff, buff_len);
    break;
  case TYPE_TIME_SHARE:
    REQUEST_DEAL(time_share)(proxy_client_fd,buff, buff_len);
    break;
  case TYPE_HISTORY:
    REQUEST_DEAL(history)(proxy_client_fd,buff, buff_len);
    break;
  }
  
  //send request of server
  
  return 0;
}

//from server to client
int deal_from_server_to_client(client_fd, buff, buff_len)
     int client_fd;
     const char * buff;
     unsigned long buff_len;
{
  //recive from server
  WriteErrLog("send message from server to client!\n");
  unsigned short type = 0;
  //get type
  type = *((unsigned short*)buff);
  //parse and second deal
  if(type == TYPE_ZIB){

  }
  
  switch(type){
  case TYPE_HEART:
    RESPONSE_DEAL(heart)(client_fd, buff, buff_len);
    break;
  case TYPE_REALTIME:
    RESPONSE_DEAL(realtime)(client_fd, buff, buff_len);
    break;
  case TYPE_AUTO_PUSH:
    RESPONSE_DEAL(auto_push)(client_fd, buff, buff_len);
    break;
  case TYPE_TIME_SHARE:
    RESPONSE_DEAL(time_share)(client_fd, buff, buff_len);
    break;
  case TYPE_HISTORY:
    RESPONSE_DEAL(history)(client_fd, buff, buff_len);
    break;
  }
  //send to client


  return 0;
}

//request of realtime
static int deal_request_of_realtime(int proxy_client_socket_fd, 
				    const char * buff, 
				    unsigned int buff_len)
{
  char request[1024];
  int size = 1;

  //get entity_list from client
  request_c_realtime_t * request_c_realtime = (request_c_realtime_t *)buff;
  if(request_c_realtime->type != TYPE_REALTIME){
    WriteErrLog("Type err in request");
    exit(-1);
  }
  size = request_c_realtime->size;
  entity_t l_entity[size];
  memset(l_entity, 0x00, sizeof(entity_t)*size);
  entity_t * entity;
  int i=0;
  for(i=0; i< size; i++){
    entity = (entity_t*)(buff+sizeof(request_c_realtime_t));
    memcpy(l_entity[i].code, entity->code, 6);
    l_entity[i].code_type = entity->code_type;
  }

  request_realtime_t data ;
  memset(&data, 0x00, sizeof(request_s_t));
  memcpy(data.header_name, HEADER, 4);
  data.body_len =  sizeof(request_realtime_t)
    - 8
    + sizeof(entity_t)*size;

  data.type = TYPE_REALTIME;
  data.size = size;
  data.option= 0x0080;
  /*
  entity_t l_entity[size];
  memset(l_entity, 0x00, sizeof(entity_t)*size);
  memncpy(l_entity[0].code, "XAU", 6);
  l_entity[0].code_type = 0x5b00;
  */

  memset(request, 0, sizeof(data)
                     + sizeof(entity_t)*size);
  memcpy(request, &data, sizeof(data));
  memcpy(request+sizeof(data), &l_entity, sizeof(entity_t)*size);
  write(proxy_client_socket_fd,
	request,
	sizeof(data)+sizeof(entity_t)*size, 0);
  return 0;
}

//request of auto_push
static int deal_request_of_auto_push(int proxy_client_socket_fd, const char * buff, unsigned int buff_len)
{
  char request[1024];

  int size = 1;
  request_realtime_t data ;
  memset(&data,0x00,sizeof(request_realtime_t));
  memcpy(data.header_name, HEADER, 4);
  data.body_len =  sizeof(request_s_t)
    + sizeof(request_realtime_t)
    - 8
    + sizeof(entity_t)*size;
  data.type = TYPE_REALTIME;

  data.size = size;
  data.option= 0x0080;

  entity_t l_entity[size];
  memset(l_entity, 0x00, sizeof(entity_t)*size);
  memcpy(l_entity[0].code, "XAU", 6);
  l_entity[0].code_type = 0x5b00;
  memset(request, 0, sizeof(data)+sizeof(entity_t)*size);
  memcpy(request, &data, sizeof(data));
  memcpy(request+sizeof(data), &l_entity, sizeof(entity_t)*size);
  write(proxy_client_socket_fd,
	request,
	sizeof(data)+sizeof(entity_t)*size, 0);

  return 0;
}

//request of time_share
static int deal_request_of_time_share(int proxy_client_socket_fd, const char * buff, unsigned int buff_len)
{
  char request[1024];

  request_time_share_t data ;
  memset(&data,0x00,sizeof(request_s_t));
  memcpy(data.header_name ,HEADER, 4);
  data.body_len = sizeof(request_time_share_t) - 8;
  data.type = TYPE_TIME_SHARE;
  data.size = 0;
  data.option = 0x0080;
  memcpy(data.code,"XAG",6);
  data.code_type = 0x5b00;
  memcpy(data.code2,"XAG",6);
  data.code_type2 = 0x5b00;

  memset(request, 0, sizeof(data));
  memcpy(request, &data, sizeof(data));
  write(proxy_client_socket_fd, request, sizeof(data), 0);
  return 0;
}

//request of history
static int deal_request_of_history(int proxy_client_socket_fd, const char * buff, unsigned int buff_len)
{
  char request[1024];

  request_history_t data;
  memset(&data,0x00,sizeof(request_s_t));
  memcpy(data.header_name, HEADER,4);
  data.body_len = sizeof(request_history_t) - 8;
  data.type = TYPE_HISTORY;
  data.size = 1;
  data.index = 0;
  data.option = 0x0080;
  strncpy(data.code, "XAG", 6);
  data.code_type = 0x5b00;

  //日线请求
  data.period = PERIOD_TYPE_DAY;      //请求周期类型
  data.begin_position = 0; //请求开始位置
  data.period_num = 1;
  data.day = 400*data.period_num;//请求个数(400个)
  data.size = 0;
  strncpy(data.code2, "xag", 6);
  data.code_type2 = 0x5b00;

  //周线

  //月线

  //5分钟

  //15分钟

  //30分钟

  //60分钟

  //120分钟

  memset(request, 0, sizeof(data));
  memcpy(request, &data, sizeof(data));
  write(proxy_client_socket_fd, request, sizeof(data));
  return 0;
}

//request of zib
static int deal_request_of_zlib(int proxy_client_socket_fd, const char * buff, unsigned int buff_len)
{
  return 0;
}

static int init_request_sort(pid_t p_id, int *pipe_write_fd)
{
  const char *fifo_name = PUBLIC_PIPE;
  int res = 0;
  char cur_app_pipe[100];
  char *template = PRIVATE_PIPE_TEMPLATE;

  memset(&cur_app_pipe, 0x00, 100);
  snprintf(cur_app_pipe, 100, template, getpid());

  //if exists , delete it
  if(access(cur_app_pipe, F_OK) == 0){
    res = unlink(cur_app_pipe);
  }
  //create cur app pipe
  if(access(cur_app_pipe, F_OK) == -1){
    res = mkfifo(cur_app_pipe, 0777);
    if(res != 0){
      WriteErrLog("count not create fifo %s\n", cur_app_pipe);
      exit(-1);
    }
  }

  if(access(fifo_name, F_OK) == -1){
    WriteErrLog("fifo is not exists!\n");
    exit(-1);
  }
  *pipe_write_fd = open(fifo_name, O_RDWR);
  if(*pipe_write_fd == -1){
    WriteErrLog("open pipe err!\n");
    exit(-1);
  }

  return 0;
}

//request of sort
void * deal_request_of_sort(pid,
			    p_app_request_data,
			    pipe_write_fd,
			    pipe_read_fd,
			    epfd,
			    is_create_pipe)
  pid_t pid;
  void * p_app_request_data;
  int pipe_write_fd;
  int * pipe_read_fd;
  int epfd;
  bool * is_create_pipe;
{
  struct epoll_event ev;
  char cur_app_pipe[100];
  char *template = PRIVATE_PIPE_TEMPLATE;
  int begin = 0;
  int size = 0;
  app_request_t * my_request = NULL;
  app_request_data * my_app_request_data = (app_request_data  *)p_app_request_data;
  char * sort_buff;
  int sort_buff_len;
  int option = 0;
  int column = 0;
  int index  = 0;

  memset(&cur_app_pipe, 0x00, 100);
  snprintf(cur_app_pipe, 100, template, getpid());
  my_request = (app_request_t *)my_app_request_data->buff;
  begin  = my_request->begin;
  size   = my_request->size;
  option = my_request->option;
  column = my_request->column;
  index  = my_request->index;

  int app_request_len = sizeof(app_request_t);
  int res = 0;
  sort_entity_t * entity = NULL;
  int i = 0;

  /*
  sort_buff = (sort_entity_t *)malloc(sort_buff_len+1);
  if(sort_buff == NULL){
    WriteErrLog("malloc error!\n");
    exit(-1);
  }
  memset(sort_buff, 0x00, sort_buff_len);
  */

  WriteErrLog("begin:%d,size:%d,option:%d,column:%d,index:%d\n", begin, size, option,column,index);
  app_request_t app_request;
  memset(&app_request, 0x00, sizeof(app_request_t));
  //request
  app_request.pid = pid;
  app_request.begin = begin;
  app_request.size = size;
  app_request.option = option;
  app_request.column = column;
  app_request.index  = index;
  res = write(pipe_write_fd, &app_request, app_request_len);
  assert(res >0);
  WriteErrLog("send pipe request...\n");
  if(*pipe_read_fd <= 0){
    *pipe_read_fd = open(cur_app_pipe, O_RDWR);
    if(*pipe_read_fd == -1){
      WriteErrLog("open pipe read fd error!\n");
      exit(-1);
    }
  }
  //add pipe_read_fd to epoll
  if(*is_create_pipe == false){
    ev.data.fd = *pipe_read_fd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, *pipe_read_fd, &ev);
    *is_create_pipe = true;
  }
  return 0;
}

//response of realtime
static int deal_response_of_realtime(int client_socket_fd, char * my_buff,int buff_len)
{
  response_realtime_t * realtime = (response_realtime_t *)(my_buff);
  char code[7]={0};
  int i=0;

  for(i=0; i< realtime->size; i++){
    response_realtime_price_t * data_type = (response_realtime_price_t *)(my_buff
							+ sizeof(response_realtime_t)
							+ i*(sizeof(response_realtime_price_t)+sizeof(response_realtime_price_no_foreign_exchange_t)));
    memcpy(code, data_type->code, 6);
    if(data_type->code_type == 0x5b00){//非外汇
      do_no_foreign_exchange(code, my_buff, i);
    }
    else if(data_type->code_type == 0xffff8100){//外汇
      do_foreign_exchange(code, my_buff, i);
    }
    //指数（成交量单位（个，万，亿），万和亿只保留两位小数后面加单位(万或亿)
    else if(data_type->code_type == 0x1200){
      do_exponent(code, my_buff, i);
    }
    else if(data_type->code_type == 0x1101){//股票
      do_stock(code, my_buff, i);
    }
  }
  return 0;
}

//处理外汇
static void do_foreign_exchange(char * code, buff_t * my_buff, int i)
{
  WriteErrLog("do_foreign_exchange...\n");
  response_realtime_price_foreign_exchange_t * entity = (response_realtime_price_foreign_exchange_t*)(my_buff->buff
					    + 28
					  + sizeof(response_realtime_price_t)
					  + i*(sizeof(response_realtime_price_t)+sizeof(response_realtime_price_foreign_exchange_t)));
}

//处理非外汇
static void do_no_foreign_exchange(char * code, buff_t * my_buff, int i)
{
  WriteErrLog("do_no_foreign_exchange...\n");
  
  response_realtime_price_no_foreign_exchange_t * entity = (response_realtime_price_no_foreign_exchange_t*)(my_buff->buff
					    +28
					    +sizeof(response_realtime_price_ex_t)
					    +i*(sizeof(response_realtime_price_ex_t)+sizeof(response_realtime_price_no_foreign_exchange_t)));
  // WriteErrLog("品种:%s, 最新价:%d\n", code, entity->new_price);
  
}

//处理指数
static void do_exponent(char * code, buff_t * my_buff, int i)
{
  WriteErrLog("处理指标...\n");
  response_realtime_price_exponent_t * tmp = (response_realtime_price_exponent_t*)(my_buff->buff
					          +28
					     +sizeof(response_realtime_price_t)
					     + i*(sizeof(response_realtime_price_t)+sizeof(response_realtime_price_exponent_t)));
}

//处理股票
static void do_stock(char * code, buff_t * my_buff, int i)
{
  WriteErrLog("处理股票...\n");
  response_realtime_price_stock_t * tmp = (response_realtime_price_stock_t *)(my_buff->buff
					            +28
					      +sizeof(response_realtime_price_t)
					      +i*(sizeof(response_realtime_price_t)+sizeof(response_realtime_price_stock_t)));
}

//response of auto_push
int deal_response_of_auto_push()
{
  return 0;
}

//response of time_share
int deal_response_of_time_share(int client_socket_fd, char * my_buff, int buff_len)
{
  WriteErrLog("解析分时数据\n");
  char code[7];
  memset(code, 0, 7);
  response_time_share_t * time_share = (response_time_share_t *)my_buff;
  memcpy(code, time_share->code, 6);
  int code_len = strlen(code);
  int index = 0;
  for(index = 0; index < code_len; index++){
    code[index] = tolower(code[index]);
  }
  if(time_share->type == TYPE_TIME_SHARE){
    response_time_share_price_t * price = (response_time_share_price_t *)time_share->his_data;
    int i = 0;
    for(i=0; i<time_share->his_len; i++){
      WriteErrLog("第%d条数据 收到code_type:%d\tcode:%s\tnew_price:%d\ttotal:%d\n",
		  i+1,
		  time_share->code_type,
		  time_share->code,
		  price->new_price,
		  price->total);
      price ++;
    }
  }
  return 0;
}

//response of history
int deal_response_of_history(int client_socket_fd, char * my_buff, int buff_lenx)
{
  unsigned short type;
  unsigned short code_type;
  char my_code[7];
  memset(my_code, 0, 7);
  WriteErrLog("client_parse_history\n");
  type = ((unsigned short*)my_buff);
  response_s_t * response_s = (response_s_t*)my_buff;
  if(type == TYPE_HISTORY){
    response_history_t * history = (response_history_t * )my_buff;
    response_history_price_t * price = history->data;
    int code_len = strlen(history->code);
    if(code_len>6) code_len = 6;
    memcpy(my_code, history->code, code_len);
    code_type = history->code_type;
    int i = 0;
    for (i =0; i<history->size; i++){
	    WriteErrLog("index:%d\t code:%.6s\t date:%ld\t open:%ld\t max:%ld\t  min:%ld\t close:%d\t money:%d\t total:%ld\n",
		   i+1,
		   my_code,
		   price->date,
		   price->open_price,
		   price->max_price,
		   price->min_price,
		   price->close_price,
		   price->money,
		   price->total);
      price ++;
    }
  }
  return 0;
}

//response of zlib
static int deal_response_of_zlib()
{
  return 0;
}

//response of heart
static int deal_response_of_heart()
{
  return 0;
}


int get_client_ip(int client_fd, char * ip)
{
  struct sockaddr_in sa;
  int len;

  len = sizeof(sa);
  if(!getpeername(client_fd, (struct sockaddr *)&sa, &len)){
    strcpy(ip, inet_ntoa(sa.sin_addr));
    return 0;
    /*
    memset(sql,0,1024);
    snprintf(sql,1024,"client login. ip: %s, port :%d",inet_ntoa(sa.sin_addr),ntohs(sa.sin_port));
    snprintf(machine_ip,17,"%s",inet_ntoa(sa.sin_addr));
    mylog(sql);
    */
  }
  return -1;
}

//exists 0,not -1
int is_connected(int socket_fd, process_ip_t process_ip_list[])
{
  char cur_ip[18];
  int i = 0;
  memset(&cur_ip, 0x00, 18);

  get_client_ip(socket_fd, cur_ip);
  for(; i< MAX_CONNECTS; i++){
    if(!strcmp(cur_ip, process_ip_list[i].ip)){
      return -1;
    }
  }

  i = 0;
  for(i=0; i< MAX_CONNECTS; i++){
    if(process_ip_list[i].ip == NULL){
      strcpy(process_ip_list[i].ip, cur_ip);
    }
  }
  return i;
}

int add_connected(int socket_fd, process_ip_t process_ip_list[])
{
  char cur_ip[18];
  int i = 0;
  memset(&cur_ip, 0x00, 18);

  get_client_ip(socket_fd, cur_ip);
  //add client ip and process_id to process_ip_list
  for(i=0; i< MAX_CONNECTS; i++){
    if(process_ip_list[i].is_used == 0){
      strcpy(process_ip_list[i].ip, cur_ip);
      process_ip_list[i].is_used = 1;
      break;
    }
  }

  return i;
}

int remove_connected(int process_id)
{
  int i = 0;

  //remove client ip from process_ip_list
  i = 0;
  for(i; i< MAX_CONNECTS; i++){
    if(process_ip_list[i].process_id  == process_id){
      memset(process_ip_list[i].ip, 0x00, 18);
      process_ip_list[i].process_id = 0;
      process_ip_list[i].is_used = 0;
      break;
    }
  }
  
  return 0;
}
