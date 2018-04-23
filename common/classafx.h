#ifndef CLASSAFX_H
#define CLASSAFX_H

#define Q_OS_UNIX
#define _DEBUG_			//调试


#if defined(WIN32) 
	
    #include <windows.h>
	#include <process.h>    // _beginthread, _endthread 
    #pragma   comment(lib,   "wsock32.lib")   

    #define sleep(msec) Sleep(msec)
#define _usleep(msec) Sleep(msec)

	   typedef  HANDLE _HANDLE;

	typedef  void (*ptrFun)( void*);
 
#elif defined(Q_OS_UNIX)

	#include <pthread.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <stdio.h>
	#include <string.h>
	#include <unistd.h>
	#include <sys/socket.h>

    #define _usleep(msec) usleep(msec)
	#define sleep(msec) usleep(1000*msec)

	typedef  pthread_t _HANDLE;
	typedef  void* (*ptrFun)( void*);

	#define INVALID_SOCKET  -1
#endif


	//定义简单表达式
#define MAX(x,y) (x)>(y)?(x):(y)
#define MIN(x,y) (x)<(y)?(x):(y)

#endif
