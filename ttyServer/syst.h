#ifndef _SYST_H
#define _SYST_H

#include <vector>
#include "tinyxml2.h"

#define WIN32_LEAN_AND_MEAN
using namespace tinyxml2;
using namespace std;

typedef struct _tag_host_info
{
	int              hostId;              //������
	bool             enabeld;             //�Ƿ�ʹ��
	char             hostName[64];        //������
	char             hostIp[40];          //IP��ַ
	_tag_host_info()	{
		memset(this,0,sizeof(_tag_host_info));
	}
}HOST_INFO;

typedef struct _tag_tty_info
{
	char             ttyName[64];  //��������
	bool             enabeld;      //�Ƿ�ʹ��
	int              comId;        //���ں�
	int              baudrate;     //������
	int              parity;       //У��
	int              databits;     //����λ
	int              stopbits;     //ֹͣλ
	_tag_tty_info()	{
		memset(this,0,sizeof(_tag_tty_info));
	}
}TTY_INFO;

typedef struct _tag_system_info
{
	char                systename[100];  //ϵͳ������"FH1000"
	int                 circle_time;     //Ѳ��ʱ����
	int                 maxHostNum;      //����
	float               fReserve;        //����
	int                 logLevel;        //��־����
	int                 listport;        //����˿�
	int                 isendhost;       //�ײ㴮���Ƿ�ת������
	int                 isetTime;        //�Ƿ�����ʱ��
	TTY_INFO            ttyInfo;         //������Ϣ
	vector<HOST_INFO>   hostList;      //ϵͳ�������������

	_tag_system_info()
	{
		memset(systename,0,sizeof(systename));
		circle_time = 0;
		maxHostNum = 5;
		fReserve = 0.0;
		logLevel =0;
		listport = 2404;
		isendhost = 1;
		isetTime = 1;
		//memset(ttyInfo,0,sizeof(TTY_INFO));
	}

}SYSTEM_INFO;
#endif // _SYST_H