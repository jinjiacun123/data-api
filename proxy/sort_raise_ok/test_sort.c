#include<stdio.h>
#define N 5

int
main()
{
  int list[] = {2,3,4,8,4};
  int i = 0, j = 0;
  int swap = 0;
  for(i=0; i< N; i++){
    for(j= i+1; j<N; j++){
      if(list[j]< list[j-1]){
	swap = list[j];
	list[j] = list[j-1];
	list[j-1] = swap;
      }

    }
  }

  for(i= 0; i< N; i++){
    printf("%d\t", list[i]);
  }
  printf("\n");

  return 0;
}
