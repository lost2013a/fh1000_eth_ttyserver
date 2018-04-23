#include "Report.h"


#ifndef WIN32
#include <sys/time.h>

#else
   #include <QString>
   #include <QDateTime>
#endif


unsigned int report_gettimesec()
{
	 unsigned int t = 0;
#ifndef WIN32
    
	 t = time(NULL); 

#else
     
	 t = QDateTime::currentDateTime().toTime_t ();
#endif
    return t;
}
string report_gettimestr()
{
	 string strt;
	 char szbuf[128]; 
  #ifndef WIN32
      	struct tm		*sysTime;
	    struct timeval  tv;

  	//取得系统时间
	  gettimeofday( &tv, NULL);
  	  sysTime = localtime( &tv.tv_sec);
     
	  sprintf(szbuf, "%04d-%02d-%02d %02d:%02d:%02d.%03d", sysTime->tm_year + 1900,
		                               sysTime->tm_mon  + 1,sysTime->tm_mday,sysTime->tm_hour,sysTime->tm_min,           
		                               sysTime->tm_sec, tv.tv_usec / 1000);
	  strt = szbuf;
	
	 
#else
      QDateTime t = QDateTime::currentDateTime ();
      strt  = t.toString ("yyyy-MM-dd hh:mm:ss.zzz").toAscii().data();
	 
#endif

    return strt;
}


unsigned long report_mktime(const unsigned int year0, const unsigned int mon0,
       const unsigned int day, const unsigned int hour,
       const unsigned int min, const unsigned int sec)
{
 	unsigned int mon = mon0, year = year0;

 	/* 1..12 -> 11,12,1..10 */
 	if (0 >= (int) (mon -= 2)) 
	{
  	mon += 12; /* Puts Feb last since it has leap day */
  	year -= 1;
 	}

 	return ((((unsigned long)(year/4 - year/100 + year/400 + 367*mon/12 + day) +year*365 - 719499)*24 + hour /* now have hours */)*60 + min /* now have minutes */)*60 +sec;/*finallyseconds */
}
unsigned long long  report_gettime64()
{
	unsigned int  sec=0,usec =0;
	report_getsecandusec(sec, usec);

	 return  (((unsigned long long )sec) << 32) + usec;

}
void  report_int64tostrtime(unsigned long long  t)
{
       
	unsigned int sec = t >> 32; //得到秒
	unsigned int msec = (t & 0x0000FFFFFFFF);

	
	printf("printf sec = %u\n", sec);
	sec = sec - REPORT_1900TIME;
	
	#if defined(Q_OS_WIN)
		QDateTime dt;
		dt.setTime_t(sec);
		QDateTime tmpt = dt.toLocalTime(); 

		printf(" czy = %04d-%02d-%02d %02d:%02d:%02d.%06d\n", tmpt.date().year(),tmpt.date().month(),tmpt.date().day(),
		                            tmpt.time().hour(),tmpt.time().minute(),tmpt.time().second(), msec);
    #endif  


}
void   report_getsecandusec(unsigned int  & sec,  unsigned int & usec)
{
	#if defined(Q_OS_WIN)
     
	 QDateTime t = QDateTime::currentDateTime(); 

	 unsigned int mec = report_mktime(t.date().year(),t.date().month(),t.date().day(),t.time().hour(),t.time().minute(),t.time().second());
	 sec = mec +  REPORT_1900TIME - 8 * 3600;

	 unsigned int tmpusec = t.time().msec();
	 usec =  tmpusec * 1000L;
    
    #elif defined(Q_OS_UNIX)
    //clock_gettime(clockid_t clk_id, struct timespect *tp); //这个函数可以取得纳秒
	  struct timeval tv;
      gettimeofday(&tv, NULL);  //取的微妙
      sec = tv.tv_sec + REPORT_1900TIME;
	  usec = tv.tv_usec;
    #endif



}

CReport::CReport(void)
{
	 
}

CReport::~CReport(void)
{
	
}
int CReport::ShowDataFrm(void * buf, int len ,const char * from , const char *to,  bool dec)
{
  
  string strTmp =report_gettimestr();
  char szbuf[256];
  sprintf(szbuf,"%s --> %s  %s %d bytes \n", from, to,dec?"Send >> ":"Recive << ",len);
  unsigned char *p = (unsigned char*)buf;
  len =  len > 128?128:len;
  for (int i = 0; i < len; i++)
  {
	  if ((i + 1) % 8 == 0)
	  {
		    strTmp += report[p[i]] + string("  ");
	  }
	  else
	  {
		   strTmp += report[p[i]] + string(" ");
	  }
	 if ((i + 1) % 32 == 0 )
	 {
		 strTmp += "\n";
	 }
   }
   if ((len) % 32 != 0)
   {
	   strTmp += "\n";
   }
   strTmp += "\n";
   printf("%s", strTmp.c_str());
   
   return 0;
} 

string CReport::GetShowReport(void * buf, int len ,const char * from , const char *to,  bool dec)
{
  string strTmp = report_gettimestr();
  strTmp += "\n";
  strTmp += string(from) + string("-->") + string(to) + string("  ");
  strTmp += dec? "Send >> ":"Recive << ";
  char szbuf[64];
  sprintf(szbuf,"%d",len);
  string strlen = string(szbuf) + string("bytes \n");
  strTmp += strlen;

  unsigned char *p = (unsigned char*)buf;
  //len =  len > 32?32:len;
  for (int i = 0; i < len; i++)
  {

	  if ((i + 1) % 8 == 0)
	  {
		    strTmp += report[p[i]] + string("  ");
	  }
	  else
	  {
		   strTmp += report[p[i]] + string(" ");
	  }
	 if ((i + 1) % 32 == 0 )
	 {
		 strTmp += "\n";
	 }
   }
   if ((len) % 32 != 0)
   {
	   strTmp += "\n";
   }
   strTmp += "\n";
   return strTmp;

}
void CSafeMsgDeque::Clear()
{
	 m_ComMutex.lock ();
     while(m_List.size ())
	 {
		FRAME *pFrme =  m_List.front ();
		m_List.pop_front ();
		delete pFrme;
	 }
 	 m_ComMutex.unlock ();
	
}
bool CSafeMsgDeque::PushBack(unsigned char *pMsgBuffer,unsigned short uiMsgSize)
{
	FRAME * pFrame = new FRAME(pMsgBuffer, uiMsgSize);
	m_ComMutex.lock ();
	m_List.push_back(pFrame);
	m_ComMutex.unlock ();
	return true;
}

int  CSafeMsgDeque::Size()
{
    int nCount = 0;
	m_ComMutex.lock ();
	nCount  = m_List.size ();
	m_ComMutex.unlock ();
	return nCount;
}

int CSafeMsgDeque::PopFront(unsigned char *pMsgBuffer,int bufflen, unsigned short & uiMsgSize)
{
	uiMsgSize = 0;
	m_ComMutex.lock (); //yhc
	if (m_List.size () > 0)
	{
		/*list<FRAME*>::iterator it = m_List.begin();
		for (; it != m_List.end();)
		{
		    if (1)
			{
			  it = m_List.erase(it);
			}
			else
			{
			  it++;
			}
		
		}*/
		
		FRAME *pFrme =  m_List.front ();
		m_List.pop_front ();
		m_ComMutex.unlock ();
		if (bufflen >= pFrme->DataLen)
		{
			memcpy(pMsgBuffer,pFrme->pData, pFrme->DataLen); 
			uiMsgSize = pFrme->DataLen;
			delete pFrme;
			return 1L;
		}
		else
		{
			delete pFrme;
			
		}
	}
	else
	{
		m_ComMutex.unlock ();
	}
    return 0L;
}


bool CSafeMsgDeque::PopFront(int wCount)
{
	m_ComMutex.lock ();
    for (int i = 0; i < wCount; i++)
	{
		if (m_List.size () > 0)
		{
			FRAME *pFrme =  m_List.front ();
		    m_List.pop_front ();
			delete pFrme;
		
		}
	}
	m_ComMutex.unlock ();
	return m_List.size () > 0;
}
