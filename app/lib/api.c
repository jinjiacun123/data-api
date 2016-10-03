#include "api.h"
//request simple entity of realtime
static int reqeust_simple_of_realtime();
//request multi entity of realtime
static int request_multi_of_realtime();
//request simple entity of auto_push
static int request_simple_of_auto_push();
//request multi entity of auto_push
static int request_multi_of_auto_push();
//request entity of history
static int request_of_history();
//request entity of time_share
static int requet_of_time_share();
//request zlib
static int request_of_zlib();

//response realtime
static int response_of_realtime();
//response auto_push
static int response_of_auto_push();
//response history
static int response_of_history();
//response time_share
static int response_of_time_share();
//response zlib
static int response_of_zlib();

//request------------------------------------------------------------
int request(type, entity[], size, package)
  const unsigned short type;
const entity_t entity[]; 
const int size; 
char * package;
{
  return 0;
}

//request simple entity of realtime
static int reqeust_simple_of_realtime()
{
  return 0;
}

//request multi entity of realtime
static int request_multi_of_realtime()
{
  return 0;
}

//request simple entity of auto_push
static int request_simple_of_auto_push()
{
  return 0;
}

//request multi entity of auto_push
static int request_multi_of_auto_push()
{
  return 0;
}

//request entity of history
static int request_of_history()
{
  return 0;
}

//request entity of time_share
static int requet_of_time_share()
{
  return 0;
}

//request zlib
static int request_of_zlib()
{
  return 0;
}
//reqeust------------------------------------------------------------

//response------------------------------------------------------------
int response(package, length, type, entity_info, size)
  const char * package;
  const int length;
  unsigned short * type;
  entity_info_t * entity_info;
  int size;
{
  return 0;
}
//response realtime
static int response_of_realtime()
{

}
//response auto_push
static int response_of_auto_push()
{

}
//response history
static int response_of_history()
{

}
//response time_share
static int response_of_time_share()
{

}
//response zlib
static int response_of_zlib()
{

}
//response------------------------------------------------------------
