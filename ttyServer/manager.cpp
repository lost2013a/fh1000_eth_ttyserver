////****************************************************************************//
//* Copyright (c) 2007, ���ӵ������޹�˾                                      
//* All rights reserved.                                                     
//* �ļ� ttyS1000.h                                               
//* ���� 2015-5-18        
//* ���� rwp                 
//* ע��                                  
//****************************************************************************//
#include "manager.h"


Cmanager::Cmanager(unsigned short pgid)
{
	m_Pgid = pgid;
	m_bExit = false;
	m_logLevel= 3;
	m_pSerial = NULL;
	m_kwsrver = NULL;
}

void Cmanager::clearsource()
{
	if (m_pSerial != NULL)
	{
		m_pSerial->stop();
		delete m_pSerial;
		m_pSerial = NULL;
	}

	if (m_kwsrver != NULL)
	{
		m_kwsrver->CloseServer();
		delete m_kwsrver;
		m_kwsrver = NULL;
	}

}
Cmanager::~Cmanager(void)
{

	End();
}

int Cmanager::Start()
{
	CttyS1000Attach tempAttach;	//���ü�����
	tempAttach.Attach(&syst);	//ָ��ָ������ģ��
	if (!tempAttach.LoadModelFromXML("ttyS1000.xml")){
		WriteLog(1,"FH1000-ETH���ü���ʧ�ܣ�����Ĭ�ϲ���");
		/*�Զ�����һ��ģ�壬�Ա��޸Ĳ��ԣ���ʽ����ʱע��*/
		//attacher.SaveModelToXML("ttyS1000.xml");
	}
    else{
		WriteLog(1,"FH1000-ETH���ü��سɹ�(ttyS1000.xml)");
		if(m_logLevel == 3)
			tempAttach.Print(&syst);
	}
	
	if (0 != Inittty1000()){ /*��ʼ�����ڶ���*/
		WriteLog(1, "FH1000-ETH���ڷ����ʼ��ʧ��");
		return -1;
	}

	
	if (0 != InitcpSrvr()){	 /*��ʼ������������*/
		WriteLog(1, "FH1000-ETH��������ʼ��ʧ��");
		return -1;
	}
	WriteLog(1, "FH1000-ETH�����������");
	return 0;
}


void Cmanager::End()
{
	clearsource();
}



int Cmanager::Inittty1000()
{
	int ret(-1);
	m_pSerial = new CttyS1000(&syst);
	
	if(bool ret = m_pSerial->set_serial(syst.ttyInfo.comId,syst.ttyInfo.baudrate,0, 
		syst.ttyInfo.parity,syst.ttyInfo.databits,syst.ttyInfo.stopbits) == false)
	{
		delete m_pSerial;
		m_pSerial = NULL;
		return -1;
	}

	//ע�����ݴ���ص�
	m_pSerial->register_asdu_handler(dataHandler,this);

	if (!m_pSerial->start()){
		delete m_pSerial;
		m_pSerial = NULL;
		return -1;
	}
	
	return 0;
}




int Cmanager::InitcpSrvr()
{
	m_kwsrver = new Kw104Server(&syst);
	if (m_kwsrver){
		if (0 != m_kwsrver->Init(&syst)){
			WriteLog(1,"��ʼ��tcp������ʧ��");
			delete m_kwsrver;
			m_kwsrver = NULL;
			return -1;
		}

		if (0 != m_kwsrver->start()){
			WriteLog(1,"tcp����������ʧ��");
			delete m_kwsrver;
			m_kwsrver = NULL;
			return -1;
		}
		return 0;
	}
	return -2;
}

int Cmanager::tty1000Handler(unsigned char * ttysMsg, unsigned char len)
{
	if (ttysMsg)
	{
		if (m_kwsrver)		{
			m_kwsrver->newinf(ttysMsg,len);
		}
	}
	return 0;
}

int Cmanager::dataHandler(unsigned char * pASDU, unsigned char len,void* param)
{
	if (param)	{
		Cmanager* pThis = (Cmanager*)param;
		pThis->tty1000Handler(pASDU,len);
	}
	return 0;
}



void Cmanager::WriteLog(int iLevel,const char *format, ...)
{
	if (iLevel>m_logLevel){
		return;
	}	
#define LOGLENGTH 1024
	char buffer[LOGLENGTH];
  	va_list ap;
	va_start(ap,format);
	vsnprintf(buffer,LOGLENGTH,format,ap);//vsprintf�ĳ������ư�
	va_end(ap);
    if(1){	//����ʱ�����������CPU����
		struct timeval  tv;
	    struct timezone tz;
		gettimeofday(&tv,&tz);
		struct tm *tm = localtime(&tv.tv_sec);
		char time_prt[50]="";
		sprintf(time_prt,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d.%03ld ",
			tm->tm_year + 1900, tm->tm_mon + 1,tm->tm_mday, tm->tm_hour,
			tm->tm_min, tm->tm_sec,tv.tv_usec/1000);
		printf("[Cmanager] %s %s\n",time_prt,buffer);	
    }
	else{
		printf("[Cmanager] %s\n",buffer);	
	}
}

