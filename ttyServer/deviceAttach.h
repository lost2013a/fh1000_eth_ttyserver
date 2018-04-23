//****************************************************************************//
//* Copyright (c) 2007, ���ӵ������޹�˾                                      
//* All rights reserved.                                                     
//* �ļ� deviceAttach.h                                               
//* ���� 2015-8-19        
//* ���� rwp                 
//* ע�� ϵͳ������                                 
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
// 	int              hostId;              //������
// 	bool             enabeld;             //�Ƿ�ʹ��
// 	char             hostName[64];        //������
// 	char             hostIp[40];          //IP��ַ
// 	_tag_host_info()	{
// 		memset(this,0,sizeof(_tag_host_info));
// 	}
// }HOST_INFO;
// 
// typedef struct _tag_tty_info
// {
// 	char             ttyName[64];  //��������
// 	bool             enabeld;      //�Ƿ�ʹ��
// 	int              comId;        //���ں�
// 	int              baudrate;     //������
// 	int              parity;       //У��
// 	int              databits;     //����λ
// 	int              stopbits;     //ֹͣλ
// 	_tag_tty_info()	{
// 		memset(this,0,sizeof(_tag_tty_info));
// 	}
// }TTY_INFO;
// 
// typedef struct _tag_system_info
// {
// 	char                systename[100];  //ϵͳ������"FH1000"
// 	int                 circle_time;     //Ѳ��ʱ����
// 	int                 maxHostNum;      //����
// 	float               fReserve;        //����
// 	int                 logLevel;        //��־����
// 	int                 listport;        //����˿�
// 	TTY_INFO            ttyInfo;         //������Ϣ
// 	vector<HOST_INFO>   hostList;      //ϵͳ�������������
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