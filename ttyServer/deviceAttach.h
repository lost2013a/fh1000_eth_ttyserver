//****************************************************************************//
//* Copyright (c) 2007, 府河电气有限公司                                      
//* All rights reserved.                                                     
//* 文件 deviceAttach.h                                               
//* 日期 2015-8-19        
//* 作者 rwp                 
//* 注释 系统配置类                                 
//****************************************************************************//
#ifndef _deviceAttach_H
#define _deviceAttach_H

#include "syst.h"
#include "tinyxml2.h"
#define WIN32_LEAN_AND_MEAN
using namespace tinyxml2;
using namespace std;

// typedef struct _tag_host_info
// {
// 	int              hostId;              //主机号
// 	bool             enabeld;             //是否使用
// 	char             hostName[64];        //主机名
// 	char             hostIp[40];          //IP地址
// 	_tag_host_info()	{
// 		memset(this,0,sizeof(_tag_host_info));
// 	}
// }HOST_INFO;
// 
// typedef struct _tag_tty_info
// {
// 	char             ttyName[64];  //串口名称
// 	bool             enabeld;      //是否使用
// 	int              comId;        //串口号
// 	int              baudrate;     //波特率
// 	int              parity;       //校验
// 	int              databits;     //数据位
// 	int              stopbits;     //停止位
// 	_tag_tty_info()	{
// 		memset(this,0,sizeof(_tag_tty_info));
// 	}
// }TTY_INFO;
// 
// typedef struct _tag_system_info
// {
// 	char                systename[100];  //系统名称如"FH1000"
// 	int                 circle_time;     //巡检时间秒
// 	int                 maxHostNum;      //保留
// 	float               fReserve;        //保留
// 	int                 logLevel;        //日志级别
// 	int                 listport;        //服务端口
// 	TTY_INFO            ttyInfo;         //串口信息
// 	vector<HOST_INFO>   hostList;      //系统管理的主机配置
// 
// 	_tag_system_info()
// 	{
// 		memset(systename,0,sizeof(systename));
// 		circle_time = 0;
// 		maxHostNum = 5;
// 		fReserve = 0.0;
// 		logLevel =0;
// 		listport = 2405
// 		//memset(ttyInfo,0,sizeof(TTY_INFO));
// 	}
// 
// }SYSTEM_INFO;

class CttyS1000Attach
{
public:
	CttyS1000Attach(void);
	virtual ~CttyS1000Attach(void);
private:
	SYSTEM_INFO* model;
public:
	void Attach(SYSTEM_INFO* pModel);
public:
	int SaveModelToXML(const char* FileName );
	bool LoadModelFromXML( char* FileName );
	void Print(SYSTEM_INFO* syst);
private:
	bool LoadHostInfoNode(HOST_INFO *device,tinyxml2::XMLElement* element);
	bool LoadModel( tinyxml2::XMLElement* ModelInfo );
	tinyxml2::XMLElement* BuildSystemInfoModle(tinyxml2::XMLDocument* pDoc);
	tinyxml2::XMLElement* BuildHostList( tinyxml2::XMLDocument* pDoc );
	tinyxml2::XMLElement* BuildHostInfo(HOST_INFO *pNode, tinyxml2::XMLDocument* pDoc );
	tinyxml2::XMLElement* BuildTTYInfo(TTY_INFO *pNode, tinyxml2::XMLDocument* pDoc );

};
#endif // _deviceAttach_H