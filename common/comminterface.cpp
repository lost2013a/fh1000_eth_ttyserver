#include "comminterface.h"
//#include <QtDebug>
//#include "classafx.h"

#if defined(Q_OS_WIN)

#elif defined(Q_OS_UNIX)
	#include <unistd.h>
	#include <sys/socket.h>
	#include <resolv.h>
	#include <sys/ioctl.h>
	#include <net/if.h>
	#include <netinet/if_ether.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netinet/tcp.h>
#include <string>
#endif


bool   CusomSocket::createSocket(unsigned char type)
{
	
	 closeSocket();
	
#if defined(Q_OS_WIN) 

	 if (type == SOCKET_TCP)
	   m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	 else 
		// m_socket = socket( PF_INET, SOCK_DGRAM, 0);//
	   m_socket = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP);

#elif defined(Q_OS_UNIX)
	  
	 if (type == SOCKET_TCP)
	    m_socket = socket(PF_INET, SOCK_STREAM, 0);
	 else
		m_socket = socket(PF_INET, SOCK_DGRAM, 0);

 
#endif

	  if (m_socket < 0 )
		  return false;

	  return true;

}



int  CusomSocket::getSocket()
{

	return m_socket;
}

void  CusomSocket::setSocket(unsigned int socket)
{

    m_socket = socket;
}


bool  CusomSocket::closeSocket()
{

	if (m_socket == INVALID_SOCKET)
		return false;


 #if defined(Q_OS_WIN) 
  
	closesocket(m_socket);

#elif defined(Q_OS_UNIX) 
	
	::close(m_socket);
#endif

	m_socket = INVALID_SOCKET;
	m_connectFlag = false;

	return true;

}

string CusomSocket::getErrStr(unsigned char err)
{

	string str = "";

	switch(err)
	{
	case 0:
		str = "";
		break;
	case 1:
		str = "UNIX  端口复用设置失败\r\n";
		break;
	case 2:
		str = "端口与socket绑定失败\r\n";
		break;
	case 3:
		str = "加入到组播组中失败\r\n";
		break;
	case 4:
		str = "设置广播失败\r\n";
		break;
	default:
		str = "未知的错误\r\n";
		break;
	}

	return str;

}


unsigned char CusomSocket::udpBind()
{

	//Q_ASSERT(m_socket);

	struct sockaddr_in addr;

	#if defined(Q_OS_WIN) 

		addr.sin_family = AF_INET;
		addr.sin_port = htons(m_localPort); 
		if (m_localIp=="")
		{
		    addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
		}
		else
		{
			addr.sin_addr.s_addr = inet_addr(m_localIp.c_str()); 
		}
		bool l = true;
		setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&l, sizeof(l)); 
	
    #elif defined(Q_OS_UNIX)

		int					i = 1;
	    bzero(&addr, sizeof(addr));
	    addr.sin_family = PF_INET;
	    addr.sin_port = htons( m_localPort);
		if (m_localIp== "")
		{
		   // addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
		}
		else
		{
			inet_aton( m_localIp.c_str(), &addr.sin_addr);
		}
	    if( m_type == SOCKET_UDP_MULTICAST)
		addr.sin_addr.s_addr	= inet_addr( m_destIp.c_str());
				
	  //设置端口复用
	  if( setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, ( char * )&i, sizeof( i )) < 0) 
	  {
			closeSocket();
			return 1;
	  }

    #endif


	  //将端口与SOCKET绑定
	if( bind( m_socket, ( struct sockaddr *)&addr, sizeof( struct sockaddr_in)) < 0 )
	{
		closeSocket();
		return 2;
	}	

	if (m_type == SOCKET_UDP_MULTICAST)
	{

		struct ip_mreq 			mreq;
		mreq.imr_multiaddr.s_addr = inet_addr( m_destIp.c_str());
		mreq.imr_interface.s_addr = inet_addr( m_localIp.c_str());
		//mreq.imr_interface.s_addr = htons(INADDR_ANY);

		//加入到组播组中
		if( setsockopt( m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, ( char * )&mreq, sizeof( mreq)) < 0) 
		{
			#if defined(Q_OS_WIN) 
			   QString str = QString("bind error=%1").arg(WSAGetLastError());
			#elif defined(Q_OS_UNIX)

		    #endif
			
			closeSocket();
			return 3;				
		}
		#if defined(Q_OS_UNIX)

		struct in_addr if_req;
		inet_aton(m_localIp.c_str(), &(if_req));
		if( setsockopt(m_socket, SOL_IP,IP_MULTICAST_IF, &if_req, sizeof(struct in_addr) ) < 0 )
		{

           printf("绑定发送地址失败\n");
		   closeSocket();
            
           return 5;

		}
        #endif


	}
	else if (m_type == SOCKET_UDP_BROADCAST)
	{

		int	i =	1;

		if( setsockopt( m_socket, SOL_SOCKET, SO_BROADCAST, ( char * )&i, sizeof( i )) < 0)
		{
			closeSocket();
			return 4;			
		}

	}

   return 0;

}

bool  CusomSocket::connectSocket()
{
	
	return connectSocket(m_destIp,m_destPort);

}

bool CusomSocket::connectSocket1()
{
	//Q_ASSERT(m_socket);

	struct sockaddr_in addr;

#if defined(Q_OS_WIN) 
	memset(&addr, 0,sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port =  1025;
	addr.sin_addr.s_addr = inet_addr(m_localIp.c_str());
	if(::bind(m_socket, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return false;
	}
	//设置一步套接字
	addr.sin_port = htons(m_destPort); 
	addr.sin_addr.s_addr = inet_addr(m_destIp.c_str()); 
	
	if(::connect(m_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return false;
	}
#elif defined(Q_OS_UNIX)

	bzero(&addr, sizeof(addr));
	addr.sin_family = PF_INET;
	addr.sin_port   = 1025;
	inet_aton( m_localIp.c_str(), &addr.sin_addr);

	//将端口与SOCKET绑定
	if( bind( m_socket, ( struct sockaddr *)&addr, sizeof(addr)) < 0 )
		return false;

	bzero(&addr, sizeof(addr));
	addr.sin_family = PF_INET;
	addr.sin_port = htons( m_destPort);
	inet_aton( m_destIp.c_str(), &addr.sin_addr);

	//连接服务端
	if( ::connect(m_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
 		return false;	
#endif
	m_connectFlag = true;
	return true;
}


bool CusomSocket::connectSocket(string ipStr,unsigned int port)
{
	//Q_ASSERT(m_socket);

#if defined(Q_OS_WIN) 
  
	struct sockaddr_in addr;
	memset(&addr, 0,sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	//addr.sin_port =  0;
	//addr.sin_addr.S_un.S_addr = ::htonl( INADDR_ANY );
	/*if(::bind(m_socket, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return false;
	}*/
	//设置一步套接字
	addr.sin_port = htons(port); 
	addr.sin_addr.s_addr = inet_addr(ipStr.c_str()); 
	
	if(::connect(m_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return false;
	}

	

#elif defined(Q_OS_UNIX) 
	
	struct sockaddr_in dest;
	bzero(&dest, sizeof(dest));
	dest.sin_family = PF_INET;
	dest.sin_port = htons( port);
	inet_aton( ipStr.c_str(), &dest.sin_addr);

	//连接服务端
	if( ::connect(m_socket, (struct sockaddr*)&dest, sizeof(dest)) < 0)
 		return false;	

#endif

	m_connectFlag = true;
	return true;

}

unsigned int  CusomSocket::sendData(unsigned char *buf,unsigned int len,unsigned int timeout)
{
	if (m_socket == INVALID_SOCKET)
		return -1;

	int result = 0;

#if defined(Q_OS_UNIX) 

       if (m_type == SOCKET_TCP){
	   	
	      result= send( m_socket, buf, len, MSG_NOSIGNAL);
		  //printf("SOCKET_TCP:%d\n",result);
       }
	   else
	   {
		   int		        ret	= 0;
		   struct sockaddr_in 	dest;
		   memset(&dest,0,sizeof(dest));
	       dest.sin_family = PF_INET;
		   dest.sin_port = htons(m_destPort); 
	       dest.sin_addr.s_addr = inet_addr(m_destIp.c_str()); 

		   result = sendto( m_socket, buf, len, 0, ( struct sockaddr *)&dest, sizeof(struct sockaddr_in));
		   //printf("sendto:%d\n",result);
	   }

	
#elif defined(Q_OS_WIN)
       if(timeout == 0)
	   {
			if (m_type == SOCKET_TCP)
			  result = ::send(m_socket, (char*)buf, len, 0);
			else
			{
				 int		        ret	= 0;
		         struct sockaddr_in 	dest;
				 memset(&dest,0,sizeof(dest));
	             dest.sin_family = AF_INET;
				 dest.sin_port = htons(m_destPort); 
	             dest.sin_addr.s_addr = inet_addr(m_destIp.c_str()); 
	
				 result = ::sendto( m_socket,(char FAR*) buf, len, 0, ( struct sockaddr *)&dest, sizeof(struct sockaddr_in));
				 //检查发送失败的原因，用返回值表示
                 if (result == -1)
				 {
					 result = WSAGetLastError();
				 }
				 printf("sendto,0\n")
				
			}
	   
	   }
	   else
	   {
			fd_set WriteFds;
			timeval tv;
			tv.tv_sec = timeout / 1000;
			tv.tv_usec = (timeout % 1000) * 1000;
			WriteFds.fd_count = 1;
			WriteFds.fd_array[0] = m_socket;
			if ((result = ::select(m_socket + 1, 0, &WriteFds, 0, &tv)) > 0)
			{
				if (m_type == SOCKET_TCP)
					result = ::send(m_socket, (char*)buf, len, 0);
				else	
				{
					int		        ret	= 0;
					struct sockaddr_in 	dest;
                    dest.sin_family = AF_INET;
				    dest.sin_port = htons(m_destPort); 
	                dest.sin_addr.s_addr = inet_addr(m_destIp.c_str()); 
	
					result = ::sendto( m_socket,(char FAR*) buf, len, 0, ( struct sockaddr *)&dest, sizeof(struct sockaddr_in));
				}
		   }
			printf("sendto,time\n")
	   }
	 
#endif


     return result;


}


 int  CusomSocket::recvData(unsigned char *buf,unsigned int len,unsigned int timeout)
{
    unsigned int  result= 0;

	if (m_socket == INVALID_SOCKET)
		return -1;

	#if defined(Q_OS_UNIX) 

	    int		opts;
	
		opts = fcntl(m_socket, F_GETFL);
		opts &= O_NONBLOCK;
		fcntl(m_socket, F_SETFL, opts);

        struct		timeval tv;
		fd_set		readfds;
		
		tv.tv_sec  = timeout /1000;
		tv.tv_usec = ( timeout % 1000) * 1000;

		FD_ZERO( &readfds);
		FD_SET( m_socket, &readfds);

		switch( select( m_socket + 1, &readfds, NULL, NULL, &tv))
		{
		case -1:

			return -1;		//错误			

		case 0:

			return 0;		//超时

		default:
			
			if( FD_ISSET( m_socket, &readfds))
			{
				if (m_type == SOCKET_TCP){
				  result = recv( m_socket, buf, len, 0);
				  //=0 连接关闭 >0 接收到数据大小
				  // printf("SOCKET_TCP recv : %d \n",result);
				}
				else{
				  struct sockaddr_in	dest;
				  unsigned int sockaddrLen	 = 0;
				  result= recvfrom( m_socket, buf, len, 0, ( struct sockaddr *)&dest, (socklen_t*)&sockaddrLen);
				}
			}
		}

	
	#elif defined(Q_OS_WIN)  

        FD_SET fd = {1, m_socket};
		TIMEVAL tv = {timeout /1000, (timeout%1000) * 1000};
		if ((result = select(0, &fd, NULL, NULL, &tv)) > 0)
		{
			

			if (m_type == SOCKET_TCP)
			  result = ::recv(m_socket, (char FAR*)buf, len, 0);
			else   //UDP
			{
			  //ulong   mode = 0;   
	          //ioctlsocket( m_socket, FIONBIO, ( ulong * )&mode);  

			  struct sockaddr_in	dest;
			  int sockaddrLen	 = sizeof( struct sockaddr_in);;
			  result = recvfrom( m_socket,(char FAR*) buf, len, 0, ( struct sockaddr *)&dest, &sockaddrLen);	
			}
		}

	#endif

     return result;

}

unsigned int   CusomSocket:: recvUdpData(unsigned char *buf,int len, char *addr, unsigned int timeout )
{
	unsigned int  result= 0;

	if (m_socket == INVALID_SOCKET)
		return -1;

	#if defined(Q_OS_UNIX) 

	    int		opts;
	
		opts = fcntl(m_socket, F_GETFL);
		opts &= O_NONBLOCK;
		fcntl(m_socket, F_SETFL, opts);

        struct		timeval tv;
		fd_set		readfds;
		
		tv.tv_sec  = timeout /1000;
		tv.tv_usec = ( timeout % 1000) * 1000;

		FD_ZERO( &readfds);
		FD_SET( m_socket, &readfds);

		switch( select( m_socket + 1, &readfds, NULL, NULL, &tv))
		{
		case -1:

			return -1;		//错误			

		case 0:

			return 0;		//超时

		default:
			
			if( FD_ISSET( m_socket, &readfds))
			{
				if (m_type == SOCKET_TCP)
				  result = recv( m_socket, buf, len, 0);
				else
				{
				  struct sockaddr_in	dest;
				  int sockaddrLen	 = sizeof(dest);
				  result= recvfrom( m_socket, buf, len, 0, ( struct sockaddr *)&dest, (socklen_t*)&sockaddrLen);
				  //if (result == 5)
				  {
					 strcpy(addr, inet_ntoa(dest.sin_addr));
				  }
				}
			}
		}

	
	#elif defined(Q_OS_WIN)  

        FD_SET fd = {1, m_socket};
		TIMEVAL tv = {timeout /1000, (timeout%1000) * 1000};
		if ((result = select(0, &fd, NULL, NULL, &tv)) > 0)
		{
			

			if (m_type == SOCKET_TCP)
			  result = ::recv(m_socket, (char FAR*)buf, len, 0);
			else   //UDP
			{
			  //ulong   mode = 0;   
	          //ioctlsocket( m_socket, FIONBIO, ( ulong * )&mode);  

			  struct sockaddr_in	dest;
			  int sockaddrLen	 = sizeof( struct sockaddr_in);
			  result = recvfrom( m_socket,(char FAR*) buf, len, 0, ( struct sockaddr *)&dest, &sockaddrLen);
			  //if (result == 5)
			  {
				strcpy(addr, inet_ntoa(dest.sin_addr));
			  }

			}
		}

	#endif

     return result;



}

bool CusomListenSocket::CreateListenSocket(unsigned int port)
{

	if (!createSocket())
		return false;

	int socket = getSocket();

	// Q_ASSERT(socket);

#if defined(Q_OS_WIN) 
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = ::htons(port);
	addr.sin_addr.s_addr = ::htonl(INADDR_ANY);

	bool l = true;
	::setsockopt(socket,SOL_SOCKET,SO_REUSEADDR,(char *)&l,sizeof(l)); //重用端口设置


	if(SOCKET_ERROR == ::bind(socket, (struct sockaddr*)&addr, sizeof(addr)))
	{	
		//QString str = QString("bind error=%1").arg(WSAGetLastError());
		//printf() << str;
		return false ;
	}

	if(SOCKET_ERROR == ::listen(socket, SOMAXCONN))
	{
		//QString str = QString("listen error=%1").arg(WSAGetLastError());
		//printf() << str;
		return false;
	}
   
 
#elif defined(Q_OS_UNIX)
	
	 struct sockaddr_in	dest;
	 int					i = 1;

	 //初始化目标结构
	 bzero((void*)&dest, sizeof(dest));
	 dest.sin_family = PF_INET;
	 dest.sin_port = htons( port);

	 dest.sin_addr.s_addr = INADDR_ANY;

	 //设置端口复用
	 if( setsockopt( socket, SOL_SOCKET, SO_REUSEADDR, &i, sizeof( i)) < 0) 
		return false;

	 if(0){
		int keepalive = 1; // 开启keepalive属性
		int keepidle = 60; // 如该连接在60秒内没有任何数据往来,则进行探测
		int keepinterval = 5; // 探测时发包的时间间隔为5 秒
		int keepcount = 3; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.
		setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive , sizeof(keepalive ));
		setsockopt(socket, SOL_TCP, TCP_KEEPIDLE, (void*)&keepidle , sizeof(keepidle ));
		setsockopt(socket, SOL_TCP, TCP_KEEPINTVL, (void *)&keepinterval , sizeof(keepinterval ));
		setsockopt(socket, SOL_TCP, TCP_KEEPCNT, (void *)&keepcount , sizeof(keepcount ));
	 }


	 //将端口与SOCKET绑定
	 if( bind( socket, (struct sockaddr*)&dest, sizeof(dest)) < 0 )
 		return false;		

	 //侦听
	 if( listen( socket, 20) < 0)
 		return false;				

#endif


	 return true;

}


unsigned int CusomListenSocket::AcceptSocket(string &ipStr)
{
    int socket = -1;

	int listenSocket = getSocket();

#if defined(Q_OS_WIN) 

	
	struct sockaddr_in tcpaddr = {'\0'};
	int len = sizeof(struct sockaddr_in);


	socket =  ::accept(listenSocket, (struct sockaddr *)&tcpaddr, (int *)&len);
	ipStr = string(inet_ntoa(tcpaddr.sin_addr));
 
#elif defined(Q_OS_UNIX)

    struct sockaddr_in	client_addr;
	fd_set				rfds;
	socklen_t 			addrlen = sizeof(client_addr);

	FD_ZERO( &rfds);

	FD_SET(listenSocket, &rfds);

	socket =  accept(listenSocket, (struct sockaddr*)&client_addr, &addrlen);

	char  strIpAddr[50] = {'\0'};		
	strcpy( strIpAddr, (const char *)( inet_ntoa( client_addr.sin_addr)));

	//string  str(strIpAddr);

	ipStr = strIpAddr;


#endif

	return socket;

}


bool Comm_CreateThread(_HANDLE &handle ,ptrFun funName,void *funParam, unsigned char threadtype)
{

	bool ret = true ;

	#if defined(Q_OS_UNIX) 

	    pthread_attr_t		threadAttrib;		//线程属性

		//初始化线程属性
	    pthread_attr_init( &threadAttrib);
	
	    //设置线程绑定属性( 绑定:PTHREAD_SCOPE_SYSTEM, 非绑定:PTHREAD_SCOPE_PROCESS)
	    pthread_attr_setscope( &threadAttrib, PTHREAD_SCOPE_SYSTEM);
	
	    //设置线程分离属性( 分离:PTHREAD_CREATE_DETACHED, 非分离:PTHREAD_CREATE_JOINABLE)
		if (threadtype == 0)
		{
			pthread_attr_setdetachstate( &threadAttrib, PTHREAD_CREATE_DETACHED);
		}
		else
		{
		   pthread_attr_setdetachstate( &threadAttrib, PTHREAD_CREATE_JOINABLE);
		}
		int value  = pthread_create(&handle, &threadAttrib, funName, (void*)funParam);
		if (value != 0)	
			ret = false;

	#elif defined(Q_OS_WIN)

		handle = (_HANDLE)_beginthread (funName, 0, funParam);
		if (handle == NULL)
			ret = false;
	#endif

	return ret;


}
void Comm_WaitObjectfinshed(_HANDLE &handle)
{
     #if defined(Q_OS_UNIX)
	   pthread_join(handle, 0);
     #elif defined(Q_OS_WIN)
	  //检查这个函数是否需要等待
	 DWORD		dwExitCode		= 0; 
	 sleep(200);
	::GetExitCodeThread(handle,&dwExitCode);
	if(dwExitCode == STILL_ACTIVE)
	{
       ::WaitForSingleObject(handle, INFINITE);
	}	 
     #endif

}

bool Comm_exitThread(_HANDLE &handle)
{

#if defined(Q_OS_WIN) 

	//TerminateThread((HANDLE)handle, 1L);
	//CloseHandle((HANDLE)handle);
	//handle = NULL;
	_endthread();

 
#elif defined(Q_OS_UNIX)

	 pthread_exit(NULL);

#endif

	 return true;

}


unsigned int Comm_GetTickCount()
{
	#if defined(Q_OS_UNIX) 
		
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

}  


void Comm_MsgTrace(string str,string model)
{
	

#if defined(_DEBUG_) 
	 
	//QString strInfo = QString("%1%2").arg(model == ""? "":QString("%1").arg(model) ).arg(str);
	 // printf() << strInfo.toAscii().data();


#endif
}

bool SocketDLLInit()
{
	
	#if defined(Q_OS_WIN) 
		WSADATA WSAData;
		if (WSAStartup(MAKEWORD(2,2), &WSAData) != 0 )
		{
        
			return false;
		}
		return true;
	#endif
   return true;
}
bool SocketDllRelease()
{
   #if defined(Q_OS_WIN) 
	WSACleanup();
   #endif
   return true;
}


