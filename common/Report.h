#ifndef REPORT_H
#define REPORT_H
#include <list>
#include <string>
#include <iostream> 
#include <vector>

#include <stdlib.h>  
#include <string.h> 
#include <stdio.h> 

using namespace std;

//增加条件编译
#if defined(Q_OS_UNIX)
 #include <pthread.h>
 #include <time.h>  

#elif defined(WIN32)
#include <QMutex>
#include <QDateTime>

#endif


//报文显示查找表，到时在向外发送报文
static char  *report[256] =
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


//建立自定义锁
class CMyMutex
{
public:
	CMyMutex()
	{
	   #if defined(Q_OS_UNIX)
        m_mutex  = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
		pthread_mutex_init(&m_mutex, NULL);
       
       #endif

	}
	~CMyMutex()
	{
        #if defined(Q_OS_UNIX)	    
		  pthread_mutex_destroy(&m_mutex);
	    #endif
	}
	void lock()
	{
        #if defined(Q_OS_UNIX)
        pthread_mutex_lock(&m_mutex);

#elif defined(WIN32)
     m_ComMutex.lock();
#endif
	
	
	}
	void unlock()
	{
	   #if defined(Q_OS_UNIX)
       pthread_mutex_unlock(&m_mutex);

     #elif defined(WIN32)
       m_ComMutex.unlock();
     #endif
	
	
	}
private:

     
#if defined(Q_OS_UNIX)
    
	pthread_mutex_t m_mutex; 

#elif defined(WIN32)
     QMutex  m_ComMutex;
#endif
	 
    
   

};

class CReport
{


public:
	CReport(void);

public:
	~CReport(void);
	 static int ShowDataFrm(void * buf, int len ,const char * from , const char *to,  bool dec);
	 static  string GetShowReport(void * buf, int len ,const char * from , const char *to,  bool dec);
private:
  
};
//建立安全的数据队列

struct FRAME
{
	int DataLen;
	unsigned char *pData;
	FRAME(unsigned char *p, int len)
	{		
		DataLen = len;
		pData = new unsigned char[len];
		memcpy(pData, p, len);
	}
	~FRAME()
	{
		if (pData) delete []pData;
	}

};
//创健报文队列
class CSafeMsgDeque
{
public:	
	CSafeMsgDeque()
	{
		
	}
	~CSafeMsgDeque()
	{
		Clear();
		
	}
	void Clear();
	bool PushBack(unsigned char *pMsgBuffer,unsigned short uiMsgSize);
	int  PopFront(unsigned char *pMsgBuffer,int bufflen, unsigned short & uiMsgSize);
	bool PopFront(int wCount);
	int  Size();

private:

	list<FRAME*> m_List;
	CMyMutex  m_ComMutex;
};
#define REPORT_1900TIME  0X83AA7E80
extern unsigned long report_mktime(const unsigned int year0, const unsigned int mon0,
       const unsigned int day, const unsigned int hour,
       const unsigned int min, const unsigned int sec);
extern  unsigned long long  report_gettime64();
extern void   report_int64tostrtime(unsigned long long t);

/* 
得到秒与微妙
*/
extern void   report_getsecandusec(unsigned int & sec,  unsigned int & usec);
extern unsigned int report_gettimesec();
extern string report_gettimestr();

#endif;


