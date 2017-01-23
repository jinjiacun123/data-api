#define _DEBUG_TRACE_CMH_2
#if 0 != _DEBUG_TRACE_CMH_
    #include<stdio.h>
#endif
#include<stdarg.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#define FMT(p) "%s(%d)-<%s>:"#p
/*
#if 1==_DEBUG_TRACE_CMH_
  #define TRACE_CMH_printf
#elif 2==_DEBUG_TRACE_CMH_
  #define TRACE_CMH(fmt, ...) printf("%s(%d):"##fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#elif 3==_DEBUG_TRACE_CMH_
*/
//#define TRACE_CMH(fmt, ...) printf("%s(%d)-<%s>:"#fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define TRACE_CMH(fmt, ...) WriteErrLog("%s(%d)-<%s>:"#fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

/*
#else
  #define TRACE_CMH
#endif
*/

void WriteErrLog(const char *i_sFormat,...)
{
  va_list args;
  char  *sLogFile="/tmp/err.log";
  FILE  *fLogFile;
  time_t tWriteTime;
  char  sWriteTime[20] = {0};
  pid_t ThisPid;

  ThisPid=getpid();
  time(&tWriteTime);
  strftime(sWriteTime, 20, "%Y/%m/%d %H:%M:%S", localtime(&tWriteTime));

  if(sLogFile != NULL)
    fLogFile=fopen(sLogFile,"a+");

  if(sLogFile != NULL && fLogFile !=NULL)
    fprintf(fLogFile,"[%6d] %s: ",ThisPid, sWriteTime);
  va_start(args,i_sFormat);
  if(sLogFile != NULL && fLogFile !=NULL)
    vfprintf(fLogFile,i_sFormat,args);
  va_end(args);
  if(sLogFile != NULL && fLogFile != NULL) fclose(fLogFile);
}

int
main(){
	TRACE_CMH("%s","test");
	return 0;
}
