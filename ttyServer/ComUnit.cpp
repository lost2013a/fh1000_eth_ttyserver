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
	m_pSocket->setSocket(socket); 	//��socket���뵽
	bool ret = Comm_CreateThread(m_recivehandle, (ptrFun)recivethread, this);
	//WriteLog(1,"����1���������߳�, ������%s, handle:%d",ret?"�ɹ�":"ʧ��",m_recivehandle);
	return ret?0:-1;
}
void ComUnit::recivethread(void *lp)
{
	ComUnit *pUnit = (ComUnit *)lp;
	//�߳�n    ÿ���ͻ��˶�Ӧ1���߳�
	char pname[30] = "";
	sprintf(pname, "P_NAME[%d]",pcnt);
	pcnt++;
	prctl(PR_SET_NAME, (unsigned long)pname);
	pUnit->recivefun(); 

}

/*
 *	����һ��ÿ���߳���ת��,����ٶ�
 */
/*unsigned char data[MSG_BUFFER_LENGTH]="";
unsigned int data_leng;
int iRet = TranslateDLT1100((unsigned char*)ASDU,len,(unsigned char*)data,data_leng);
if (iRet == 0)
{
	if (m_pSocket->sendData(data,data_leng) != data_leng)
	{
		WriteLog(1,"���ͳ�����·����ر�����");
		m_pSocket->closeSocket();
		return;
	}
}*/
/*
 *	���������ϲ�ͳһת������ֻ������
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
		ret = m_dataDeque.PopFront((unsigned char*)ASDU, MSG_BUFFER_LENGTH, len);//�����ݶ���ȡ1֡����
		if(ret){
			if (m_pSocket->sendData(ASDU,len) != len){
				WriteLog(1,"�������̷߳��ͳ����ر�����");
				return;
			}
			time(&tLiveTime);
			//WriteLog(1,"����OK");
			usleep(10*1000);
		}
		tcnt ++;
		if(tcnt>500){
			//�������ڼ����·�Ƿ񿪱�---rwp
			unsigned char buf[100]="";
			int iBytes = ReceData(buf, MSG_BUFFER_LENGTH, 20);
			//WriteLog(1,"�����·:%d",iBytes);
			if (iBytes < 0){
				WriteLog(1,"�������߳����Ӽ��ʧ�ܣ��ر�����");
				return;
			}
			tcnt = 0;
		}
		const int MAX_NODATA_TIMEOUT = 30 ;
		if(time(NULL)-tLiveTime>  MAX_NODATA_TIMEOUT){
			WriteLog(1,"�������̳߳�ʱ(�޷�������%d��)���ر�����",MAX_NODATA_TIMEOUT);
			return;
		}
		usleep(100);
	}
}

void  ComUnit::recivefun()
{
	WriteLog(1,"ͨ�ŷ������ע��1���µ�����");
	m_pServer->AddCom(this);	//�����ݶ�������1������
	recivedata();			    //�߳�loop
	m_bExit = true;				//ȷ���˳���ȫ
	m_pSocket->closeSocket(); 
	sleep(3 *1000);    
	WriteLog(1,"ͨ�ŷ������ע��1������");
	m_pServer->DeleteCom(this); 
	m_dataDeque.Clear(); //��Ӧ�ģ����1�����ݻ�����е�����
	delete this;		 //ɾ��1������
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
	vsnprintf(buffer,LOGLENGTH,format,ap);//vsprintf�ĳ������ư�
	va_end(ap);

	
		
		
	
	
    if(1){	//����ʱ�����������CPU����
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
		//ͬ��ͷ'TS'
		p += 2; 

		//��ȡʱ�䴮
		char ymd_str[15]="";
		memcpy(ymd_str,p,14);
		ymd_str[0] = '0'; //��Ϊ��ǧΪ0
		ymd_str[1] = '0'; //��Ϊ���Ϊ0
		p += 14;

		//�ָ���
		p += 1;

		//��ȡƵ��
		char freq_intstr[8]="";
		memcpy(freq_intstr,p,2);
		p += 2;

		//����С����
		p +=1; 

		//��ȡƵ��С��
		char freq_fstr[8]="";
		memcpy(freq_fstr,p,4);
		p += 4;

		//�ָ���
		p += 1;

		//��ȡ����6�ַ�
		char clock_str[7]="";
		memcpy(clock_str,p,6);
		p += 6;
		//2530303832_30303138_3033_3032_3130_3539_3234_2A35303037 393232303034 333632390D 

		unsigned char kwAPDU[100];
		unsigned char* ptr = pDLT1100;


		//֡ͷ'%'
		*ptr++ = 0x25;

		//״̬��־1
		*ptr++ = 0x30;

		//״̬��־2
		*ptr++ = 0x30;

		//״̬��־3��ʱ������
		*ptr++ = 0x38;

		//״̬��־4��ʱ������
		*ptr++ = 0x30;

		//��Ϊʱ�䴮
		memcpy(ptr,ymd_str,14);
		ptr += 14;

		//��Ϊ�ָ���
		*ptr++  = 0x2a;

		//Ƶ������
		memcpy(ptr,freq_intstr,2);
		ptr += 2;

		//Ƶ��С��
		memcpy(ptr,freq_fstr,4);
		ptr += 4;

		//���Ӳ���
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
		//WriteLog(3,"���ֵ:%s",Chr);

		//ʱ��У��ֵ
		*ptr++ = Chr[0];
		*ptr++ = Chr[1];

		//������
		*ptr++ = 0x0d;
		*ptr++ = 0x0a;

		outleng = 36;
		ret = 0;

	}

	return ret;
}
*/

