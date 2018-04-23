////****************************************************************************//
//* Copyright (c) 2007, 府河电气有限公司                                      
//* All rights reserved.                                                     
//* 文件 ttyS1000.h                                               
//* 日期 2015-5-18        
//* 作者 rwp                 
//* 注释                                  
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
	CttyS1000Attach tempAttach;	//配置加载器
	tempAttach.Attach(&syst);	//指针指向配置模型
	if (!tempAttach.LoadModelFromXML("ttyS1000.xml")){
		WriteLog(1,"FH1000-ETH配置加载失败，采用默认参数");
		/*自动生成一个模板，以便修改测试，正式运行时注释*/
		//attacher.SaveModelToXML("ttyS1000.xml");
	}
    else{
		WriteLog(1,"FH1000-ETH配置加载成功(ttyS1000.xml)");
		if(m_logLevel == 3)
			tempAttach.Print(&syst);
	}
	
	if (0 != Inittty1000()){ /*初始化串口对象*/
		WriteLog(1, "FH1000-ETH串口服务初始化失败");
		return -1;
	}

	
	if (0 != InitcpSrvr()){	 /*初始化服务器对象*/
		WriteLog(1, "FH1000-ETH网络服务初始化失败");
		return -1;
	}
	WriteLog(1, "FH1000-ETH服务启动完成");
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

	//注册数据处理回调
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
			WriteLog(1,"初始化tcp服务器失败");
			delete m_kwsrver;
			m_kwsrver = NULL;
			return -1;
		}

		if (0 != m_kwsrver->start()){
			WriteLog(1,"tcp服务器启动失败");
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
	vsnprintf(buffer,LOGLENGTH,format,ap);//vsprintf的长度限制版
	va_end(ap);
    if(1){	//插入时间戳，会增加CPU开销
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

