#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<errno.h>

struct student{
	char name[10];
	unsigned int grade;
};

//global variable
int gnum = 0;

pthread_mutex_t mutex;

//func of thread
static void pthread_func_1(void);
static void pthread_func_2(void);
static void * pthread_func_3(void *);

int
main(void){
  pthread_t pt_1 = 0;
  pthread_t pt_2 = 0;
  int ret = 0;
  struct student s;

  pthread_mutex_init(&mutex, NULL);

  /*
  ret = pthread_create(&pt_1,
		       NULL,
		       (void*)pthread_func_1,
		       NULL);
  if(ret != 0){
    perror("pthread_1 create!\n");
  }
  
  ret = pthread_create(&pt_2,
		       NULL,
		       (void*)pthread_func_2,
		       NULL);
  if(ret != 0){
    perror("pthread_2 create!\n");
  }
  
  pthread_join(pt_1, NULL);
  pthread_join(pt_2, NULL);
 */
  strcpy(s.name, "jim");
  s.grade = 80;
  ret = pthread_create(&pt_1,
	               NULL,
	               (void*)pthread_func_3,
                       &s);
   if(ret != 0){
     perror("pthread_func_3 error!\n");
   }	
	
   while(1){
      sleep(3);
   }
  
  printf("main programming run!\n");
  return 0;

}


static void
pthread_func_1(void){
  int i = 0;

  for(i=0; i< 3; i++){
    printf("this is pthread_1\n");
    pthread_mutex_lock(&mutex);
    sleep(1);
    gnum++;
    printf("thread_1 add one num:%d\n", gnum);
    pthread_mutex_unlock(&mutex);
  }
  pthread_exit(NULL);
}

static void
pthread_func_2(void){
  int i=0;
  
  for(i=0; i<3; i++){
    printf("this is pthread_2\n");
    pthread_mutex_lock(&mutex);
    sleep(1);
    gnum++;
    printf("thead_2 add one num:%d\n", gnum);
    pthread_mutex_unlock(&mutex);
  }

  pthread_exit(0);
}

static void *
pthread_func_3(void * param)
{
  struct student * s = (struct student *)param;
  printf("name:%s,grade:%d\n", s->name, s->grade);	
  pthread_exit(0);
}
