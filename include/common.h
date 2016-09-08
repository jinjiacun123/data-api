#ifndef __COMMON_H__
#define __COMMON_H__

#pragma pack(1)
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

#ifdef __IS_SERVER__
#define REMOTE_SERVER "116.226.241.36"
#else
#define REMOTE_SERVER "192.168.1.201"
#endif
#define REMOTE_PORT 9999

#define USERNAME "jrjvip_android"
#define PASSWORD "zjw_android"
#define HEADER   "ZJHR"

#define MAX_BUFF 1024*1024

typedef struct{
	char head[10];
	unsigned length;
}t_base_c_request_head;
#endif
