#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<signal.h>
#include "./../include/data.h"

#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)

void do_service(int);

void handler(int sig)
{
  /*  wait(NULL); //只能等待第一个退出的子进程 */
 
  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;
}


int main(void)
{
  //    signal(SIGCHLD, SIG_IGN);
    signal(SIGCHLD, handler);
    int listenfd; //被动套接字(文件描述符），即只可以ACCEPT, 监听套接字
    if ((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        //  listenfd = socket(AF_INET, SOCK_STREAM, 0)
        ERR_EXIT("socket error");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); */
    /* inet_aton("127.0.0.1", &servaddr.sin_addr); */

    int on = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt error");

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind error");

    if (listen(listenfd, SOMAXCONN) < 0) //listen应在socket和bind之后，而在accept之前
        ERR_EXIT("listen error");

    struct sockaddr_in peeraddr; //传出参数
    socklen_t peerlen = sizeof(peeraddr); //传入传出参数，必须有初始值
    int conn=0; // 已连接套接字(变为主动套接字，即可以主动connect)

    pid_t pid;

    while (1)
    {
        if ((conn = accept(listenfd, (struct sockaddr *)&peeraddr, &peerlen)) < 0) //3次握手完成的序列
            ERR_EXIT("accept error");
        printf("recv connect ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr),
               ntohs(peeraddr.sin_port));

        pid = fork();
        if (pid == -1)
            ERR_EXIT("fork error");
        if (pid == 0)
        {
            // 子进程
            close(listenfd);
	    if(conn>0){
	      do_service(conn);
	    }
            exit(EXIT_SUCCESS);
        }
        else
            close(conn); //父进程
    }

    return 0;
}

void do_service(int conn)
{
    server_package_t * package;
    int package_len = sizeof(struct server_package);
    package = (server_package_t *)malloc(package_len);
    assert(package);
    memset(package, 0, sizeof(package_len));

    server_request_t * req;
    int req_len = sizeof(struct server_request);
    req = (server_request_t *)malloc(req_len);
    assert(req);
    memset(req, 0, req_len);
    package->request = req;
    
    server_response_t * resp;
    int resp_len = sizeof(struct server_response);
    resp = (server_response_t *)malloc(resp_len);
    assert(resp);
    package->response = resp;

    package->client_fd = conn;
    while (1)
    {      
        memset(req->package_head, 0, PACKAGE_HEAD_LEN);
        int ret = read(conn, req->package_head, PACKAGE_HEAD_LEN);
        if (ret == 0)   //客户端关闭了
        {
            printf("client close\n");
	    break;
        }
        else if (ret == -1)
            ERR_EXIT("read head error");
	printf("head ret:%d\n", ret);

	req->package_body_len = 0;
	req->package_body_len = get_c_request_package_length(req->package_head);
	assert(req->package_body_len);
	printf("recv package head:%s\n", req->package_head);
	
	
	req->package_body = (char * )malloc(req->package_body_len+1);
	memset(req->package_body, 0, req->package_body_len+1);
	//memcpy(recvbuff, head_buff, PACKAGE_HEAD_LEN);
	ret = read(conn, req->package_body, req->package_body_len);
	if(ret == 0){
	  printf("client close\n");
          break;
	}
	else if(ret == -1)
	  ERR_EXIT("read body error");
	
	printf("recv package body:%s\n", req->package_body);


	do_client_request(package);
        //fputs(recvbuf, stdout);
        //write(conn, recvbuf, ret);
	
    }
}
