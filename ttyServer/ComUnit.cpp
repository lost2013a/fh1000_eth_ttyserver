#include "ComUnit.h"
#include "kwserver.h"
#include <sys/prctl.h>

static int pcnt = 1;


ComUnit::ComUnit(Kw104Server * pServer, string &strIp)
{
	m_bExit = false;
	this->m_strIp = strIp;
	m_pServer = pServer;
	m_pSocket = new CusomSocket();
	m_logLevel =1;
}

ComUnit::~ComUnit(void)
{
	if (m_pSocket)
	{
		delete m_pSocket;
	}
}
int ComUnit::newinf(unsigned char *pASDU,  unsigned short len)
{
	m_dataDeque.PushBack(pASDU, len); 
	return 0;
}

int ComUnit::startallthread(unsigned int socket)
{
	m_pSocket->setSocket(socket); 	//把socket传入到
	bool ret = Comm_CreateThread(m_recivehandle, (ptrFun)recivethread, this);
	//WriteLog(1,"创建1个服务处理线程, 创建：%s, handle:%d",ret?"成功":"失败",m_recivehandle);
	return ret?0:-1;
}
void ComUnit::recivethread(void *lp)
{
	ComUnit *pUnit = (ComUnit *)lp;
	//线程n    每个客户端对应1个线程
	char pname[30] = "";
	sprintf(pname, "P_NAME[%d]",pcnt);
	pcnt++;
	prctl(PR_SET_NAME, (unsigned long)pname);
	pUnit->recivefun(); 

}

/*
 *	方案一：每个线程里转换,提高速度
 */
/*unsigned char data[MSG_BUFFER_LENGTH]="";
unsigned int data_leng;
int iRet = TranslateDLT1100((unsigned char*)ASDU,len,(unsigned char*)data,data_leng);
if (iRet == 0)
{
	if (m_pSocket->sendData(data,data_leng) != data_leng)
	{
		WriteLog(1,"发送出错，链路问题关闭连接");
		m_pSocket->closeSocket();
		return;
	}
}*/
/*
 *	方案二：上层统一转换本层只负责发送
 */

void ComUnit::recivedata()
{
	unsigned int count = 0;
	unsigned char ASDU[MSG_BUFFER_LENGTH];
	unsigned short len=0;
	int ret = 0 ;
	static int tcnt = 0;
	time_t tLiveTime=time(NULL);
	while(!m_bExit)
	{
		ret = m_dataDeque.PopFront((unsigned char*)ASDU, MSG_BUFFER_LENGTH, len);//从数据队列取1帧数据
		if(ret){
			if (m_pSocket->sendData(ASDU,len) != len){
				WriteLog(1,"服务处理线程发送出错，关闭连接");
				return;
			}
			time(&tLiveTime);
			//WriteLog(1,"发送OK");
			usleep(10*1000);
		}
		tcnt ++;
		if(tcnt>500){
			//接收用于检查链路是否开闭---rwp
			unsigned char buf[100]="";
			int iBytes = ReceData(buf, MSG_BUFFER_LENGTH, 20);
			//WriteLog(1,"检查线路:%d",iBytes);
			if (iBytes < 0){
				WriteLog(1,"服务处理线程连接检查失败，关闭连接");
				return;
			}
			tcnt = 0;
		}
		const int MAX_NODATA_TIMEOUT = 30 ;
		if(time(NULL)-tLiveTime>  MAX_NODATA_TIMEOUT){
			WriteLog(1,"服务处理线程超时(无发送数据%d秒)，关闭连接",MAX_NODATA_TIMEOUT);
			return;
		}
		usleep(100);
	}
}

void  ComUnit::recivefun()
{
	WriteLog(1,"通信服务队列注册1个新的连接");
	m_pServer->AddCom(this);	//在数据队列增加1个对象
	recivedata();			    //线程loop
	m_bExit = true;				//确保退出安全
	m_pSocket->closeSocket(); 
	sleep(3 *1000);    
	WriteLog(1,"通信服务队列注销1个连接");
	m_pServer->DeleteCom(this); 
	m_dataDeque.Clear(); //相应的，清除1份数据缓冲队列的数据
	delete this;		 //删除1个对象
}

int ComUnit::ReceData(unsigned char *buf, int len,int nTimeout)
{
	int nBytesSent = 0;
	int nBytesThisTime = 0;
	unsigned char *p = buf;
	do 
	{
		nBytesThisTime = m_pSocket->recvData (p, len - nBytesSent, nTimeout);
		if (nBytesThisTime <= 0)
		{
			return nBytesThisTime;
		} 
		nBytesSent += nBytesThisTime;
		p += nBytesThisTime;

	}while (nBytesSent < len);

	return nBytesSent;

}

void ComUnit::WriteLog(int iLevel,const char *format, ...)
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
    	char pname[128] = "";
		prctl(PR_GET_NAME, pname);
	
		struct timeval  tv;
	    struct timezone tz;
		gettimeofday(&tv,&tz);
		struct tm *tm = localtime(&tv.tv_sec);
		char time_prt[50]="";
		sprintf(time_prt,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d.%03ld ",
			tm->tm_year + 1900, tm->tm_mon + 1,tm->tm_mday, tm->tm_hour,
			tm->tm_min, tm->tm_sec,tv.tv_usec/1000);
		printf("%s-[ComUnit] %s %s\n",pname,time_prt,buffer);	
    }
	else{
		printf("[ComUnit] %s\n",buffer);	
	}
}

/*
int ComUnit::TranslateDLT1100(unsigned char* pASDU, unsigned int inleng, unsigned char* pDLT1100, unsigned int& outleng)
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
		//2530303832_30303138_3033_3032_3130_3539_3234_2A35303037 393232303034 333632390D 

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

		outleng = 36;
		ret = 0;

	}

	return ret;
}
*/

