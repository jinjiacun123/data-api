#include<stdio.h>
#include<assert.h>

int test_value(char ** value, int length)
{
 *value = (char *)malloc(length+1);
  if(*value == NULL){
    return -1;
  }
  memset(*value, 0x00, length+1);
  return 0;
}

int main()
{
  char * my_value = NULL;
  assert(test_value(&my_value, 10) == 0);
  printf("success");
  return 0;
}
