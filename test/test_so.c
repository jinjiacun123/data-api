#include <stdio.h>   
#include <dlfcn.h>   
  
int main(void){   
   int (*myadd)(int a,int b);//fuction pointer   
   void *handle;   
      
   handle=dlopen("./libmyadd.so",RTLD_LAZY);//open lib file   
   myadd=dlsym(handle,"output");//call dlsym function   
      
  
   int result=myadd(1,2);   
   dlclose(handle);   
   printf("%d\n",result);     
}  
