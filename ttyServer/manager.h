////****************************************************************************//
//* Copyright (c) 2007, 府河电气有限公司                                      
//* All rights reserved.                                                     
//* 文件 ttyS1000.h                                               
//* 日期 2015-5-18        
//* 作者 rwp                 
//* 注释                                  
//****************************************************************************//
#ifndef MANAGER_H_
#define MANAGER_H_
#include "../common/Report.h"
#include "../common/classafx.h"
#include <map>
#include <string>
#include <stdarg.h>
#include "ttyS1000.h"
#include "kwserver.h"
#include "deviceAttach.h"

#define MULTICOMM_UDP_LEN  2048

using namespace  std;


class Cmanager
{
public:
	Cmanager(unsigned short pgid = 0);
	~Cmanager(void);
	int  Start();
	void End();
private:
	SYSTEM_INFO syst;
	void clearsource();  
	static  int dataHandler(unsigned char * pASDU, unsigned char len,void* param);
	int tty1000Handler(unsigned char * pASDU, unsigned char len);
	int Inittty1000();
	int InitcpSrvr();
	void WriteLog(int iLevel,const char *format, ...);
private:
	CttyS1000     *m_pSerial;  //fh1000串口协议
	Kw104Server   *m_kwsrver;  //私有类“可为”GPS协议
	bool           m_bExit;
	int 		   m_logLevel;
	unsigned short m_Pgid;
	unsigned short m_Port;
	string  m_strSericalParam; //串口参数
};

#endif
