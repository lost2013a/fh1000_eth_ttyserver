#ifndef _SYST_H
#define _SYST_H

#include <vector>
#include "tinyxml2.h"

#define WIN32_LEAN_AND_MEAN
using namespace tinyxml2;
using namespace std;

typedef struct _tag_host_info
{
	int              hostId;              //主机号
	bool             enabeld;             //是否使用
	char             hostName[64];        //主机名
	char             hostIp[40];          //IP地址
	_tag_host_info()	{
		memset(this,0,sizeof(_tag_host_info));
	}
}HOST_INFO;

typedef struct _tag_tty_info
{
	char             ttyName[64];  //串口名称
	bool             enabeld;      //是否使用
	int              comId;        //串口号
	int              baudrate;     //波特率
	int              parity;       //校验
	int              databits;     //数据位
	int              stopbits;     //停止位
	_tag_tty_info()	{
		memset(this,0,sizeof(_tag_tty_info));
	}
}TTY_INFO;

typedef struct _tag_system_info
{
	char                systename[100];  //系统名称如"FH1000"
	int                 circle_time;     //巡检时间秒
	int                 maxHostNum;      //保留
	float               fReserve;        //保留
	int                 logLevel;        //日志级别
	int                 listport;        //服务端口
	int                 isendhost;       //底层串口是否转发主机
	int                 isetTime;        //是否设置时间
	TTY_INFO            ttyInfo;         //串口信息
	vector<HOST_INFO>   hostList;      //系统管理的主机配置

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