////****************************************************************************//
//* Copyright (c) 2007, ���ӵ������޹�˾                                      
//* All rights reserved.                                                     
//* �ļ� ttyS1000.h                                               
//* ���� 2015-5-18        
//* ���� rwp                 
//* ע��                                  
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
	//�������������߳�
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
    vector<ComUnit * > m_ComList; //ͨ���б�
	CMyMutex  m_comMutex;		  //˽�е�ͬ����
	
	bool           bAccept;       //�Ƿ��һ�ν��յ����ڷ����������� 
	unsigned short m_equipid;     //װ�ñ��

	int m_logLevel;

	SYSTEM_INFO* m_syst;
};

#endif // KWSERVER_H
