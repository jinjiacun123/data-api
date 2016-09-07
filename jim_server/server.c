#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

char buff[1024]; 

void server(char *ip)
{
  int server_sockfd;//服务器端套接字
  int client_sockfd;//客户端套接字
  int len;
  struct sockaddr_in my_addr;   //服务器网络地址结构体
  struct sockaddr_in remote_addr; //客户端网络地址结构体
  int sin_size;
  int pid;    
  int ppid=0;//父进程id
  fd_set allset, reset;
  struct timeval timeout={3,0};
  int client[FD_SETSIZE];
  int i,maxfd, maxi, sockfd;

  memset(&my_addr,0,sizeof(my_addr)); //数据初始化--清零
  my_addr.sin_family=AF_INET; //设置为IP通信
  my_addr.sin_addr.s_addr=INADDR_ANY;//服务器IP地址--允许连接到所有本地地址上
  my_addr.sin_port=htons(8888); //服务器端口号
  int nready;

  /*创建服务器端套接字--IPv4协议，面向连接通信，TCP协议*/
  if((server_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
    {
      perror("socket");
      return 1;
    }

  /*将套接字绑定到服务器的网络地址上*/
  if (bind(server_sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0)
    {
      perror("bind");
      return 1;
    }

  /*监听连接请求--监听队列长度为5*/
  listen(server_sockfd,1000);

  sin_size=sizeof(struct sockaddr_in);
    
  maxfd = server_sockfd;
  maxi = -1;
  for(i=0; i< FD_SETSIZE; i++)
    {
      client[i] = -1;
    } 
  FD_ZERO(&allset); 
  FD_SET(server_sockfd, &allset);

  for(;;)
    {
      reset = allset;
      nready = select(maxfd+1, &reset, NULL, NULL, NULL); 
        
      if(FD_ISSET(server_sockfd, &reset))
        {
	  client_sockfd = accept(server_sockfd, (struct sockaddr *)&remote_addr, &sin_size);
          printf("a new connection...\n"); 
	  for(i=0; i< FD_SETSIZE; i++)
	    {
	      if(client[i] <0){
		client[i] = client_sockfd;
		break;
	      }
	    }   
	  if(i == FD_SETSIZE)
	    perror("too many clients");
            
	  FD_SET(client_sockfd, &allset);
	  if(client_sockfd > maxfd)
	    {
              maxfd = client_sockfd;
	    } 
	  if(i > maxi)
	    maxi = i;

	  if(--nready <=0)
	    continue;
        }
      
      for(i=0; i <= maxi; i++){
	if((sockfd = client[i]) <0)
	  continue;
	if(FD_ISSET(sockfd, &reset)){       
	  if(fork()==0)
	    {
	      	char head_buff[20];
		unsigned long package_length;
		 
		while(1){
	      		memset(head_buff, 0, 20);
			//get head of package
			if(recv(sockfd, head_buff, 20, 0)>0){
			   package_length = get_c_request_package_length(head_buff);						
			}
			memset(buff, 0, 1024);			
			//get data of request package
	      		if(recv(sockfd, buff, package_length, 0)>0){
	    			//strcpy(buff, struct_to_json());
				//proxy: deal
				//write client 
				write(sockfd, buff, sizeof(buff));
	      		}
		}  
	    }
	  //my_request = do_request(sockfd);
	  // do_response(my_request);
	  // close(sockfd);

	  if(--nready <=0)
	    break;    
	}
      }
    }

  close(server_sockfd);
}
