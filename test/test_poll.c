#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include<errno.h>
#include<poll.h>

#define LISTENQ 1024
#define MAXLINE 1024
#define OPEN_MAX 50000
#define SERVER_PORT 3333

#ifndef INFTIM
#define INFTIM -1
#endif

int
main(int argc, char * argv[]){
  int i, maxi, listenfd, connfd, sockfd;
  int nready;
  ssize_t n;
  socklen_t clilen;
  struct sockaddr_in servaddr,cliaddr;
  struct hostent * hp;
  char buf[MAXLINE];
  struct pollfd client[OPEN_MAX];

  if(argc != 2){
    printf("please input %s<hostname>\n", argv[0]);
    exit(1);
  }

  if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) <0){
    printf("create socket error\n");
    exit(0);
  }
  
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  if((hp = gethostbyname(argv[1])) != NULL){
    bcopy(hp->h_addr, (struct sockaddr*)&servaddr.sin_addr, hp->h_length);
  }
  else if(inet_aton(argv[1], &servaddr.sin_addr) <0){
    printf("input server ip error!\n");
    exit(1);
  }
  
  servaddr.sin_port = htons(SERVER_PORT);

  if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) <0){
    printf("ip address bound failted!\n");
    exit(1);
  }

  listen(listenfd, LISTENQ);
  
  client[0].fd = listenfd;
  client[0].events = POLLIN;
  
  for(i=1; i<OPEN_MAX; i++){
    client[i].fd = -1;
  }

  maxi = 0;
  
  while(1){
    nready = poll(client, maxi+1, INFTIM);
    if(client[0].revents & POLLIN){
      clilen = sizeof(servaddr);
      connfd = accept(listenfd, (struct sockaddr *)&servaddr, &clilen);
      for(i=1; i<OPEN_MAX; i++){
	if(client[i].fd <0){
	  client[i].fd = connfd;
	  client[i].events = POLLIN;
	  break;
	}
      }
      
      if(i == OPEN_MAX){
	printf("too many clients\n");
	exit(1);
      }

      if(i>maxi)
	maxi = i;

      if(--nready <= 0)
	continue;
    }
    
    for(i=1; i <=maxi; i++){
      if((sockfd = client[i].fd)<0)
	continue;

      if(client[i].revents & (POLLIN|POLLERR)){
	if((n = read(sockfd, buf, MAXLINE)) <0){
	  if(errno == ECONNRESET){
	    close(sockfd);
	    client[i].fd = -1;
	  }
	  else{
	    perror("read error");
	  }
	}
	else if(n == 0){
	  close(sockfd);
	  client[i].fd = -1;
	}
	else{	  
	  printf("%s\n", buf);
	  write(sockfd, buf, n);	  
	}


	if(--nready <=0)
	  break;

      }      
    }
  }

  exit(0);
  return 0;
}
