//****************************************************************************//
//* Copyright (c) 2007, 府河电气有限公司                                      
//* All rights reserved.                                                     
//* 文件 deviceAttach.cpp                                               
//* 日期 2015-8-19        
//* 作者 rwp                 
//* 注释 系统配置类                                 
//****************************************************************************//
#include "deviceAttach.h"

CttyS1000Attach::CttyS1000Attach(void)
{

}

CttyS1000Attach::~CttyS1000Attach(void)
{

}


void CttyS1000Attach::Attach( SYSTEM_INFO* pModel )
{
	model = pModel;
}

tinyxml2::XMLElement* CttyS1000Attach::BuildHostList( tinyxml2::XMLDocument* pDoc )
{
	tinyxml2::XMLElement* element = pDoc->NewElement("hostList");
	for (int i=0; i<model->hostList.size();i++)
	{
		HOST_INFO  *pNode = &model->hostList[i];
		if (!pNode)
			continue;

		tinyxml2::XMLElement* devNod = BuildHostInfo(pNode,pDoc);
		element->LinkEndChild(devNod);
	}
	return element;
}

tinyxml2::XMLElement* CttyS1000Attach::BuildHostInfo(HOST_INFO  *pNode, tinyxml2::XMLDocument* pDoc )
{
	tinyxml2::XMLElement* elemHostInfo = pDoc->NewElement("hostInfo");
	elemHostInfo->SetAttribute( "hostId"          , pNode->hostId            );   
	elemHostInfo->SetAttribute( "enabeld"         , pNode->enabeld          );   
	elemHostInfo->SetAttribute( "hostName"       , pNode->hostName            );   
	elemHostInfo->SetAttribute( "hostIp"       , pNode->hostIp                );   
	return elemHostInfo;
}

tinyxml2::XMLElement* CttyS1000Attach::BuildTTYInfo(TTY_INFO  *pNode, tinyxml2::XMLDocument* pDoc )
{
	tinyxml2::XMLElement* elemTTYInfo = pDoc->NewElement("ttyInfo");
	elemTTYInfo->SetAttribute( "ttyName"          , pNode->ttyName            );   
	elemTTYInfo->SetAttribute( "enabeld"          , pNode->enabeld          );   
	elemTTYInfo->SetAttribute( "comId"            , pNode->comId            );   
	elemTTYInfo->SetAttribute( "baudrate"         , pNode->baudrate                );   
	elemTTYInfo->SetAttribute( "parity"           , pNode->parity            );   
	elemTTYInfo->SetAttribute( "databits"         , pNode->databits          );   
	elemTTYInfo->SetAttribute( "stopbits"         , pNode->stopbits            );  
	return elemTTYInfo;
}

tinyxml2::XMLElement* CttyS1000Attach::BuildSystemInfoModle( tinyxml2::XMLDocument* pDoc )
{
	SYSTEM_INFO* mplc = model;
	tinyxml2::XMLElement* systXML = pDoc->NewElement("systemInfo");
	systXML->SetAttribute( "systename"          , mplc->systename            );      
	systXML->SetAttribute( "circle_time"        , mplc->circle_time          );     
	systXML->SetAttribute( "maxHostNum"        , mplc->maxHostNum            );     
	systXML->SetAttribute( "fReserve"        , mplc->fReserve                );     
	systXML->SetAttribute( "logLevel"        , mplc->logLevel                );     
	systXML->SetAttribute( "listport"        , mplc->listport                );
	systXML->SetAttribute( "isendhost"       , mplc->isendhost               );
	systXML->SetAttribute( "isetTime"        , mplc->isetTime                );
	tinyxml2::XMLElement* ttyInfoNode = BuildTTYInfo(&mplc->ttyInfo,pDoc);
	systXML->LinkEndChild(ttyInfoNode);

	tinyxml2::XMLElement* devListNode = BuildHostList(pDoc);
	systXML->LinkEndChild(devListNode);
	return systXML;
}

int CttyS1000Attach::SaveModelToXML( const char* xmlFileName )
{
	//新生成一个tinyxml2::XML文件，并按文档要求填写申明
	tinyxml2::XMLDocument* pDoc = new tinyxml2::XMLDocument;
	tinyxml2::XMLDeclaration* pDelare =NULL;
	pDelare = pDoc->NewDeclaration("xml version=\"1.0\" encoding=\"GBK\" ");
	if (pDelare==NULL)		{  
		return -1;
	}
	pDoc->LinkEndChild(pDelare);
	tinyxml2::XMLElement* root = BuildSystemInfoModle(pDoc);
    pDoc->LinkEndChild(root);

	return (tinyxml2::XML_SUCCESS == pDoc->SaveFile(xmlFileName))?0:-1;
}

bool CttyS1000Attach::LoadModelFromXML( char* xmlFileName )
{
	char strLog[512]="";
	tinyxml2::XMLDocument* pDoc = new tinyxml2::XMLDocument;
	tinyxml2::XMLError errNo = pDoc->LoadFile(xmlFileName);
	if (errNo != tinyxml2::XML_SUCCESS)
	{
		pDoc->PrintError();
		return 0;
	}

	tinyxml2::XMLElement* sysInfo = pDoc->FirstChildElement("systemInfo");
	if (sysInfo == NULL)	{
		return false;
	}

	return LoadModel(sysInfo);

}


bool CttyS1000Attach::LoadHostInfoNode(HOST_INFO *device,tinyxml2::XMLElement* element)
{
	if (!device)
		return false;

	const char* elemAttr=NULL;
	elemAttr = element->Attribute( "hostId"     );  if (elemAttr)  device->hostId = atoi(elemAttr);      
	elemAttr = element->Attribute( "enabeld"    );  if (elemAttr)  device->enabeld = atoi(elemAttr);               
	elemAttr = element->Attribute( "hostName"   );  if (elemAttr)  strncpy(device->hostName ,elemAttr,64);                
	elemAttr = element->Attribute( "hostIp"     );  if (elemAttr)  strncpy(device->hostIp,elemAttr,40);                       
	return true;
}


bool CttyS1000Attach::LoadModel( tinyxml2::XMLElement* element )
{
	int index(0);
	const char* elemAttr=NULL;

	elemAttr = element->Attribute( "systename"         ); if (elemAttr) strncpy( model->systename,elemAttr,100);   //系统名称    
	elemAttr = element->Attribute( "circle_time"         ); if (elemAttr)  model->circle_time = atoi(elemAttr);   //             
	elemAttr = element->Attribute( "maxHostNum"         ); if (elemAttr)  model->maxHostNum = atoi(elemAttr);   //               
	elemAttr = element->Attribute( "fReserve"         ); if (elemAttr)  model->fReserve = atof(elemAttr);   //                   
	elemAttr = element->Attribute( "logLevel"         ); if (elemAttr)  model->logLevel = atoi(elemAttr);   //                   
	elemAttr = element->Attribute( "listport"         ); if (elemAttr)  model->listport = atoi(elemAttr);   //  	
	elemAttr = element->Attribute( "isendhost"         ); if (elemAttr)  model->isendhost = atoi(elemAttr);   //  
	elemAttr = element->Attribute( "isetTime"         ); if (elemAttr)  model->isetTime = atoi(elemAttr);   //  
	tinyxml2::XMLElement* ttyInfo = element->FirstChildElement("ttyInfo");
	if (ttyInfo == NULL)	{
		return false;
	}
	elemAttr = ttyInfo->Attribute( "ttyName"   );  if (elemAttr) strncpy( model->ttyInfo.ttyName,elemAttr,64);   //系统名称     
	elemAttr = ttyInfo->Attribute( "enabeld"   );  if (elemAttr)  model->ttyInfo.enabeld = atoi(elemAttr);   //                
	elemAttr = ttyInfo->Attribute( "comId"     );  if (elemAttr)  model->ttyInfo.comId = atoi(elemAttr);   //                 
	elemAttr = ttyInfo->Attribute( "baudrate"  );  if (elemAttr)  model->ttyInfo.baudrate = atof(elemAttr);   //                   
	elemAttr = ttyInfo->Attribute( "parity"    );  if (elemAttr)  model->ttyInfo.parity = atoi(elemAttr);   //                   
	elemAttr = ttyInfo->Attribute( "databits"  );  if (elemAttr)  model->ttyInfo.databits = atoi(elemAttr);   //                                                                         
	elemAttr = ttyInfo->Attribute( "stopbits"  );  if (elemAttr)  model->ttyInfo.stopbits = atoi(elemAttr);   //                                                                         

	index =0;
	tinyxml2::XMLElement* hostList = element->FirstChildElement("hostList");
	if (hostList == NULL)	{
		return false;
	}
	for (tinyxml2::XMLNode* node = hostList->FirstChild(); node; node = node->NextSibling())
	{
		tinyxml2::XMLElement* elemHostInfo=NULL;
		if (elemHostInfo = node->ToElement())	{
			HOST_INFO deviceNod;
			LoadHostInfoNode(&deviceNod,elemHostInfo);
			model->hostList.push_back(deviceNod);
		}
	}
	return true;
}

void CttyS1000Attach::Print(SYSTEM_INFO* syst)
{
	char strLog[254]="";                                                                                                                 
	sprintf( strLog, "syst.systename  : %s ", syst->systename   );     printf("%s\n",strLog); //系统名称如"FH1000"                          
	sprintf( strLog, "syst.circle_time: %d ", syst->circle_time );     printf("%s\n",strLog);  //巡检时间秒                                 
	sprintf( strLog, "syst.maxHostNum : %d ", syst->maxHostNum  );     printf("%s\n",strLog);  //保留                                       
	sprintf( strLog, "syst.fReserve   : %f ", syst->fReserve    );     printf("%s\n",strLog);  //保留                                       
	sprintf( strLog, "syst.logLevel   : %d ", syst->logLevel    );     printf("%s\n",strLog);  //日志级别   
	sprintf( strLog, "syst.listport   : %d ", syst->listport    );     printf("%s\n",strLog);  //日志级别
	sprintf( strLog, "syst.isendhost  : %d ", syst->isendhost    );    printf("%s\n",strLog);  //日志级别
	sprintf( strLog, "syst.isetTime   : %d ", syst->isetTime    );     printf("%s\n",strLog);  //日志级别
	sprintf( strLog, "syst.hostList   : %d ", syst->hostList.size() ); printf("%s\n",strLog);    //系统管理的主机配置                       
	sprintf( strLog, "syst.ttyInfo.ttyName      : %s ", syst->ttyInfo.ttyName      );     printf("%s\n",strLog);  //串口信息                
	sprintf( strLog, "syst.ttyInfo.enabeld      : %d ", syst->ttyInfo.enabeld      );     printf("%s\n",strLog);  //串口信息                
	sprintf( strLog, "syst.ttyInfo.comId        : %d ", syst->ttyInfo.comId        );     printf("%s\n",strLog);  //串口信息                
	sprintf( strLog, "syst.ttyInfo.baudrate     : %d ", syst->ttyInfo.baudrate     );     printf("%s\n",strLog);  //串口信息                
	sprintf( strLog, "syst.ttyInfo.parity       : %d ", syst->ttyInfo.parity       );     printf("%s\n",strLog);  //串口信息                
	sprintf( strLog, "syst.ttyInfo.databits     : %d ", syst->ttyInfo.databits     );     printf("%s\n",strLog);  //串口信息                
	sprintf( strLog, "syst.ttyInfo.stopbits     : %d ", syst->ttyInfo.stopbits     );     printf("%s\n",strLog);  //串口信息                
}