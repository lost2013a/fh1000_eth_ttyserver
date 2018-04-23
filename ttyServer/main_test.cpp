////****************************************************************************//
//* Copyright (c) 2007, 府河电气有限公司                                      
//* All rights reserved.                                                     
//* 文件 ttyS1000.h                                               
//* 日期 2015-5-18        
//* 作者 rwp                 
//* 注释                                  
//****************************************************************************//
#include "stdio.h"
#include "stdlib.h"
#include "CComPort.h"
#include "kwserver.h"

static void WriteLog(int iLevel,const char *format, ...);

int main(int argc, char *argv[])
{
	printf("The progarm will open ttyS0(===>comm1) and recv fh1000 gps message. please check /dev/ttySxx device. \nexample: ttyTest 1\n");
	unsigned short comid = 2;
	
	CComPort* m_rCommPort = new CComPort();
	m_rCommPort->do_config(comid,9600,8,1,'n',1000,1000);
	
	if (0 != m_rCommPort->open()){
		printf("打开串口(%d)错误！\n",comid);
	}
	const unsigned int  USART_REC_LEN =100;
	char buf[USART_REC_LEN]="";
	int i=0,read_lengh;
	unsigned short USART_RX_STA = 0;
	char pdata;
	//printf("read a :%c [%x]\n",pdata,USART_RX_STA);

	m_rCommPort->purge_comm(1,1);
	
	while(1)
	{
		while((USART_RX_STA&0x8000)==0){
			read_lengh = m_rCommPort->read_bytes(&pdata,1);//有超时机制
			
			if (read_lengh == 1){
				if(USART_RX_STA&0x4000)//接收到了0x0d
				{
					if(pdata!=0x0a)
						USART_RX_STA=0;//接收错误,重新开始
					else 
						USART_RX_STA|=0x8000;	//接收完成了 
				}
				else //还没收到0X0D
				{	
					if(pdata==0x0d)USART_RX_STA|=0x4000;
					else
					{
						buf[USART_RX_STA&0X3FFF]=pdata ;
						USART_RX_STA++;
						if(USART_RX_STA>(USART_REC_LEN-1)){
							WriteLog(1,"USART_REC_LEN");
							USART_RX_STA=0;
						}
					}		 
				}   		 
			}
		}
		WriteLog(1,"REC[%02d]:%s",USART_RX_STA&0X3FFF,buf);
		USART_RX_STA = 0;
		
	}

	m_rCommPort->close();
	delete m_rCommPort;
	return 0;
}


static void WriteLog(int iLevel,const char *format, ...)
{
	
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
		printf("%s %s\n",time_prt,buffer);	
    }
	else{
		printf("%s\n",buffer);	
	}
}



