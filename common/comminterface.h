#ifndef COMMON_INTERFACE_H
#define COMMON_INTERFACE_H
#include <string>
#include "classafx.h"

#define  SOCKET_TCP               1    // tcp
#define  SOCKET_UDP_PTP           2    //��Ե�
#define  SOCKET_UDP_MULTICAST     3    //�鲥 
#define  SOCKET_UDP_BROADCAST     4    //�㲥
using namespace std;


class CusomSocket
{
public :
   
	CusomSocket(){m_connectFlag = false;m_type = SOCKET_TCP;};
	CusomSocket(unsigned char type ){	m_type = type;  m_connectFlag = false;};
	~CusomSocket(){closeSocket();};

	bool    createSocket(unsigned char type = SOCKET_TCP);
	bool    closeSocket();
	bool    connectSocket(string ipStr,unsigned int port);
	bool    connectSocket1();
	bool    connectSocket();
	unsigned int  sendData(unsigned char *buf,unsigned int len,unsigned int timeout = 1000);
	int       	  recvData(unsigned char *buf,unsigned int len,unsigned int timeout = 1000);
	unsigned int  recvUdpData(unsigned char *buf,int len, char *addr,unsigned int timeout = 10000);
	int     getSocket();
	void    setSocket(unsigned int socket);
	bool    getConnectStatus(){return m_connectFlag;};
	string  getErrStr(unsigned char err);

	//UDP
	void    setLocalIp(string strIP){m_localIp = strIP;};
	void    setLocalPort(unsigned int port){m_localPort = port;};
	void    setDestIp(string strIP){m_destIp = strIP;};
	void    setDestPort(unsigned short port){m_destPort = port;};
	unsigned char udpBind();
private :
    unsigned int   m_socket;
	unsigned char  m_type;
	string    	   m_localIp;
	unsigned int   m_localPort;
	string    	   m_destIp;
	unsigned int   m_destPort;
	bool       	   m_connectFlag;       //���ӱ�־
};


class CusomListenSocket  :public CusomSocket
{

public:

	CusomListenSocket():CusomSocket(){};
	~CusomListenSocket(){closeSocket();};

public:

	bool    CreateListenSocket(unsigned int port);
	unsigned  int  AcceptSocket(string &ipStr);

};

//�����߳� threadtype = 0Ĭ��Ϊ����״̬  1Ϊ�ȴ�����ģʽ
bool Comm_CreateThread(_HANDLE &handle ,ptrFun funName,void *funParam,unsigned char threadtype = 0);

//�ر��߳�
bool Comm_exitThread(_HANDLE &handle);

unsigned int Comm_GetTickCount();

void  Comm_MsgTrace(string str,string model="");

void  Comm_WaitObjectfinshed(_HANDLE &handle);

//�׽��ֵĳ�ʼ��
bool SocketDLLInit();
bool SocketDllRelease();





#endif // COMMON_INTERFACE_H
