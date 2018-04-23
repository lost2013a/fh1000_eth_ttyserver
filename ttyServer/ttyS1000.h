////****************************************************************************//
//* Copyright (c) 2007, ���ӵ������޹�˾                                      
//* All rights reserved.                                                     
//* �ļ� ttyS1000.h                                               
//* ���� 2015-5-18        
//* ���� rwp                 
//* ע��                                  
//****************************************************************************//
#ifndef TTYS1000_H
#define TTYS1000_H

#include <stdarg.h>
#include "../common/classafx.h"
#include <list> 
#include <string>
#include <stdio.h>
using namespace std;
#include <stdio.h>
#include <string>
#ifndef WIN32
#include <sys/time.h>
#include <time.h>
#endif
using namespace std;

#if defined(WIN32) || defined(_WIN32_WCE)

#else 
#include     <unistd.h>     /*Unix ��׼��������*/
#include     <sys/types.h>  
#include     <sys/time.h>
#include     <sys/stat.h>   
#include      <sys/ioctl.h>
#include     <fcntl.h>      /*�ļ����ƶ���*/
#include     <termios.h>    /*PPSIX �ն˿��ƶ���*/
#include     <errno.h>      /*����Ŷ���*/

#endif 

#include "syst.h"
#include "CComPort.h"


//Э����������
//TS20050221021538F50.0043C021535+003.78*1C
//����  TS20050221021538F50.0043C021535+003.78*1C
//<T><B>YYYYMMDDhhmmss<F>FF.F1F2F3F4<C>HHM1M2SSX CCC.CC	*HH<CR><LF>
#define MSG_CHAR_SYN 0x53  //'S'
#define MSG_CHAR_ASN 0x41  //'B'
#define MSG_CHAR_FRM 0x54  //'T'
#define MSG_CHAR_CYC 0x46  //'F'
#define MSG_CHAR_FRQ 0x43  //'C'
#define MSG_CHAR_SUM '*'   //*
#define MSG_CHAR_CR_ 0x0d  //�س�
#define MSG_CHAR_LF_ 0x0a  //����
const int msg_body_lengh = 41;

#define  FH1000LOGNAME    "fh1000log.txt"   //��־�ļ�����
class Cmanager;

typedef int (*FUNC_ASDU_HANDLER)(unsigned char * pASDU, unsigned char len,void* param);

class CttyS1000
{
public:
	CttyS1000(SYSTEM_INFO *pShell);
	~CttyS1000();

	bool set_serial(unsigned short  com_id, //���ڱ��
		unsigned int    baud,   //������
		unsigned short  addr,
		unsigned char  parity,
		unsigned char  databits,
		unsigned char  stopbits		
		);

	bool start();

	void register_asdu_handler(FUNC_ASDU_HANDLER pHandler,void* pCallObj);

	void stop();

private:
	FUNC_ASDU_HANDLER m_callbackASDUHandler;
	void*  m_callbackObj;
	void initVars();
	void  processfun();
	static	void  circleThread(void *pVoid);	               //Ѳ���߳�		  
	void    circleFun();    
	void	closeAllThread();

	bool create_thread(_HANDLE &handle ,ptrFun fun_name,void *fun_param);
	bool thread_exit(_HANDLE &handle);
	unsigned long comm_gettickcount();

	int ttysDataHandler(unsigned char * pASDU, unsigned char len);

	void WriteLog(int iLevel,const char *format, ...);
	int recvGPSMsg();
	//int recvMsg();
	void MessageLog(unsigned char *buf,int len,bool bSend);
	int modifyLocalTime(unsigned char* pASDU, unsigned int inleng);
private:
	int m_logLevel;
	int m_addr;
	CComPort* m_rCommPort;  //���ھ��
	int m_port;
	int m_parity; 
	int m_databits; 
	int m_stopbit;
	unsigned int m_speed;
	unsigned int m_mode;

	_HANDLE		  m_circleTheaddHandle;					   //Ѳ���̵߳ľ��
	bool           m_circleTheadExt;
	bool           m_bExit;

	SYSTEM_INFO *m_pShell; 

	int m_valflag;

	bool setlocalTimeFlag;
};




#endif// TTYS1000_H
