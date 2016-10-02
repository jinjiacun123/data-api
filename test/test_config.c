#include "confile.h"
 
void main(int argc,char* argv[]){
  INI_CONFIG* config;
  printf("------------test1-----------\n");
  config=ini_config_create_from_string("hover     =      lees       \n data =    7 \n maxthread=255\n;this is a comment line\r\nyahoo    =alibaba     \n     [section1]   \nhover   =  lees2  \nhover=lees333\n\n hover = lees4444\nyahoo=3\n\n\n",0,0);
  if(config){
    ini_config_print(config,stdout);
    ini_config_destroy(config);
  }
 
  printf("\n------------test2-----------\n");
  config=ini_config_create_from_file("php.ini",0);
  if(config){
    printf(
	   "%s %s\n",
	   ini_config_get_string(config,"soap","soap.wsdl_cache_dir","/nodir"),
	   ini_config_get_string(config,"soap","soap.wsdl_cache_ttl","xixi")
	   );
    ini_config_destroy(config);
  }
}
