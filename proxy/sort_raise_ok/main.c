#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include "comm.h"
#include "market.h"
#include "./../comm_pipe.h"

int last_time_market;//effective time
int cur_time;        //current time
int heart_times = 0;
bool may_show_sort = false;
app_request_t app_list[APP_SIZE] = {0};
pthread_mutex_t work_mutex;
pthread_mutex_t send_sort_mutex;
pthread_cond_t allow_start_app    = PTHREAD_COND_INITIALIZER;
pthread_cond_t allow_display_sort = PTHREAD_COND_INITIALIZER;

//buff
char * g_buff = NULL;
#ifdef system_32
unsigned long g_buff_max_len = 0;
unsigned long g_buff_len = 0;
#else
unsigned int g_buff_max_len = 0;
unsigned int g_buff_len = 0;
#endif
char * g_zib_buff = NULL;
uLong g_zib_buff_max_len = 0;
uLong g_zib_buff_len = 0;

//simulate
bool is_simulate = false;

static void do_stock(market_t * my_market, unsigned short, char *, char *, int, option_n);
static int send_sort(app_request_t * my_app);
static int init_login(int socket_fd);
static int write_data(char * buff, uLongf buff_len);

bool is_exit = false;
int socket_fd = 0;

market_t market_list[] = {
  //上证a股
  {"1101.txt","20161012",0x1101,1000,"[570-690][780-900][-1--1][-1--1]"},
  //深证a股
  {"1201.txt","20161012",0x1201,1000,"[570-690][780-900][-1--1][-1--1]"}
};

int main()
{
  pthread_t p_socket_id = 0, p_sort_id = 0, p_app_id = 0;
  int ret = 0;
  int test_times = 0;
  void * recycle;
  //  is_simulate = true;

#ifndef WIN32
  sigset_t signal_mask;
  sigemptyset(&signal_mask);
  sigaddset(&signal_mask, SIGPIPE);
  int rc = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
  if(rc != 0){
    DEBUG("error:[%s]", "block sigpipe error!");
  }
#endif

  //char program[1024] = {0};
  //snprintf(program, 1023, "main_%d.prof" ,getpid());
  //ProfilerStart(program);
  signal(SIGINT, sig_stop);
  //signal(SIGPIPE, sig_pipe);
  //dynamic show sort area
  ret = pthread_create(&p_sort_id, NULL, (void *)write_app, NULL);
  assert( ret == 0);
  //printf("init_sort_display ret:%d\n", ret);
  //init app
  ret = pthread_create(&p_app_id, NULL, (void *)init_app, NULL);
  assert (ret == 0);
  //printf("init app ret:%d\n", ret);
  //--receive both shanghhai and shenzhen market stock
  ret = init_market();
  DEBUG("info:[ret:%d]", ret);
  assert(ret == 0);
  ret = init_socket(&socket_fd);
  assert(ret == 0);

  //init server's login
  ret = init_login(socket_fd);
  assert( ret == 0);

  //init mutext
  ret = pthread_mutex_init(&work_mutex, NULL);
  assert( ret == 0);
  ret = pthread_mutex_init(&send_sort_mutex, NULL);
  assert( ret == 0);
  if(ret != 0){
    perror("Mutext initialization failed");
    exit(-1);
  }
  ret = pthread_create(&p_socket_id, NULL, (void *)init_receive, (void *)&socket_fd);
  assert( ret == 0);
  //printf("init_receive ret:%d\n", ret);
  //---init data and sort
  //get realtime data
  ret = send_realtime(socket_fd, 0, market_list[0].entity_list_size, 0);
  assert(ret == 0);
  //send_realtime(socket_fd, 0, 100, 0);
  //---auto push data---
  //sleep(4);
  //get auto push data and resort data
  //ret = send_auto_push(socket_fd, 0, market_list[0].entity_list_size, 0);
  //assert(ret == 0);
  //send_auto_push(socket_fd, 0, 20, 0);

  int menu = 1;
  while(true){
    sleep(3);
    ret = send_heart(socket_fd);
    assert(ret == 0);

    if(is_simulate){
      ret = send_realtime(socket_fd, 0, market_list[0].entity_list_size, 0);
      assert(ret == 0);
      sleep(5);
    }
    heart_times++;
    test_times++;
    //if(is_exit) break;
    // if(test_times > 1000) break;
  }

  //ProfilerStop();
  pthread_join(p_socket_id, &recycle);
  //pthread_join(p_sort_id, &recycle);
  pthread_join(p_app_id, &recycle);
  pthread_mutex_destroy(&work_mutex);
  pthread_cond_destroy(&allow_start_app);
  pthread_cond_destroy(&allow_display_sort);
  DEBUG("info:[%s]", "exit system");
  return 0;
}

int get_content(char * filename, char * buff, int buff_len)
{
  int fd;
  fd = open(filename, O_RDONLY);
  if( fd == -1){
    DEBUG("error:[%s]", "open err!");
    exit(-1);
  }

  int ret = 0;
  int length = 0;
  while(1){
    ret = read( fd, buff, buff_len);
    if(ret == -1){
      DEBUG("error:[%s]", "read err!");
      exit(-1);
    }
    length += ret;
    //   printf("buff:%s\n", buff);
    if(ret == 0){
      break;
    }
  }

  close(fd);
  return length;
}

int init_socket(int * socket_fd)
{
  int c_len = 0;
  struct sockaddr_in cli;

  cli.sin_family = AF_INET;
  cli.sin_port = htons(SERVER_MARKET_PORT);
  cli.sin_addr.s_addr = inet_addr(SERVER_MARKET);

  *socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(*socket_fd < 0){
    DEBUG("info:[%s]","socket() failrue!");
    return -1;
  }

  if(connect(*socket_fd, (struct sockaddr*)&cli, sizeof(cli)) < 0){
    DEBUG("error:[%s]", "connect() failure!");
    return -1;
  }
  DEBUG("info:[%s]", "connect() success");
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
  DEBUG("info:[%s]", "send login info!");

  //response login
  p_response_s_t  p_response_s;
  char * buff;
  int buff_len = 8;
  unsigned short type;
  assert(buff = (char *)malloc(buff_len+1));
  memset(buff, 0x00, buff_len);
  int n = read(proxy_client_fd, buff, buff_len);
  DEBUG("info:[%s]", "recive login info!");
  if(n == 8){
    p_response_s = (p_response_s_t)buff;
    //parse head
    if(strncmp(p_response_s->header_name, HEADER, 4)){
      DEBUG("error:[%s]", "recive login head of info err!");
      exit(-1);
    }
    buff_len = p_response_s->body_len;
    free(buff);
    assert(buff = (char *)malloc(buff_len+1));

    memset(buff, 0x00, buff_len+1);
    n = read(proxy_client_fd, buff, buff_len);
    if(n != buff_len){
      DEBUG("error:[%s]", "recive login body's length of info err!");
      exit(-1);
    }

    type = *((unsigned short *)buff);
    if(type != TYPE_LOGIN){
      DEBUG("error:[%s]", "recive login body's type of info err!");
      exit(-1);
    }

    DEBUG("info:[%s]", "recive login info check ok!");
    return 0;
  }
  else{
    DEBUG("error:[%s]", "recive login info err!");
    exit(-1);
  }
  return -1;
}

void reset_socket(int * sock_fd)
{
  int ret = 0;
  close(socket_fd);
  ret = init_socket(socket_fd);
  assert( ret == 0);
  //send auto_push
  ret = send_auto_push(socket_fd, 0, market_list[0].entity_list_size, 0);
  assert(ret == 0);
}

int send_realtime(int socket_fd, int index, int size, int code_type_index)
{
  char * request;
  int entity_count = market_list[0].entity_list_size;
  int off = sizeof(RealPack);
  int codeinfo_length = sizeof(CodeInfo);
  CodeInfo * codeinfo;
  entity_t * entity;
  int entity_length = sizeof(entity_t);

  request = (char *)malloc(sizeof(RealPack) + codeinfo_length*size);
  if(request == NULL){
    DEBUG("error:[%s]", "malloc err!");
    exit(-1);
  }
  int request_length = sizeof(RealPack)+ codeinfo_length * size;

  if(request == NULL){
    DEBUG("error:[%s]", "malloc err!");
    exit(-1);
  }
  memset(request, 0x00, sizeof(RealPack)+ codeinfo_length*size);

  RealPack * data = (RealPack*)request;
  memcpy(data->m_head, HEADER, 4);
  data->m_length =  request_length - 8;
  data->m_nType = TYPE_REALTIME;
  data->m_nSize = size;
  data->m_nOption= 0x0080;

  //股票:pass
  int i=0;
  entity = (entity_t *)(market_list[0].list+i*entity_length);
  for(i=index; i<(index+1)*size; i++){
    codeinfo = (CodeInfo *)(request+off+i*codeinfo_length);
    codeinfo->code_type = entity->type;
    strncpy(codeinfo->code, entity->code, 6);
    entity ++;
  }

  if(send(socket_fd, request, request_length, 0)){
    DEBUG("info:[%s]", "send success!");
    return 0;
  }

  return -1;
}

void init_receive(void * socket_fd)
{
  char head[4];
  char head_buff[9];
  char * buff = NULL;
  int package_body_length = 0;
  int ret_count = 0;
  int head_length = 8;
  int length = head_length;
  int off = 0;
  int fd = *((int *)socket_fd);
  int buff_len = 0;
  int max_buff = 1;

   while(1){
     memset(&head, 0x00, 4);
     memset(&head_buff, 0x00, 8);
     //接受头部
     ret_count = read(fd, head_buff, head_length);
     //printf("fd:%d\n", fd);
     if(ret_count == 0){
       DEBUG("info:[%s]", "connect close!");
       sleep(3);
       return 0;
     }
     else if(ret_count <0){
       DEBUG("error:[%s]", "recive server err!");
	   exit(-1);
       //reset_socket(socket_fd);
       //sleep(3);
       return 0;
     }else if(ret_count == head_length){
       off = 0;
       //receive body of package
       strncpy(head, head_buff, 4);
       length = *((int *)(head_buff+4));
       buff_len = length;
       if(g_buff_len == 0){
	 g_buff = (char *)malloc(length);
	 if(g_buff == NULL){
	   DEBUG("error:[%s]", "malloc err!");
	   exit(-1);
	 }
	 memset(g_buff, 0x00, length);
	 g_buff_len = length;
	 g_buff_max_len = length;
       }else{
	 if(length > g_buff_max_len){
	   free(g_buff);
	   //remalloc
	   g_buff = (char *)malloc(length);
	   if(g_buff == NULL){
	     DEBUG("error:[%s]", "malloc err!");
	     exit(-1);
	   }
	   memset(g_buff, 0x00, length);
	   g_buff_len = length;
	   g_buff_max_len = length;
	 }else{
	   //clean
	   memset(g_buff, 0x00, g_buff_len);
	   g_buff_len = length;
	 }
       }
       while(length != (ret_count = read(fd, g_buff+off, length))){
	 off += ret_count;
	 length -= ret_count;
       }
       //parse
       //printf("%s\n", buff);
       //printf("recive complete!\n");
       parse(g_buff, buff_len);
     }
   }
}

void init_sort_display(void * param)
{
  int i = 0, j = 0;
  int begin = 1000;
  int res = 0;
  const char * fifo_name = "" ;
  int p_id = -1;

  while(1){
    if(may_show_sort){
      //send to app
      for(j = 0; j < APP_SIZE; j++){
	if(app_list[j].app_fifo_fd > 0 && app_list[j].is_create == false){
	  //begin new pthread
	  res = pthread_create(&p_id, NULL, write_app, (void *)&app_list[i]);
	  assert(res == 0);
	  app_list[j].is_create = true;
	  continue;
	}
      }
      DEBUG("info:[%s]", "display show complete");
      sleep(2);
    }else{
      sleep(3);
    }
  }

  for(j = 0; j < APP_SIZE; j++){
    if(app_list[i].app_fifo_fd > 0){
      close(app_list[i].app_fifo_fd);
      app_list[i].pid = 0;
    }
  }
}

void init_app(void *param)
{
  pthread_mutex_lock(&work_mutex);
  pthread_cond_wait(&allow_start_app, &work_mutex);
  pthread_mutex_unlock(&work_mutex);

  int fifo_fd = -1;
  int res = 0;
  int i = 0;
  int app_request_len = sizeof(app_request_t);
  char app_request_buff[app_request_len];
  int app_fifo_fd = 0;
  char *template = PRIVATE_PIPE_TEMPLATE;
  char app_fifo_name[100] = {0};
  app_request_t * my_app = NULL, *tmp_app = NULL;
  bool is_exists = false;
  int ret = -1;
  int max_time = 0;

  struct timeval start_time;
  struct timeval end_time;
  struct timeval diff_time;

  //open fifo
  fifo_fd = open(PUBLIC_PIPE, O_RDWR|O_NONBLOCK);
  if(fifo_fd == -1){
    DEBUG("error:[%s]", "open pipe error!");
    exit(-1);
  }

  while(true){
    is_exists = false;
    res = read(fifo_fd, app_request_buff, app_request_len);
    if(res < 0){
      if(errno == EAGAIN){
	usleep(500);
	continue;
      }
      DEBUG("error:[%s]", "receive client app request error!");
      exit(-1);
      //continue;
    }else if(res == 0){
      continue;
    }else{
      //check is exists
      tmp_app = (app_request_t*)app_request_buff;
      DEBUG("info:[index:%d]", tmp_app->index);
      for(i = 0; i< APP_SIZE; i++){
	if(app_list[i].pid >0){
	  if(kill(app_list[i].pid, 0) != 0){
	    DEBUG("info:[close pid:%d]", app_list[i].pid);
	    //delete pipe
	    ret = snprintf(app_fifo_name, 100, template, app_list[i].pid);
	    assert(res > 0);
	    ret = unlink(app_fifo_name);
	    DEBUG("info:[delete fifo pid:%d retL%d]", app_list[i].pid);
	    close(app_list[i].app_fifo_fd);
	    app_list[i].pid = 0;
	    app_list[i].app_fifo_fd = -1;
	    app_list[i].begin = 0;
	    app_list[i].size  = 0;
	    continue;
	  }
	}
	if((tmp_app->pid == app_list[i].pid)
	   && (tmp_app->option == app_list[i].option)){
	  is_exists = true;
	  app_list[i].begin = tmp_app->begin;
	  app_list[i].size  = tmp_app->size;
	  if(app_list[i].size > 10){
		app_list[i].size = 10;
	  }
	  my_app = &app_list[i];
	  gettimeofday(&start_time, 0);
	  res = send_sort(my_app);
	  assert(res == 0);
	  gettimeofday(&end_time, 0);
	  timeval_subtract(&diff_time, &start_time, &end_time);
	  printf("总计用时:%d微妙\n", diff_time.tv_usec);
	  if(max_time < diff_time.tv_usec)
	    max_time = diff_time.tv_usec;
	  printf("到目前为止最大时间:%d\n", max_time);
	  break;
	}
      }
      if(!is_exists){
	//not exists
	for(i = 0; i< APP_SIZE; i++){
	  my_app = &app_list[i];
	  //i = i % APP_SIZE;
	  if(my_app->pid == 0){
	    memset(&app_fifo_name, 0x00, 100);
	    //get
	    *my_app = *((app_request_t*)app_request_buff);
	    snprintf(app_fifo_name, 100, template, my_app->pid);
	    //open fifo
	    app_fifo_fd = open(app_fifo_name, O_WRONLY|O_NONBLOCK);
	    if(app_fifo_fd == -1){
	      // printf("open %s error!\n");
	      app_list[i].pid = 0;
	      app_list[i].app_fifo_fd = 0;
	      app_list[i].is_create = false;
	      break;
	    }
	    my_app->app_fifo_fd = app_fifo_fd;
	    DEBUG("info[pid:%d]", my_app->pid);
	    res = send_sort(my_app);
	    assert(res == 0);
	    break;
	  }
	}
      }
    }
  }

  close(fifo_fd);
}

void write_app(void *param)
{
  /*
  int res = -1;
  entity_t entity_list[SORT_SHOW_MAX_NUM];
  market_t * my_market = NULL;
  int i = 0;
  app_request_t * my_app = NULL;
  char price[10];

   while(true){
     pthread_mutex_lock(&work_mutex);
     pthread_cond_wait(&allow_display_sort, &work_mutex);
     for(i = 0; i < APP_SIZE; i++){
       my_app = &app_list[i];
       if(my_app->app_fifo_fd>0){
	 send_sort(my_app);
       }
     }
     pthread_mutex_unlock(&work_mutex);
   }
  */
}

static int send_sort(app_request_t * my_app)
{
  int res = 0;
  market_t * my_market = NULL;
  //entity_t entity_list[SORT_SHOW_MAX_NUM];
  int entity_list_size = 0;
  int head_off = 8;
  char t_buff[8 + sizeof(entity_t)* SORT_SHOW_MAX_NUM] = {0};
  bool is_allow_send = true;
  char *template = PRIVATE_PIPE_TEMPLATE;
  int ret = 0;
  char app_fifo_name[100] = {0};

  if(pthread_mutex_trylock(&send_sort_mutex) == 0){
    if(my_app->pid > 0){
      if(kill(my_app->pid, 0) != 0){
	is_allow_send = false;
	DEBUG("info:[close pid:%d]", my_app->pid);
	//delete pipe
	ret = snprintf(app_fifo_name, 100, template, my_app->pid);
	assert(ret > 0);
	ret = unlink(app_fifo_name);
	DEBUG("info:[delete fifo pid:%d retL%d]", my_app->pid);
	close(my_app->app_fifo_fd);
	my_app->pid = 0;
	my_app->app_fifo_fd = -1;
	my_app->begin = 0;
	my_app->size  = 0;
	pthread_mutex_unlock(&send_sort_mutex);
	return 0;
      }
    }

    if(my_app->app_fifo_fd >0 && my_app->pid > 0){
      //write app pipe
      my_market = &market_list[0];
      entity_list_size = my_market->entity_list_size;
      //memset(&entity_list, 0x00, SORT_SHOW_MAX_NUM * sizeof(entity_t));
      if(my_app->begin < entity_list_size
	 && my_app->begin + my_app->size < entity_list_size){
	res = sort_get(my_market,
		       my_app->column,
		       my_app->begin,
		       my_app->size,
		       t_buff + head_off);
      }else if(my_app->begin < entity_list_size){
	res = sort_get(my_market,
		       my_app->column,
		       my_app->begin,
		       entity_list_size - my_app->begin,
		       t_buff + head_off);
      }
      assert(res == 0);
      memcpy(t_buff, &my_app->option, 4);
      memcpy(t_buff+4, &my_app->begin, 4);
      //memcpy(t_buff+4, &entity_list, my_app->size*sizeof(entity_t));
      res = write(my_app->app_fifo_fd, &t_buff, my_app->size*sizeof(entity_t)+head_off);
      if(res == -1){
	DEBUG("error:[%s]", "write app fifo err!");
	//close pipe
	close(my_app->app_fifo_fd);
	my_app->app_fifo_fd = 0;
	my_app->pid = 0;
	my_app->begin = 0;
	my_app->size = 0;
	my_app->is_create = false;
      }
    }
    pthread_mutex_unlock(&send_sort_mutex);
  }
  return 0;
}

int send_auto_push(int socket_fd, int index, int size, int code_type_index)
{
  char * request;
  int off = sizeof(RealPack_ex);
  int codeinfo_length = sizeof(CodeInfo);
  CodeInfo * codeinfo;
  entity_t * entity;
  int entity_length = sizeof(entity_t);
  int i = 0;
  int request_length = sizeof(RealPack_ex)+codeinfo_length*size;

  assert(jim_malloc(request_length, &request) == 0);

  RealPack_ex * data = (RealPack_ex *)request;
  memcpy(data->m_head,HEADER,4);
  data->m_length =  request_length-8;
  data->m_nType = TYPE_AUTO_PUSH;
  data->m_nSize = size;
  data->m_nOption= 0x0080;

  entity = (entity_t *)(market_list[code_type_index].list+i*entity_length);
  for(i = index; i<(index+1)*size; i++){
    codeinfo = (CodeInfo *)(request + off + i * codeinfo_length);
    codeinfo->code_type = entity->type;
    strncpy(codeinfo->code, entity->code, 6);
    entity ++;
  }

  if(send(socket_fd, request, request_length, 0)){
    //printf("send auto_push success!\n");
    return 0;
  }

  return -1;
}

int send_heart(int socket_fd)
{
  char request[1024];
  TestSrvData2 data ;
  memset(&data,0x00,sizeof(TestSrvData2));
  memcpy(data.head, HEADER,4);
  data .length      = sizeof(TestSrvData2) -8;
  data.m_nType     = TYPE_HEART;
  data.m_nIndex= 1;

  memset(request, 0, 1024);
  memcpy(request, &data, sizeof(data));
  send(socket_fd, request, sizeof(data), 0);
  //printf("send heart message...\n");
  return 0;
}
int option_times = 0;
int parse(char * buff, uLongf  buff_len)
{
  unsigned short type;
  int res = -1;

  if(buff == NULL)
    return -2;
  type = (*(unsigned short *)buff);
  //printf("type:%02x\n", type);
  switch(type){
  case TYPE_REALTIME:{
    //printf("realtime...\n");
    res = parse_realtime(buff, buff_len);
    assert( res == 0);
    //may display sort
    if(!is_simulate){
      pthread_mutex_lock(&work_mutex);
      pthread_cond_signal(&allow_start_app);
      pthread_mutex_unlock(&work_mutex);
    }
    //sort
    column_n sort_column = NEW_PRICE;
    //is_exit = true;
    if(is_simulate){
      pthread_mutex_lock(&work_mutex);
      //pthread_cond_signal(&allow_display_sort);
      pthread_cond_signal(&allow_start_app);
      pthread_mutex_unlock(&work_mutex);
    }
    option_times ++;
    DEBUG("info:[option_times:%d]", option_times);
    sleep(3);
    // is_simulate = true;
    //res = send_auto_push(socket_fd, 0, market_list[0].entity_list_size, 0);
    //assert(res == 0);
  }break;
  case TYPE_AUTO_PUSH:{
    //printf("recieve auto_push...\n");
    res = parse_auto_push(buff, buff_len);
    assert( res == 0);
    pthread_mutex_lock(&work_mutex);
    pthread_cond_signal(&allow_display_sort);
    pthread_mutex_unlock(&work_mutex);
    option_times ++;
    DEBUG("info:[option_times:%d]", option_times);
  }break;
  case TYPE_HEART:{
    //printf("heart...\n");
    heart_times--;
  }break;
  case TYPE_ZIB:{
    //printf("bzib...\n");
    res = unpack(buff, buff_len);
    if(res == -4){
      res = write_data(buff, buff_len);
      assert(res == 0);
    }else{
      assert( res == 0);
      res = parse(g_zib_buff, g_zib_buff_len);
      assert( res == 0);
    }
  }break;
  default:{
    DEBUG("info:[unknown type:%d]", type);
  }break;
  }
  return 0;
}

static int write_data(char * buff, uLongf buff_len){
  int fd = -1;
  char file_name[100] = {0};
  #define SIMULATE_FILE "./data/%d.data"
  int ret = -1;
  uLongf off = 0;
  char head[8] = {0};

  //calculate current time
  int seconds;
  seconds = time((time_t*)NULL);
  ret = snprintf(file_name, 100, SIMULATE_FILE, seconds);
  if((fd = open(file_name, O_WRONLY | O_CREAT,  S_IRUSR | S_IWUSR)) == -1){
    printf("open simulate file err!\n");
    exit(-1);
  }

  memcpy(head, HEADER, 4);
  memcpy(head+4, &buff_len, 4);
  ret = write(fd, &head, 8);
  if(ret == -1){
    printf("write head err!\n");
    return -1;
  }

  while(ret == write(fd, buff+off, buff_len-off)){
    if(ret == -1){
	break;
   }
   else if(ret >0){
	off += ret;
  }
  }

  close(fd);
  printf("%d write finish\n", seconds);
  return 0;
}

int parse_realtime(char * buff, uLongf buff_len)
{
  AskData2 * data_head = (AskData2 *)(buff);
  char code[7]={0};
  int i= 0;
  int index = 0;
  market_t * my_market = NULL;
  int code_len = 0;

  for(i=0; i< data_head->m_nSize; i++){
    memset(&code, 0x00, 7);
    CommRealTimeData * data_type = (CommRealTimeData *)(buff
							+ 20
							+ i*(sizeof(CommRealTimeData)+sizeof(HSStockRealTime)));
    //code_len = strlen(data_type->m_cCode);
    strncpy(code, data_type->m_cCode, 6);
    code[6] = '\0';
    //printf("code:%s\n", code);
    switch(data_type->m_cCodeType){
    case 0x1101:{
      my_market = &market_list[index];
      do_stock(my_market, data_type->m_cCodeType, code, buff, i, ADD);
    }break;
    case 0x1201:{
      my_market = &market_list[index];
      do_stock(my_market, data_type->m_cCodeType, code, buff, i, ADD);
    }break;
    case 0x1206:{
      my_market = &market_list[index];
      do_stock(my_market, data_type->m_cCodeType, code, buff, i, ADD);
    }break;
    case 0x120b:{
      my_market = &market_list[index];
      do_stock(my_market, data_type->m_cCodeType, code, buff, i, ADD);
    }break;
    }
  }
  return 0;
}

//处理股票
static void
do_stock(my_market, code_type, code, buff, i, option)
     market_t * my_market;
     unsigned short code_type;
     char * code;
     char * buff;
     int i;
     option_n option;
{
  void * address = NULL;
  unsigned int code_type_index = 0;
  entity_t * entity;
  int tmp_range = 0;
  // column_n column = NEW_PRICE;
  switch(code_type){
  case 0x110:{
    code_type_index = 0;
  }break;
  case 0x1201:{
    code_type_index = 1;
  }break;
  case 0x1206:{
    code_type_index = 2;
  }break;
  case 0x120b:{
    code_type_index = 3;
  }break;
  }
  address = find_entity_by_key(code, 6, code_type_index);
  assert(address != NULL);
  entity = (entity_t *)address;

  HSStockRealTime * tmp = (HSStockRealTime *)(buff
					      +20
					      +sizeof(CommRealTimeData)
					      +i*(sizeof(CommRealTimeData)+sizeof(HSStockRealTime)));

  entity->price       = tmp->m_lNewPrice;
  if(entity->price == 0){
    entity->raise = 0;
    entity->range = 0;
  }
  else{
    entity->raise  = entity->price - entity->pre_close;
    //entity->range = round(entity->raise * 100000 * 1.0 /entity->pre_close); 
    tmp_range  = round(entity->raise * 100000 * 1.0 / entity->pre_close);
    
    if(entity->raise * tmp_range < 0){
      tmp_range = round((unsigned int)entity->raise * 100000 * 1.0 / entity->pre_close);
      if(entity->raise < 0){
        entity->range = tmp_range * (-1);
      }
      else{
        entity->range = tmp_range;
      }
    }
    else{
      entity->range = tmp_range;
    }
     
    //entity->range  = ceil(entity->raise *10000*1.0) / entity->pre_close;
  }
  entity->max         = tmp->m_lMaxPrice;
  entity->min         = tmp->m_lMinPrice;
  entity->total       = tmp->m_lTotal;
  entity->money       = tmp->m_fAvgPrice;
  DEBUG("info:[index:%d,code_type:%2x,code:%s,pre_close:%d,new_price:%d,raise:%d,range:%d,tmp_range:%d]",i, code_type, code, entity->pre_close, entity->price, entity->raise, entity->range,tmp_range);
  if(is_simulate){
    srand(time(0));
    entity->price = tmp->m_lNewPrice + (rand()%10);
    option = UPDATE;
  }
  switch(option){
  case ADD:{
    //add to sort
    sort_add(my_market, entity, NEW_PRICE);
    sort_add(my_market, entity, RAISE);
    sort_add(my_market, entity, RANGE);
  }break;
  case UPDATE:{
    //update
    sort_update(my_market, entity, NEW_PRICE);
    sort_update(my_market, entity, RAISE);
    sort_update(my_market, entity, RANGE);
  }break;
  default:{

  }
  }
}

int parse_auto_push(char * buff, uLong   buff_len)
{
  //printf("parse auto_push...\n");
  AskData2 * data_head = (AskData2 *)(buff);
  char code[7]={0};
  int i = 0;
  int index = 0;
  market_t * my_market = NULL;

  for(i=0; i< data_head->m_nSize; i++){
    CommRealTimeData * data_type = (CommRealTimeData *)(buff
							+ 20
							+ i*(sizeof(CommRealTimeData)+sizeof(HSStockRealTime)));
    memcpy(code, data_type->m_cCode, 6);
    switch(data_type->m_cCodeType){
    case 0x1101:{
      my_market = &market_list[index];
      //printf("code_type:%2x, code:%s\n", data_type->m_cCodeType, code);
      do_stock(my_market, data_type->m_cCodeType, code, buff, i, UPDATE);
    }
    case 0x1201:{
      my_market = &market_list[index];
      //printf("code_type:%2x, code:%s\n", data_type->m_cCodeType, code);
      do_stock(my_market, data_type->m_cCodeType, code, buff, i, UPDATE);
    }
    case 0x1206:{
      my_market = &market_list[index];
      //printf("code_type:%2x, code:%s\n", data_type->m_cCodeType, code);
      do_stock(my_market, data_type->m_cCodeType, code, buff, i, UPDATE);
    }
    case 0x120b:{
      my_market = &market_list[index];
      //printf("code_type:%2x, code:%s\n", data_type->m_cCodeType, code);
      do_stock(my_market, data_type->m_cCodeType, code, buff, i, UPDATE);
    }
  }
  }

  return 0;
}

int unpack(char * des_buff, uLongf des_buff_len)
{
  uLong src_length = 0;
  TransZipData2   * zheader;
  zheader = (TransZipData2 *)des_buff;
  if(zheader->m_nType != TYPE_ZIB){
    DEBUG("error:[%s]", "parse zlib package type error!");
    return -100;
  }

  src_length = (uLongf)zheader->m_lOrigLen;
  if(g_zib_buff_len == 0){
    g_zib_buff_len = src_length;
    g_zib_buff_max_len= src_length;
    g_zib_buff = (char *)malloc(g_zib_buff_len);
    assert(g_zib_buff != NULL);
    if(g_zib_buff == NULL){
      DEBUG("error:[%s]", "malloc err!");
      return -1;
    }
    memset(g_zib_buff, 0x00, g_zib_buff_len);
  }else{
    if(src_length > g_zib_buff_max_len){//remalloc
      free(g_zib_buff);
      g_zib_buff_len = src_length;
      g_zib_buff_max_len = src_length;
      g_zib_buff = (char *)malloc(g_zib_buff_len);
      assert(g_zib_buff != NULL);
      if(g_zib_buff == NULL){
	DEBUG("error:[%s]", "malloc err!");
	return -1;
      }
      memset(g_zib_buff, 0x00, g_zib_buff_len);
    }else{//clean
      memset(g_zib_buff, 0x00, g_zib_buff_len);
      g_zib_buff_len = src_length;
    }
  }


  int unzip =  uncompress((Bytef *)(g_zib_buff), &g_zib_buff_len,
			  (Bytef*)zheader->m_cData, (uLongf)zheader->m_lZipLen);
  if(unzip == Z_MEM_ERROR){
    DEBUG("error:[%s]", "memory not enough");
    return -2;
  }
  if(unzip == Z_BUF_ERROR){
    DEBUG("error:[%s]", "buff not enough!");
    return -3;
  }
  if(unzip == Z_DATA_ERROR){
    DEBUG("error:[%s]", "unpack data err!");
    return -4;
   }
  if(unzip == Z_OK
     && g_zib_buff_len == zheader->m_lOrigLen){
    DEBUG("info:[%s]", "unpack success!");
    return 0;
  }
  DEBUG("info:[unzip:%d]", unzip);
  return -1;
}

int get_index_by_code_ascii(char ascii)
{
  int base_digit = 0;
  int base_alphabet = 10;
  //digit
  if(ascii>=48 || ascii <=57){
    return base_digit+ascii-48;
  }

  //alphabet
  if(ascii>=65 || ascii <= 80){
    return base_alphabet+ascii-65;
  }

  return -1;
}

void sig_stop(int signo)
{
  DEBUG("info:[%s]", "abovt exit!");
  shutdown(socket_fd, SHUT_RDWR);
  //close(socket_fd);
  exit(-1);
}

void sig_pipe(int signo){
}
