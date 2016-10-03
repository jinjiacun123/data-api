#include <stdio.h>
#include <stdlib.h>

#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
/*客户端*/
char *host_name ="127.0.0.1";//需要搜寻服务端IP地址
int port = 8001;//代理服务器端口号
//int port = 3333;

int main(int argc, char** argv){
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

  printf("\n Response from server:\n\n%s\n",buf);

  close(socket_descriptor);

  return (EXIT_SUCCESS);
}
