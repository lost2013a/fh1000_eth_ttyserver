////****************************************************************************//
//* Copyright (c) 2007, 府河电气有限公司                                      
//* All rights reserved.                                                     
//* 文件 ttyS1000.h                                               
//* 日期 2015-5-18        
//* 作者 rwp                 
//* 注释                                  
//****************************************************************************//
#ifndef KWSERVER_H
#define KWSERVER_H
#include <stdarg.h>
#include "../common/classafx.h"
#include "../common/comminterface.h"
#include <vector>
#include "../common/Report.h"
#include "ComUnit.h"
#include "syst.h"
using namespace std;

class  Kw104Server
{
public:
	Kw104Server(SYSTEM_INFO *pCom);
	~Kw104Server();
public:
	//启动监听服务线程
    int StartListenServer();
   	void CloseServer();
	unsigned short equipid() { return m_equipid;}

	int start();

	int Init(SYSTEM_INFO* syst);
public:
	 
	 void comServerFun();
	 int  newinf(unsigned char *pASDU,  unsigned short len);

     void AddCom(ComUnit * p);
     void DeleteCom(ComUnit * p);
private:
	static  void  comServerThread(void * lp);
    int readcfg();
	int addreport(unsigned char *pASDU,  unsigned short len);
	void WriteLog(int iLevel,const char *format, ...);
	int TranslateDLT1100(unsigned char* pASDU, unsigned int inleng, unsigned char* pDLT1100, unsigned int& outleng);
	bool LoginVerify();
private:

	CusomListenSocket *m_pListen;

	unsigned int m_dbconnectIndex;
    unsigned short m_ServerPort;
    _HANDLE m_listenhandle;
	bool  m_bExit;
    vector<ComUnit * > m_ComList; //通信列表
	CMyMutex  m_comMutex;		  //私有的同步锁
	
	bool           bAccept;       //是否第一次接收到产口发送来的数据 
	unsigned short m_equipid;     //装置编号

	int m_logLevel;

	SYSTEM_INFO* m_syst;
};

#endif // KWSERVER_H
