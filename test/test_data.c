#include<stdio.h>
#include<assert.h>
#include "./../include/data.h"
void test_do_client_request();

int
main(){
  test_do_client_request();
}

void 
test_do_client_request(){
  server_package_t * package;
  server_request_t * request;
  server_response_t * response;

  package = (server_package_t *)malloc(sizeof(struct server_package));
  assert(package);
  memset(package, 0, sizeof(struct server_package));
  request = (server_request_t *)malloc(sizeof(struct server_request));
  assert(request);
  memset(request, 0, sizeof(struct server_request));
  response = (server_response_t *)malloc(sizeof(struct server_response));
  assert(response);
  memset(response, 0, sizeof(struct server_response));

  package->request = request;
  package->response = response;
  
  int client_fd = 0;
  //send package to server
  char package_str[1024];
  char * package_body_str = "{\"type\":\"000100010001\", \"data\":{\"code_type\":\"0x8100\",\"code\":\"EURUSD\"}}";
  int package_body_length = strlen(package_body_str);
  char * tmp = "{\"type\":\"000100010001\", \"length\":%d}";
  char package_head[50];

  // memset(package_head, 0, 50);
  assert(sprintf(request->package_head, tmp, package_body_length));

  //memset(package, 0, 1024);
  //memcpy(package, package_head, 50);
  //memcpy(package+50, package_body, package_body_length);
  request->package_body = (char *)malloc(package_body_length);
  memset(request->package_body, 0, package_body_length);
  memcpy(request->package_body, package_body_str, package_body_length);

  do_client_request(package);
}
