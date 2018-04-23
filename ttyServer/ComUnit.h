#ifndef COMUNIT_H
#define COMUNIT_H
#include "../common/classafx.h"
#include "../common/comminterface.h"
#include "../common/Report.h"
#include "../common/gpsprostruct.h"
#include "../common/gpscomdef.h"
#include <string>
#include <list>

#if defined(Q_OS_UNIX)
#include <sys/time.h>
#endif

using namespace std;
#define MSG_BUFFER_LENGTH 100

class Kw104Server;
class ComUnit
{
public:
	ComUnit(Kw104Server * pServer, string &strIp);
	~ComUnit(void);
public:

	int startallthread(unsigned int socket);
    int newinf(unsigned char *pASDU,  unsigned short len);

public:
	 void  recivefun();

private:
    static void recivethread(void *lp);
private:

	void recivedata();
	int ReceData(unsigned char *buf, int len,int nTimeout = 10 * 1000);
	int ReciveBuffer(unsigned char *buf, int len,int nTimeout = 10 * 1000);
    int makereport(list<GPSPROSTRUCT_BASE*> & list, unsigned char *pASDU, unsigned short len);
	void WriteLog(int iLevel,const char *format, ...);
	//int TranslateDLT1100(unsigned char* pASDU, unsigned int inleng, unsigned char* pDLT1100, unsigned int& outleng);
private:

	CusomSocket *m_pSocket;   //连接的套接字
	

	bool m_bExit;
	_HANDLE m_recivehandle;

    CSafeMsgDeque m_dataDeque; //数据队列

	Kw104Server *m_pServer;
	string  m_strIp;

	int m_logLevel;
};
#endif;
