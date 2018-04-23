////****************************************************************************//
//* Copyright (c) 2007, 府河电气有限公司                                      
//* All rights reserved.                                                     
//* 文件 ttyS1000.cpp                                               
//* 日期 2015-5-18        
//* 作者 rwp                 
//* 注释                                  
//****************************************************************************//
#include "ttyS1000.h"
#include <sys/prctl.h>


void ltrim(char *str)  
{  
	int i=0,j,len=strlen(str);  
	while(str[i]!='/0')  
	{  
		if(str[i]!=32&&str[i]!=9)break;/*32:空格,9:横向制表符*/  
		i++;  
	}  
	if(i!=0)  
		for(j=0;j<=len-i;j++)  
		{     
			str[j]=str[j+i];/*将后面的字符顺势前移,补充删掉的空白位置*/  
		}  
}  

CttyS1000::CttyS1000(SYSTEM_INFO *pShell)
	:m_callbackASDUHandler(NULL),m_callbackObj(NULL),m_rCommPort(NULL),setlocalTimeFlag(false)
{
	m_pShell = pShell;
	m_logLevel = m_pShell->logLevel;
	m_valflag = -1;
	initVars();
}

CttyS1000::~CttyS1000()
{

	if(m_rCommPort)
	{
		m_rCommPort->close();
		delete m_rCommPort;
		m_rCommPort = NULL;
	}

	closeAllThread();
}


void CttyS1000::initVars()
{

	m_rCommPort = NULL;
	m_bExit = false;

}



bool CttyS1000::start()
{

	bool  ret = create_thread(m_circleTheaddHandle,(ptrFun)circleThread,this);
	if (ret == false){
		WriteLog(1,"串口处理线程创建失败");
	}
	return ret;

}


bool CttyS1000::create_thread(_HANDLE &handle ,ptrFun fun_name,void *fun_param)
{

	bool ret = true ;

#if defined(OS_LINUX) 

	pthread_attr_t		threadAttrib;		//线程属性

	//初始化线程属性
	pthread_attr_init( &threadAttrib);

	//设置线程绑定属性( 绑定:PTHREAD_SCOPE_SYSTEM, 非绑定:PTHREAD_SCOPE_PROCESS)
	pthread_attr_setscope( &threadAttrib, PTHREAD_SCOPE_SYSTEM);

	//设置线程分离属性( 分离:PTHREAD_CREATE_DETACHED, 非分离:PTHREAD_CREATE_JOINABLE)
	pthread_attr_setdetachstate( &threadAttrib, PTHREAD_CREATE_DETACHED);

	int value  = pthread_create(&handle, &threadAttrib, fun_name, (void*)fun_param);
	if (value != 0)	
		ret = false;

#elif defined(Q_OS_WIN)

	handle = (_HANDLE)_beginthread (fun_name, 0, fun_param);
	if (handle == NULL)
		ret = false;
#endif

	return ret;

}

bool CttyS1000::thread_exit(_HANDLE &handle)
{

#if defined(Q_OS_WIN) 

	_endthread();


#elif defined(OS_LINUX)

	pthread_exit(NULL);

#endif

	return true;

}

unsigned long CttyS1000::comm_gettickcount()
{

#if defined(OS_LINUX) 

	int				fd;
	unsigned long	sec,usec;
	char			buf[64];

	fd = open("/proc/uptime",O_RDONLY);

	if (fd == -1)
	{
		perror("open");
		return 0;
	}

	memset(buf,0,64);
	read(fd,buf,64);
	sscanf(buf,"%ld.%ld",&sec,&usec);

	close( fd);

	return (sec * 1000 + usec);


#elif defined(Q_OS_WIN)

	return GetTickCount();

#endif

	return 0;
}


void CttyS1000::closeAllThread()
{
	int   interval   = 25*1000 ;  //2s

	unsigned long	 beginTick	= comm_gettickcount();
	unsigned long   curTick ;


	m_bExit = true;
	while(  !m_circleTheadExt  )
	{	
		curTick = comm_gettickcount();
		unsigned long  offset = curTick - beginTick;

		if (offset > interval) 
		{	
			break;
		}
		sleep(50);
	}

}


void  CttyS1000::circleThread(void *pVoid)
{

	//线程3 每秒串口数据(输入)的处理
	prctl(PR_SET_NAME, (unsigned long)"TSERVER_2_Serial");
	CttyS1000 *pInf = (CttyS1000 *)pVoid;

	pInf->circleFun();
}


void   CttyS1000::circleFun()
{

	WriteLog(1,"串口处理线程运行成功");;
	while(!m_bExit)
	{	
		
		processfun();

		/*
		 *	注意加延时后会影响串口数据接收，可能是因为板卡性能太低---rwp 20180319
		 */
		usleep(1000);
	}
	m_bExit = true;
	m_circleTheadExt = true;
	thread_exit(m_circleTheaddHandle);
	WriteLog(1,"串口处理线程退出");
}


void  CttyS1000::processfun()
{

	const unsigned int  MAX_FAULT_NUM = 10000;
	static int g_errNum=0;
	int ret = recvGPSMsg();
	if (ret != 0)	{
		g_errNum++;
		WriteLog(1,"**********接收GPS报文失败,失败次数:(%d)**********",g_errNum);
		if (g_errNum>MAX_FAULT_NUM)		{
			WriteLog(1,"超过最大错误次数，退出");
			m_bExit = true;
			//退出程序
			exit(1);
		}
	}else
		g_errNum = 0;
}

/*int CttyS1000::recvMsg()
{
	const unsigned int MSG_LENGH = 43 ;
	char buf[100]="";
	char buf_bak[100]="";
	bool hdr_find = false;
	int recv_len = m_rCommPort->read_bytes(buf,  MSG_LENGH);
	if ((recv_len >= 2)&&(recv_len<= MSG_LENGH))	{
		WriteLog(2,"[&&&&&&%s&&&&&&]\n",buf);
		int left_bytes = 0;
		for (int i=0; i<recv_len; i++)
		{
			if ((buf[i] != MSG_CHAR_FRM))
			{
				WriteLog(2,"###check msg_head's frame flag faild: %c(0x%02x)###",buf[i],buf[i]);
				continue;
			}else
			{
				WriteLog(3,"check msg_head's frame ok");

				//预判范围
				if ((i+1)<recv_len)
				{
					if (((buf[i+1] == MSG_CHAR_SYN)||(buf[i+1] == MSG_CHAR_ASN)))
					{
						WriteLog(3,"check msg_head's sync ok");
						hdr_find = true;
						left_bytes = (MSG_LENGH - (recv_len-i));
						memcpy(buf_bak,(char*)&(buf[i]),recv_len-i);
						WriteLog(1,"寻找到头(leftbytes:%d),(%s)",left_bytes,buf_bak);
						break;
					}else
					{
						WriteLog(2,"###check msg_head's synch flag faild: %c(0x%02x)###",buf[i+1],buf[i+1]);
						continue;
					}
				}else
				{
					//最后一个字符为'T'
					if ((i+1)==recv_len)
					{
						//再读领近一个字符判断
						recv_len = m_rCommPort->read_bytes(buf, 1);
						if ((recv_len == 1)&&
								((buf[i+1] == MSG_CHAR_SYN)||(buf[i+1] == MSG_CHAR_ASN)))
						{
							WriteLog(3,"msg_head's T is trail!");
							left_bytes = MSG_LENGH -2;
							buf_bak[0]= MSG_CHAR_FRM;
							buf_bak[1]= buf[i+1];
							hdr_find = true;
							WriteLog(1,"寻找到头2(leftbytes:%d),(%s)",left_bytes,buf_bak);
							break;
						}
					}
				}

			}
		}

		if ((hdr_find))
		{
			//最后一次读
			if (left_bytes > 0)
			{
				recv_len = m_rCommPort->read_bytes(buf,  left_bytes);
				if (recv_len == left_bytes)
				{
					int read_bytes = MSG_LENGH - left_bytes;
					memcpy(&buf_bak[read_bytes],buf,left_bytes);
					if (m_pShell->isendhost)						{
						WriteLog(1,"处理字符串(%s)",buf_bak);
						ttysDataHandler((unsigned char*)buf_bak,MSG_LENGH);
						return 0;
					}
				}
			}

			//只读一次即可
			if (left_bytes == 0)
			{
				if (m_pShell->isendhost)						{
					WriteLog(1,"处理字符串2(%s)",buf_bak);
					ttysDataHandler((unsigned char*)buf_bak,MSG_LENGH);
					return 0;
				}
			}

		}
	}else
	{
		WriteLog(2,"未正确读报文\n");
	}
	return -1;
}
*/

//TS20050221021538F50.0043C021535+003.78*1C
	//例：  TS20180307100348F49.9800C100347+001.94*16
	//<T><B>YYYYMMDDhhmmss<F>FF.F1F2F3F4<C>HHM1M2SSX CCC.CC	*HH<CR><LF>

int CttyS1000::recvGPSMsg()
{
 	unsigned int MSG_MAX_LENGH = 43;
	int ret(-1);
	int times(0);
	do 
	{
		unsigned char buf[255]="";
		char* p = (char*)buf;
		int recv_len = m_rCommPort->read_bytes(p,  1);
		bool hdr_find = false;
		if(recv_len != 1){
			times++;
			WriteLog(2,"recv msg_head's  frame flag faild,times:%d,len:%d",times,recv_len);
		}
		else{
			if ((buf[0] != MSG_CHAR_FRM)){
				times++;
				WriteLog(2,"###check msg_head's frame flag faild: %c(0x%02x),times:%d###",*p,*p,times);
			}
			else{
				WriteLog(3,"check msg_head's frame ok");
				p++;
				recv_len = m_rCommPort->read_bytes(p,  1);
				if (recv_len ==1)
				{
					if (((buf[1] == MSG_CHAR_SYN)||(buf[1] == MSG_CHAR_ASN)))
					{
						WriteLog(3,"check msg_head's sync ok");
						p++;
						hdr_find = true;
					}
					else{
						times++;
					    WriteLog(2,"****check msg_head's syn char faild: %c(0x%02x),recv_len:%d, times:%d****",*p,*p,recv_len,times);
					}
				}
				else{
					times++;
					WriteLog(1,"****recv msg_head's syn char faild,recv_len:%d, times:%d****",recv_len,times);
				}
			}
		}

		if (hdr_find){
			int recv_len = m_rCommPort->read_bytes(p,msg_body_lengh);
			if (recv_len == msg_body_lengh)
			{
				WriteLog(3,"接收后继报文体内容,长度:%d,buf:****%s****",2+msg_body_lengh,buf);
				MessageLog((unsigned char*)buf,2+msg_body_lengh,false);
				if ((buf[38] == MSG_CHAR_SUM)&&
					(buf[41]==MSG_CHAR_CR_)&&(buf[42]==MSG_CHAR_LF_))
				{
					//接收一帧完整报文
					//WriteLog(3,"检查报文体特征,长度:%d",2+msg_body_lengh);
					//MessageLog((unsigned char*)buf,2+msg_body_lengh,false);
					buf[43] = 0;
					//WriteLog(3,"msg*********: %s",buf);

					//当未配置时作为测试底层协议工具---rwp
// 						if (m_pShell->isendhost)						{
// 							ttysDataHandler(buf,43);
// 						}
					ttysDataHandler(buf,43);
					ret = 0;
					break;
				}else
				{
					//处理中间乱序问题
					//WriteLog(2,"检查报文体特征失败，本帧非法丢弃");
					for (int i=0; i<msg_body_lengh; i++)
					{
						if (((buf[2+i])==MSG_CHAR_CR_)&&((buf[2+i+1])==MSG_CHAR_LF_))
						{
							//本次接收结束，避免乱序，不影响下帧
							WriteLog(2,"报文结束符在帧中部(pos:%d)，本帧非法丢弃",i);
							break;
						}
					}
				}
			}else
			{
				//WriteLog(1,"接收后继报文体内容不足,长度:%d",recv_len);
				if ((recv_len>0)&&(recv_len<=msg_body_lengh))					{
					//MessageLog((unsigned char*)buf,2+msg_body_lengh,false);
					p += recv_len;//指向接收编移
					int left_bytes = msg_body_lengh - recv_len;
					recv_len = m_rCommPort->read_bytes(p,msg_body_lengh);
					if (recv_len == left_bytes)
					{
						if ((buf[38] == MSG_CHAR_SUM)&&
							(buf[41]==MSG_CHAR_CR_)&&(buf[42]==MSG_CHAR_LF_))
						{
							//接收一帧完整报文
							//WriteLog(3,"检查报文体特征,长度:%d",2+msg_body_lengh);
							//MessageLog((unsigned char*)buf,2+msg_body_lengh,false);
							buf[43] = 0;
							//WriteLog(3,"msg*********: %s",buf);

							//当未配置时作为测试底层协议工具---rwp
// 								if (m_pShell->isendhost)						{
// 									ttysDataHandler(buf,43);
// 								}
							ttysDataHandler(buf,43);
							ret = 0;
							break;
						}
					}else
					{
						//接收不全中止本次
						WriteLog(1,"接收剩余报文失败(recv_len:%d),中止接收",recv_len);
						break;
					}
				}
			}
		}

	}while ((times<MSG_MAX_LENGH)&&!m_bExit);

	return ret;
}
int CttyS1000::ttysDataHandler(unsigned char * pASDU, unsigned char len)
{
	//本地时间更新
	if ((m_pShell->isetTime==1)&&(!setlocalTimeFlag))	{
		if (0 == modifyLocalTime(pASDU,len))
		{
			setlocalTimeFlag = true;
		}
	}
	//委托上层处置
	if (m_pShell->isendhost)
	{
		WriteLog(3,"ttyS1000推数据到上层处置");
		if (m_callbackASDUHandler)
		{
			m_callbackASDUHandler(pASDU,len,m_callbackObj);
		}
	}
	return 0;
}

void CttyS1000::register_asdu_handler( FUNC_ASDU_HANDLER pHandler,void* pCallObj )
{
	m_callbackASDUHandler = pHandler;
	m_callbackObj = pCallObj;
}

void CttyS1000::stop()
{
	m_bExit = true;

	closeAllThread();
}
void CttyS1000::WriteLog(int iLevel,const char *format, ...)
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
		printf("[CttyS1000] %s %s\n",time_prt,buffer);	
    }
	else{
		printf("[CttyS1000] %s\n",buffer);	
	}
}

bool CttyS1000::set_serial(unsigned short  com_id, //串口编号
	unsigned int    baud,   //波特率
	unsigned short  addr,
	unsigned char  parity,
	unsigned char  databits,
	unsigned char  stopbits
	)
{

	m_rCommPort  = new CComPort();
	if (m_rCommPort==NULL){
		WriteLog(1,"实例化串口对象失败");
		return false;
	}
	m_rCommPort->do_config(com_id,baud,databits,stopbits,'n',1000,1000);
	if (0 != m_rCommPort->open()){
		WriteLog(1,"打开串口(%d)错误！\n",com_id);
		return false;
	}

	//清除读写缓冲区，因为缓冲区一直存在“到达而未读”报文
	m_rCommPort->purge_comm(1,1);
	WriteLog(1,"串口初始化完成!数据位:%d,停止位:%d,检验:%d",databits,stopbits,parity);
	return true;
}

void CttyS1000::MessageLog(unsigned char *buf,int len,bool bSend)
{
	if (m_logLevel < 3)	{
		//日志级别大于3才打印
		return;
	}

	static const string strData[256] =
	{
		"00","01","02","03","04","05","06","07","08","09","0A","0B","0C","0D","0E","0F",
		"10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F",
		"20","21","22","23","24","25","26","27","28","29","2A","2B","2C","2D","2E","2F",
		"30","31","32","33","34","35","36","37","38","39","3A","3B","3C","3D","3E","3F",
		"40","41","42","43","44","45","46","47","48","49","4A","4B","4C","4D","4E","4F",
		"50","51","52","53","54","55","56","57","58","59","5A","5B","5C","5D","5E","5F",
		"60","61","62","63","64","65","66","67","68","69","6A","6B","6C","6D","6E","6F",
		"70","71","72","73","74","75","76","77","78","79","7A","7B","7C","7D","7E","7F",
		"80","81","82","83","84","85","86","87","88","89","8A","8B","8C","8D","8E","8F",
		"90","91","92","93","94","95","96","97","98","99","9A","9B","9C","9D","9E","9F",
		"A0","A1","A2","A3","A4","A5","A6","A7","A8","A9","AA","AB","AC","AD","AE","AF",
		"B0","B1","B2","B3","B4","B5","B6","B7","B8","B9","BA","BB","BC","BD","BE","BF",
		"C0","C1","C2","C3","C4","C5","C6","C7","C8","C9","CA","CB","CC","CD","CE","CF",
		"D0","D1","D2","D3","D4","D5","D6","D7","D8","D9","DA","DB","DC","DD","DE","DF",
		"E0","E1","E2","E3","E4","E5","E6","E7","E8","E9","EA","EB","EC","ED","EE","EF",
		"F0","F1","F2","F3","F4","F5","F6","F7","F8","F9","FA","FB","FC","FD","FE","FF"
	};

	time_t now;
	time(&now);
	struct tm* now_time = localtime(&now);
	char time_prt[50]="";
	sprintf(time_prt, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d ",
		now_time->tm_year + 1900, now_time->tm_mon + 1,
		now_time->tm_mday, now_time->tm_hour,
		now_time->tm_min, now_time->tm_sec);


	string   str    = time_prt;
	char szlen[32];
	sprintf(szlen,"%d", len);

	if (bSend)
	{
		str = string(str) +string(" SEND len = ") + string(szlen) + string(" bytes ");
	}
	else
	{
		str = string(str) +string(" RECV len = ") + string(szlen) + string(" bytes  ");
	}

	for (int i = 0 ; i < len ; i++)
	{
		unsigned char value = buf[i];
		str += strData[value] + " " ;
	}

	//3级日志才打印报文
	WriteLog(3,"%s",str.c_str());
}

int CttyS1000::modifyLocalTime(unsigned char* pASDU, unsigned int inleng)
{
	//例：TS20050221021538F50.0043C021535+003.78*1C

	int ret(-1);
	if (((pASDU)&&(inleng >= 43)))
	{
		unsigned char* p = pASDU;
		//同步头'TS'
		p += 2; 

		//提取时间串
		char ymd_str[15]="";
		memcpy(ymd_str,p,14);

		WriteLog(1,"收到串口时间:%s",ymd_str);

		int year(0);
		int month(0);
		int day(0);
		int hour(0);
		int minut(0);
		int seconds(0);
		char tempChar[5]="";

		memset(tempChar,0,sizeof(tempChar));
		memcpy(tempChar,p, 4);
		year = atoi(tempChar);
		p += 4;

		memset(tempChar,0,sizeof(tempChar));
		memcpy(tempChar,p, 2);
		month = atoi(tempChar);
		p += 2;

		memset(tempChar,0,sizeof(tempChar));
		memcpy(tempChar,p, 2);
		day = atoi(tempChar);
		p += 2;

		memset(tempChar,0,sizeof(tempChar));
		memcpy(tempChar,p, 2);
		hour = atoi(tempChar);
		p += 2;

		memset(tempChar,0,sizeof(tempChar));
		memcpy(tempChar,p, 2);
		minut = atoi(tempChar);
		p += 2;

		memset(tempChar,0,sizeof(tempChar));
		memcpy(tempChar,p, 2);
		seconds = atoi(tempChar);
		p += 2;

		struct tm tm_ptr;  
		struct timeval tv;  
		struct timezone tz;  
		gettimeofday (&tv , &tz);//获取时区保存tz中  
		tm_ptr.tm_year = year - 1900;  
		tm_ptr.tm_mon  = month - 1;  
		tm_ptr.tm_mday = day;  
		tm_ptr.tm_hour = hour;  
		tm_ptr.tm_min  = minut;  
		tm_ptr.tm_sec  = seconds;  
		time_t utc_t = mktime(&tm_ptr);  
		tv.tv_sec = utc_t;  
		tv.tv_usec = 0;  

		if (settimeofday(&tv, &tz) < 0)
		{
			WriteLog(1,"设置系统时间失败");
			ret  = -1;
		}else
		{
			WriteLog(1,"设置系统时间成功");
			ret = 0;
		}
	}

	return ret;
}


