////****************************************************************************//
//* Copyright (c) 2007, 府河电气有限公司                                      
//* All rights reserved.                                                     
//* 文件 ttyS1000.h                                               
//* 日期 2015-5-18        
//* 作者 rwp                 
//* 注释                                  
//****************************************************************************//
#include "kwserver.h"
#include <sys/prctl.h>

#define MAX_TRY_TIME 10
Kw104Server::Kw104Server(SYSTEM_INFO *pCom)
	:m_ServerPort(2404),m_logLevel(1),m_syst(pCom)
{ 
	bAccept = false;
	m_bExit = false;
	m_dbconnectIndex = 1;
	m_equipid = 1;
}

Kw104Server::~Kw104Server()
{

	SocketDllRelease();
}

int Kw104Server::StartListenServer()
{
	bool ret = Comm_CreateThread(m_listenhandle, (ptrFun)comServerThread, this);
	if (ret == false){
		WriteLog(1,"通信侦听服务线程创建失败");
	}
	//WriteLog(1,"通信侦听服务线程创建成功");
	return 0;
}

void Kw104Server::CloseServer()
{
	m_bExit = true;
	m_pListen->closeSocket();
	Comm_WaitObjectfinshed(m_listenhandle);

}

void  Kw104Server::comServerThread(void * lp)
{
	//线程2 监听连接
	prctl(PR_SET_NAME, (unsigned long)"TSERVER_1_Listen");
	Kw104Server * pServer = (Kw104Server*)lp;


	pServer->comServerFun();

}

void Kw104Server::comServerFun()
{
	int tryTime = 0;
	while (!m_bExit){
		string strConnectAddr;
		WriteLog(3,"通信侦听服务端口:%03d)",m_ServerPort);
		bool b = m_pListen->CreateListenSocket(m_ServerPort); 
		while(!b){
			tryTime++;
			if (tryTime>=MAX_TRY_TIME){
				WriteLog(1,"通信侦听启动尝试次数超出,端口:%03d",m_ServerPort);
				exit(1);
			}
			else{
				sleep(1000);//延时下
				continue;
			}
		}
		WriteLog(1,"通信侦听线程运行成功");
		while (!m_bExit){
			unsigned int hclient = m_pListen->AcceptSocket(strConnectAddr); 
			if (hclient ==  -1){ 
				WriteLog(1,"AcceptSocket失败");
				break;
			}
			WriteLog(1,"通信侦听线程检查到新连接(ip:%s)",strConnectAddr.c_str());
			if (!LoginVerify()){	//检查连接数超限
				WriteLog(1,"超过最多支持接入主机数目(maxHostNum:%d)",m_syst->maxHostNum);
#if defined(Q_OS_WIN) 
				closesocket(hclient);
#elif defined(Q_OS_UNIX) 
				::close(hclient);
#endif
				continue;
			}

			ComUnit *pUnit = new ComUnit(this, strConnectAddr);
			int ret = pUnit->startallthread(hclient);//创建1个线程处理新的连接
			if (ret == -1){
				delete pUnit;
			}
			usleep(10*1000);
			WriteLog(1,"创建1个服务处理线程,%0d/%0d",m_ComList.size(),m_syst->maxHostNum);
		}
		WriteLog(1,"通信侦听服务线程退出");
		m_pListen->closeSocket();
	}

}
int  Kw104Server::newinf(unsigned char *pASDU,  unsigned short len)
{
	//串口程序收到数据写入到这儿，同时如果存在变化的数据将先各个连接发送
	m_comMutex.lock();
	if (m_ComList.size()>=0)
	{
		/*
		 *	方案一：统一在此处转，避免转多次---rwp
		 */
		//TS20050221021538F50.0043C021535+003.78*1C
		unsigned char dlt1100_msg[38]="";
		unsigned int lengh(36);
		int ret = TranslateDLT1100(pASDU,len,dlt1100_msg,lengh);
		if (ret == 0)
		{
			vector<ComUnit * >::iterator it = m_ComList.begin ();
			for (; it != m_ComList.end(); it++)
			{
				(*it)->newinf(dlt1100_msg, lengh); //把转换好的数据放入数据队列
			}
		}else
		{
			WriteLog(1,"串口报文转换失败");
		}



		//方案二：在对象里自已负责转换
		/*vector<ComUnit * >::iterator it = m_ComList.begin ();
		for (; it != m_ComList.end(); it++)
		{
			(*it)->newinf(pASDU, len); 
		}*/
	}

	m_comMutex.unlock();
	return 0;
}


int Kw104Server::readcfg()
{
	return 0;

}
void Kw104Server::AddCom(ComUnit * p)
{
	m_comMutex.lock ();
	m_ComList.push_back(p); //在队尾插入
	m_comMutex.unlock();

}
void Kw104Server::DeleteCom(ComUnit * p)
{
	m_comMutex.lock();
	vector<ComUnit * >::iterator it = m_ComList.begin ();
	for (; it != m_ComList.end(); it++)
	{
		if ((*it) == p)
		{
			m_ComList.erase(it); 
			break;
		}
	}
	m_comMutex.unlock();

}
int Kw104Server::addreport(unsigned char *pASDU,  unsigned short len)
{
	m_comMutex.lock();
	vector<ComUnit * >::iterator it = m_ComList.begin ();
	for (; it != m_ComList.end(); it++)
	{
		(*it)->newinf(pASDU, len); 

	}
	m_comMutex.unlock();
	return 0;
}

int Kw104Server::start()
{
	return StartListenServer();
}

int Kw104Server::Init(SYSTEM_INFO* syst)
{
	m_ServerPort = syst->listport;
	m_logLevel 	 = syst->logLevel;

	bool bsuccess = SocketDLLInit();
	if (bsuccess == false)
	{
		WriteLog(1,"初始化通信动态库失败");	
	}
	m_pListen = new  CusomListenSocket();
	if (m_pListen==NULL)
	{
		WriteLog(1,"实例化通信服务器失败");
		return -1;
	}
	return 0;
}


void Kw104Server::WriteLog(int iLevel,const char *format, ...)
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
		printf("[Kw104Server] %s %s\n",time_prt,buffer);	
    }
	else{
		printf("[Kw104Server] %s\n",buffer);	
	}
}
int Kw104Server::TranslateDLT1100(unsigned char* pASDU, unsigned int inleng, unsigned char* pDLT1100, unsigned int& outleng)
{
	int ret(-1);
	if (((pASDU)&&(inleng >= 43))&&
	       ((pDLT1100)&&(outleng>=36)))
	{
		unsigned char* p = pASDU;
		//同步头'TS'
		p += 2; 

		//提取时间串
		char ymd_str[15]="";
		memcpy(ymd_str,p,14);
		ymd_str[0] = '0'; //可为年千为0
		ymd_str[1] = '0'; //可为年百为0
		p += 14;

		//分隔符
		p += 1;

		//提取频率
		char freq_intstr[8]="";
		memcpy(freq_intstr,p,2);
		p += 2;

		//跳过小数点
		p +=1; 

		//提取频率小数
		char freq_fstr[8]="";
		memcpy(freq_fstr,p,4);
		p += 4;

		//分隔符
		p += 1;

		//提取电钟6字符
		char clock_str[7]="";
		memcpy(clock_str,p,6);
		p += 6;
		/*2530303832_30303138_3033_3032_3130_3539_3234_2A35303037 393232303034 333632390D */

		unsigned char kwAPDU[100];
		unsigned char* ptr = pDLT1100;


		//帧头'%'
		*ptr++ = 0x25;

		//状态标志1
		*ptr++ = 0x30;
		
		//状态标志2
		*ptr++ = 0x30;

		//状态标志3，时区编移
		*ptr++ = 0x38;

		//状态标志4，时区编移
		*ptr++ = 0x30;

		//可为时间串
		memcpy(ptr,ymd_str,14);
		ptr += 14;

		//可为分隔符
		*ptr++  = 0x2a;

		//频率整数
		memcpy(ptr,freq_intstr,2);
		ptr += 2;

		//频率小数
		memcpy(ptr,freq_fstr,4);
		ptr += 4;

		//电钟部分
		memcpy(ptr,clock_str,6);
		ptr += 6;

		unsigned char* it = pDLT1100;
		unsigned char xorSum = *(++it);
		for (int i=1; i<=31; i++)
		{
			++it;
			xorSum ^= (*it);
		}

		char Chr[3]="";
		sprintf(Chr,"%02x",xorSum);
		//WriteLog(3,"异或值:%s",Chr);

		//时钟校验值
		*ptr++ = Chr[0];
		*ptr++ = Chr[1];

		//结束符
		*ptr++ = 0x0d;
		*ptr++ = 0x0a;

		ret  = 0;
		outleng = 36;
	}

    return ret;
}


bool  Kw104Server::LoginVerify()	
{
	bool ret(true);
	int connectdNum;
	
	m_comMutex.lock();
	
	connectdNum = m_ComList.size();
	if ((connectdNum+1) > m_syst->maxHostNum){
		ret = false;
	}
	
	m_comMutex.unlock();
	
	return ret;
}
