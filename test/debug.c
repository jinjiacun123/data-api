#define _DEBUG_TRACE_CMH_2
#if 0 != _DEBUG_TRACE_CMH_
    #include<stdio.h>
#endif

#define FMT(p) "%s(%d)-<%s>:"#p
/*
#if 1==_DEBUG_TRACE_CMH_
  #define TRACE_CMH_printf
#elif 2==_DEBUG_TRACE_CMH_
  #define TRACE_CMH(fmt, ...) printf("%s(%d):"##fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#elif 3==_DEBUG_TRACE_CMH_
*/
#define TRACE_CMH(fmt, ...) printf("%s(%d)-<%s>:"#fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
/*
#else
  #define TRACE_CMH
#endif
*/

int
main(){
	TRACE_CMH("%s","test");
	return 0;
}
