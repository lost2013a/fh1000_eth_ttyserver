/*****************************************************************************************
					文件的注释
*****************************************************************************************/

#include  <termio.h>
#include  <math.h>
#include  <strings.h>
#include "mgettext.h"
#include  "prot_def.h"
#include  "globe.h"

#include  "sam_ip_get.h"

/*****************************************************************************************/

int	tty_fid=-1;
int	ts_socket=0;
int	sync_flag, sync_count, val_offset, val_len, comm_type=0;
char	value[64];
float	freq;
struct	sockaddr_in	fdst_send;
struct	sockaddr	fdst_sendto;
int	send_sock, channel_no;
time_t	last_recv_tm;

ReportDisp	Gps_Report_Disp;
RTU_RECV	Rtu_Recv;
FMSG_OP		fmsgop;
//add for trans
struct		disp_struct	Iec_Disp;
//add end

/*------------------------------------ REPORT define ------------------------------------*/

#define		FREQUENCY_TYPE			0x80	//频率		遥测点号：0
#define		FREQ_DIFF_TYPE			0x81	//频差		遥测点号：1
#define		TIME_DIFF_TYPE			0x82	//时差		遥测点号：2
#define		ELECTRO_HOUR_TYPE		0x83	//电钟时	遥测点号：3
#define		ELECTRO_MINUTE_TYPE		0x84	//电钟分	遥测点号：4
#define		ELECTRO_SECOND_TYPE		0x85	//电钟秒	遥测点号：5
#define		DAY_TYPE			0x86	//天文钟日	遥测点号：6
#define		HOUR_TYPE			0x87	//天文钟时	遥测点号：7
#define		MINUTE_TYPE			0x88	//天文钟分	遥测点号：8
#define		SECOND_TYPE			0x89	//天文钟秒	遥测点号：9
#define		YEAR_TYPE			0x8a	//天文钟年	遥测点号：10
#define		MONTH_TYPE			0x8b	//天文钟月	遥测点号：11
#define		LAT_DEGREE_TYPE			0x8c	//纬度度	遥测点号：12
#define		LAT_MINUTE_TYPE			0x8d	//纬度分	遥测点号：13
#define		LONG_DEGREE_TYPE		0x8e	//经度度	遥测点号：14
#define		LONG_MINUTE_TYPE		0x8f	//经度分	遥测点号：15
#define		TIME_QUALITY_TYPE		0x90	//时间质量	遥测点号：16

/*****************************************************************************************/
extern void init_disp_str();
extern "C" void kill_process(int nouse);
extern "C" void do_nothing(int nouse);
void init_shmem( );
int  set_tty(int rtu_chan_num, int type);
void init_tcp(char *dname, int port);
void init_udp(char *dname, int port);
void init_send_sock( );

void gps_recv_proc(int chan_num, char ch);
void gps_recv_proc2(int chan_num, char *buff, int len);

//美国TrueTime天文钟 Model XL-DC-600, XL-DC-601 or XL-DC-602 Time and Frequency Receiver
void New_TrueTime_Gps_Interfase(int rtu_chan_num, char ch);
void New_TrueTime_Search_Sync_Head(char ch);
void New_TrueTime_Data0_Proc(int rtu_chan_num, char *buff);
void New_TrueTime_Data1_Proc(int rtu_chan_num, char *buff);
void New_TrueTime_Data2_Proc(int rtu_chan_num, char *buff);
void New_TrueTime_Data3_Proc(int rtu_chan_num, char *buff);
void New_TrueTime_Data4_Proc(int rtu_chan_num, char *buff);

void Old_TrueTime_Gps_Interfase(int rtu_chan_num, char ch);
int  Old_TrueTime_Gps_Data_Proc(int rtu_chan_num, unsigned char *buff);

//南京英奥科技有限公司SZ系列GPS标准时间同步钟
void NARI_Gps_Interfase(int rtu_chan_num, char ch);
void NARI_Gps_Search_Sync_Head(char ch);
int  NARI_Gps_Data1_Proc(int rtu_chan_num, unsigned char *buff);
void NARI_Gps_Data2_Proc(int rtu_chan_num, unsigned char *buff);

//淄博正瑞电子有限公司GPS2U型卫星时钟
void ZRM_Gps_Interfase(int rtu_chan_num, char ch);
void ZRM_Gps_Search_Sync_Head(char ch);
int  ZRM_Gps_Data1_Proc(int rtu_chan_num, unsigned char *buff);
void ZRM_Gps_Data2_Proc(int rtu_chan_num, unsigned char *buff);

//美国XLi天文钟网络口
void New_XLi_Gps_Net_Interfase(int rtu_chan_num, char ch);
void New_XLi_Gps_Net_Search_Sync_Head1(char ch);
void New_XLi_Gps_Net_Search_Sync_Head2(char ch);
void New_XLi_Gps_Net_Search_Sync_Head3(char ch);
int  New_XLi_Gps_Net_Data_Proc1(int rtu_chan_num, unsigned char *buff);
int  New_XLi_Gps_Net_Data_Proc2(int rtu_chan_num, unsigned char *buff);
int  New_XLi_Gps_Net_Data_Proc3(int rtu_chan_num, unsigned char *buff);
int  New_XLi_Gps_Net_Data_Proc4(int rtu_chan_num, unsigned char *buff);
int  New_XLi_Gps_Net_Data_Proc5(int rtu_chan_num, unsigned char *buff);

void Gps_Refresh_Data(int rtu_chan_num, int value1, float value2, int type);
int  IsValidNumber(unsigned char ch);
float GetFloat(unsigned char c_1, unsigned char c_2, unsigned char c_3, unsigned char c_4, unsigned char c_5);
int  BcdToInt(unsigned char ch);

//中国Vcom天文钟
int  Vcom_Gps_Interfase(int chan_num, char *buff, int len);
void Vcom_Gps_Interfase2(int rtu_chan_num, char ch);
void Vcom_Gps_Search_Sync_Head(char ch);
int  Vcom_Gps_Data1_Proc(int rtu_chan_num, unsigned char *buff);
int  Vcom_Gps_Data2_Proc(int rtu_chan_num, unsigned char *buff);
int  Vcom_Gps_Data3_Proc(int rtu_chan_num, unsigned char *buff);

//北斗天文钟
void BD_Gps_Interfase(int rtu_chan_num, char ch);
void BD_Gps_Search_Sync_Head(char ch);
void BD_Gps_Data1_Proc(int rtu_chan_num, unsigned char *buff);

//YJD-2000卫星时钟系统频率协议1 (协议号031)
int YJD_Gps_Interfase(int chan_num, char *buff, int len);

//NARI TSS系列GPS标准时间同步钟
void NARI_TSS_Gps_Interfase(int rtu_chan_num, char ch);
void NARI_TSS_Gps_Search_Sync_Head(char ch);
int  NARI_TSS_Gps_Data1_Proc(int rtu_chan_num, unsigned char *buff);

//成都可为天文钟
int  CDKW_Gps_Interface(int chan_num, char *buff, int len);
void CDKW_Disp_Report(char *buff,int len,int type,int i_value,float f_value);

/*****************************************************************************************/

void kill_process(int nouse)
{
char	buf[100];

	if( tty_fid>0 )
	{
printf("Close TTY,then exit!\n");
		close(tty_fid);
	}
	if( ts_socket>0 )
	{
printf("Close RECV sock!\n");
		shutdown(ts_socket, 2);
		close(ts_socket);
	}
	if( send_sock>0 )
	{
printf("Close send_sock,then exit!\n");
		close(send_sock);
	}

	*(unsigned short *)&buf[0]=channel_no;
	buf[2]=0x0e;
	fmsgop.SendMsg(ASSIGN_MSG, buf, 3);

	exit(0);
}

void do_nothing(int nouse)
{
}

void init_shmem( )
{
FSHM_OP		shm_op;

	SysManTabPtr = (SYSMAN_TABLE *)shm_op.OpenShmBuf( SYSMAN_KEY );
	if( SysManTabPtr == ((SYSMAN_TABLE *) -1) )
	{
		printf("protocol GPS: Open SYSMAN tab exit ! errno=%d\n", errno);
		exit( 1 );
	}
	FacTabPtr = (FAC_TABLE *)shm_op.OpenShmBuf( FAC_TABLE_KEY );
	if( FacTabPtr == ((FAC_TABLE *) -1) )
	{
		printf("protocol GPS: Open FAC tab exit ! errno=%d\n", errno);
		exit( 1 );
	}
	ChanTabPtr = (CHAN_TABLE *)shm_op.OpenShmBuf( CHAN_TABLE_KEY );
	if( ChanTabPtr == ((CHAN_TABLE *) -1) )
	{
		printf("protocol GPS: Open CHAN tab exit ! errno=%d\n", errno);
		exit( 1 );
	}
	ProtoTabPtr = (PROTO_TABLE *)shm_op.OpenShmBuf( PROTO_TABLE_KEY );
	if( ProtoTabPtr == ((PROTO_TABLE *) -1) )
	{
		printf("protocol GPS: Open PROTO PARA tab exit ! errno=%d\n", errno);
		exit( 1 );
	}
}

int set_tty(int rtu_chan_num, int type)
{
struct	termio	tc;
char	port_dev_name[100];

#ifdef	_ALPHA
	if( type==0 )
	{
		strcpy(port_dev_name, "/dev/tty00");
printf("seral prot == A\n");
	}
	else
	{
		strcpy(port_dev_name, "/dev/tty01");
printf("seral prot == B\n");
	}
#endif

#ifdef	_IBM
	if( type==0 )
	{
		strcpy(port_dev_name, "/dev/tty0");
printf("seral prot == A\n");
	}
	else
	{
		strcpy(port_dev_name, "/dev/tty1");
printf("seral prot == B\n");
	}
#endif

#ifdef	_SUN
	if( type==0 )
	{
		strcpy(port_dev_name, "/dev/ttya");
printf("seral prot == A\n");
	}
	else
	{
		strcpy(port_dev_name, "/dev/ttyb");
printf("seral prot == B\n");
	}
#endif

#ifdef	_HPUX
	if( type==0 )
	{
		strcpy(port_dev_name, "/dev/tty0p0");
printf("seral prot == A\n");
	}
	else
	{
		strcpy(port_dev_name, "/dev/tty0p2");
printf("seral prot == B\n");
	}
#endif

	tty_fid=open(port_dev_name, O_RDWR);

	if( tty_fid==-1 )
	{
		printf("can't open serial %s\n", port_dev_name);
		exit(-1);
	}

	ioctl(tty_fid, TCGETA, &tc);
	switch(ChanTabPtr->chan[rtu_chan_num].check_mode)
	{
		case 1:		//ODD
			tc.c_cflag |= ~PARENB;
			tc.c_cflag |= ~PARODD;
			break;
		case 2:		//EVEN
			tc.c_cflag |= ~PARENB;
			tc.c_cflag &= ~PARODD;
			break;
		default:	//NONE
			tc.c_cflag &= ~PARENB;
			break;
	}

	tc.c_cflag |= CS8;
	tc.c_cflag &= ~CBAUD;
	switch(ChanTabPtr->chan[rtu_chan_num].baud)
	{
		case 1:		//300
			tc.c_cflag |= B300;
			break;
		case 2:		//600
			tc.c_cflag |= B600;
			break;
		case 3:		//1200
			tc.c_cflag |= B1200;
			break;
		case 4:		//2400
			tc.c_cflag |= B2400;
			break;
		case 5:		//4800
			tc.c_cflag |= B4800;
			break;
		default:	//else 9600
			tc.c_cflag |= B9600;
			break;
	}

	switch(ChanTabPtr->chan[rtu_chan_num].stop_bit)
	{
		case 0:
			tc.c_cflag &= ~CSTOPB;
			break;
		default:	//1
			tc.c_cflag |= CSTOPB;
			break;
	}

	tc.c_oflag  = 0;
	tc.c_iflag  = 0;
	tc.c_lflag  = 0;
	tc.c_oflag &= ~OPOST;
	ioctl(tty_fid, TCSETA, &tc);

	return(1);
}

void init_tcp(char *dname, int port)
{
struct	sockaddr_in	peeraddr_in;
struct	sockaddr	peeraddr;
struct	hostent		*hp;

	memset((char *)&peeraddr_in, 0, sizeof peeraddr_in);
	ts_socket=socket(AF_INET, SOCK_STREAM, 0);
	if( ts_socket==-1 )
	{
		printf(" unable to create socket\n");
		exit(0);
	}

	printf("rtu hostname %s \n", dname);
	peeraddr_in.sin_addr.s_addr = inet_addr(dname);
	if( (int)peeraddr_in.sin_addr.s_addr == (-1) )
	{
		printf("err ip: %s \n", dname);
		hp = gethostbyname(dname);
		if( hp == NULL )
		{
			printf("rtu hostname %s not found in /etc/hosts\n",dname);
			close(ts_socket);
			exit(1);
		}
		else
		{
			peeraddr_in.sin_addr.s_addr =((struct in_addr *)(hp->h_addr))->s_addr;
		}
	}

	peeraddr_in.sin_family=AF_INET;
printf("port === %d \n",port);
	peeraddr_in.sin_port = htons(port);

	memcpy((char *)&peeraddr, (char *)&peeraddr_in, sizeof peeraddr_in);
	if( connect(ts_socket, (struct sockaddr *)&peeraddr, sizeof peeraddr)==-1 )
	{
		printf(" unable to connect to %s\n", dname);
 		printf("errno==%d\n",errno);
		close(ts_socket);
		exit(2);
    	}
	printf("hostaddr ======%d.%d.%d.%d port: %d \n"
			,(peeraddr_in.sin_addr.s_addr&0x000000ff)
			,((peeraddr_in.sin_addr.s_addr&0x0000ff00)>>8)
			,((peeraddr_in.sin_addr.s_addr&0x00ff0000)>>16)
			,((peeraddr_in.sin_addr.s_addr&0xff000000)>>24)
			,port);
}

void init_udp(char *dname, int port)
{
int	on;
struct	sockaddr_in	myaddr_in;
struct	protoent	*protos;

	memset((char *)&myaddr_in, 0, sizeof(myaddr_in));
	protos=getprotobyname("udp");
	ts_socket = socket(AF_INET, SOCK_DGRAM, protos->p_proto);
	if( ts_socket<0 )
	{
		printf( " unable to create ts_socket\n");
		exit(0);
	}

	myaddr_in.sin_family = AF_INET;
	myaddr_in.sin_addr.s_addr = INADDR_ANY;
	myaddr_in.sin_port = htons(port);
	on=1;
	setsockopt(ts_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
#ifndef _SUN
#ifndef	_LINUX
	setsockopt(ts_socket, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
#endif
#endif
	if( bind(ts_socket, (struct sockaddr *)&myaddr_in, sizeof(myaddr_in))<0 )
	{
		printf(" unable to bind to ts_socket\n" );
		close(ts_socket);
		exit(1);
	}
}

void init_send_sock( )
{
int	on;
char	name[32];
struct	hostent		*hp;
CSamIPService		sam_ip;

	send_sock=socket(AF_INET, SOCK_DGRAM, 0);
	if( send_sock<0 )
	{
		exit(0);
	}
	on=1;
	setsockopt(send_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
#ifndef _SUN
#ifndef	_LINUX
	setsockopt(send_sock, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
#endif
#endif

	strcpy(name, SysManTabPtr->fes_config[SysManTabPtr->fes_no].fes_name);
	hp=gethostbyname(name);
	if( hp==NULL )
	{
		close(send_sock);
		exit(1);
	}
	bcopy(hp->h_addr, &fdst_send.sin_addr, hp->h_length);
	if( sam_ip.GetBroadcastIP(fdst_send.sin_addr)<0 )
	{
		exit(2);
	}
	fdst_send.sin_family = AF_INET;
	fdst_send.sin_port = htons(2345);
printf("hostaddr hex===%x\n",fdst_send.sin_addr.s_addr);
	memcpy((char *)&fdst_sendto, (char *)&fdst_send, sizeof fdst_send);
}

/*****************************************************************************************/

int  main(int argc, char *argv[])
{
int	serial_type, remote_net_port, chan_num=-1, fac_num, ret;
char	ch, channel_name[64], temp_buff[1024], old[128];
fd_set	fdSet;
struct	timeval	tv;
#ifdef	_IBM
	socklen_t	ccc;
#else
	#ifdef	_HPUX
		socklen_t	ccc;
	#else
		#ifdef	_LINUX
			socklen_t	ccc;
		#else
			int	ccc;
		#endif
	#endif
#endif
struct	sockaddr_in	from;
time_t	now_time;

	char pot_path[128];
	sprintf(pot_path, "%s/translations", getenv("D5000_HOME"));
	loadtextlocale("fes_prot", pot_path);

	init_disp_str();

	if( (argc<3) || (argc>4) )
	{
		printf("Usage: fes_gps 0/1 channo\n");
		printf("or\n");
		printf("Usage: fes_gps IP_address channo remote_port\n");
		exit(-1);
	}

	if( argc==3 )
	{
		serial_type=atoi(argv[1]);
		if( (serial_type>1) || (serial_type<0) )
		{
			printf("Error serial type !!!\n");
			exit(-1);
		}
		chan_num=atoi(argv[2]);
		if( (chan_num>=MAX_CHAN_NUM) || (chan_num<0) )
		{
			printf("Error channel num !!!\n");
			exit(-2);
		}
		comm_type=1;
	}
	else if( argc==4 )
	{
		if( strlen(argv[1])>60 )
		{
			printf("Error IP address !!!\n");
			exit(-3);
		}
		strcpy(channel_name, argv[1]);
		printf("fes_gps : name=%s ...\n", channel_name);

		chan_num=atoi(argv[2]);
		if( chan_num>=MAX_CHAN_NUM || chan_num<0 )
		{
			printf("Error channel num !!!\n");
			exit(-4);
		}
		remote_net_port=atoi(argv[3]);
		printf("remote_net_port=%d \n", remote_net_port);
		comm_type=2;
	}

	signal(SIGINT,  kill_process);
	signal(SIGTERM, kill_process);
	signal(SIGQUIT, kill_process);
	signal(SIGKILL, kill_process);
	sigset(SIGHUP,  do_nothing);
	sigset(SIGALRM, do_nothing);
	sigset(SIGPIPE, do_nothing);

	channel_no=chan_num;
	init_shmem( );	//打开各共享内存，得到共享内存指针

	Gps_Report_Disp.set_app_no();
	Rtu_Recv.set_app_no();
	fmsgop.set_app_no();
	Fdb_io.set_app_no();
	Data.set_app_no();
	Result_Proc.set_app_no();

	if( ChanTabPtr->chan[chan_num].exist!=1 )
	{
		exit(-1);
	}
	fac_num=ChanTabPtr->chan[chan_num].comm_fac_no;
	if( fac_num<0 )
	{
		exit(-1);
	}
	if( comm_type==1 )
	{
		set_tty(chan_num, serial_type);
	}
	else if( comm_type==2 )
	{
		if( ChanTabPtr->chan[chan_num].net_type==NET_UDP )
			init_udp(channel_name, remote_net_port);
		else
			init_tcp(channel_name, remote_net_port);
	}
	else
	{
		printf("Error Communication Type !!!\n");
		exit(0);
	}
	init_send_sock( );
	sync_flag=-1;
	sync_count=0;
	val_offset=-1;
	val_len=0;
	freq=0.0;
	memset(old, 0, 128);
	time(&last_recv_tm);

	if( comm_type==1 )
	{
		ret=read(tty_fid, &temp_buff[0], 1000);
		while( ret>100 )
		{
			ret=read(tty_fid, &temp_buff[0], 1000);
		}
	}
	else
	{
		if( ChanTabPtr->chan[chan_num].net_type!=NET_UDP )
		{
			FD_ZERO(&fdSet);
			FD_SET(ts_socket, &fdSet);
			tv.tv_sec = 2;
			tv.tv_usec = 0;
			ret=select(ts_socket+1, &fdSet, 0, 0, &tv);
			if( ret<0 )
			{
				kill_process(0);
			}
			else if( ret>0 )
			{
				ret=recv(ts_socket, &temp_buff[0], 1000, 0);
				while( ret>100 )
				{
					ret=recv(ts_socket, &temp_buff[0], 1000, 0);
				}
			}
		}
	}
  int ppid;
  CheckInterval Check_Inter;
  struct timeval check_time;
  gettimeofday(&check_time, (struct timezone*)0);
	for(;;)
	{
		if( Check_Inter.Second(&check_time, 5)>0 )
		{
			ppid=getppid();
			if( ppid<=1 )
      {
       exit(0);
      }
		}
		ch=0;
		if( comm_type==1 )
		{
			ret=read(tty_fid, &ch, 1);
			if( ret<=0 )
				continue;
			gps_recv_proc(chan_num, ch);
		}
		else
		{
			if( ChanTabPtr->chan[chan_num].net_type==NET_UDP )
			{
				ccc=sizeof(from);
				ret=recvfrom(ts_socket, &temp_buff[0], 128, 0,
						(struct sockaddr *)&from, &ccc);
				if( ret==0 )
					continue;
				else if( ret<0 )
				{
					kill_process(0);
				}
				if( from.sin_addr.s_addr!=inet_addr(channel_name) )
					continue;
				temp_buff[ret]=0;
				if( strcmp(old, temp_buff)==0 )
					continue;
				strcpy(old, temp_buff);
				gps_recv_proc2(chan_num, &temp_buff[0], ret);
			}
			else
			{
				FD_ZERO(&fdSet);
				FD_SET(ts_socket, &fdSet);
				tv.tv_sec = 2;
				tv.tv_usec = 0;
				ret=select(ts_socket+1, &fdSet, 0, 0, &tv);
				if( ret==0 )
				{
					time(&now_time);
					if( (now_time-last_recv_tm)>30 )
						kill_process(0);
					continue;
				}
				else if( ret<0 )
				{
					kill_process(0);
				}
				ret=recv(ts_socket, &ch, 1, 0);
				if( ret<0 )
				{
					usleep(50*1000);
					continue;
				}
				else if( ret==0 )
				{
					printf("recv len==0, socket not exist! \n");
					kill_process(0);
				}
				time(&last_recv_tm);
				gps_recv_proc(chan_num, ch);
			}
		}
	}
}

void gps_recv_proc(int chan_num, char ch)
{
int	gps_type;

printf("ch===%02x\n",ch);
	Rtu_Recv.Write_Recv_Buffer(chan_num, (unsigned char *)&ch, 1);
	gps_type=ChanTabPtr->chan[chan_num].proto_type;
	switch( gps_type )
	{
		case	200:
			New_TrueTime_Gps_Interfase(chan_num, ch);
			break;
		case	201:
			Old_TrueTime_Gps_Interfase(chan_num, ch);
			break;
		case	202:
			NARI_Gps_Interfase(chan_num, ch);
			break;
		case	203:
			ZRM_Gps_Interfase(chan_num, ch);
			break;
		case	204:
			New_XLi_Gps_Net_Interfase(chan_num, ch);
			break;
		case	205:
			Vcom_Gps_Interfase2(chan_num, ch);
			break;
		case	206:
			BD_Gps_Interfase(chan_num, ch);
			break;
		case	208:
			NARI_TSS_Gps_Interfase(chan_num, ch);
		default:
			break;
	}
}

void gps_recv_proc2(int chan_num, char *buff, int len)
{
int	gps_type;

	Rtu_Recv.Write_Recv_Buffer(chan_num, (unsigned char *)&buff[0], len);
	gps_type=ChanTabPtr->chan[chan_num].proto_type;
	switch( gps_type )
	{
		case	205:
			Vcom_Gps_Interfase(chan_num, buff, len);
			break;
		case	207:
			YJD_Gps_Interfase(chan_num, buff, len);
			break;
		case	209:
			CDKW_Gps_Interface(chan_num, buff, len);
			break;
		default:
			break;
	}
}

//************************************ 美国TrueTime天文钟 ************************************

void New_TrueTime_Gps_Interfase(int rtu_chan_num, char ch)
{
	if( ch<0x0a )
		return;
	if( ch==0x0d )
	{
		sync_count=0;
		sync_flag=-1;
	}
	if( sync_flag<=0 )
	{
		New_TrueTime_Search_Sync_Head(ch);
		val_len=0;
		return;
	}
	value[val_len]=ch;
	val_len++;
	switch(val_offset)
	{
		case 0:
			if( val_len>=6 )
			{
				New_TrueTime_Data0_Proc(rtu_chan_num, &value[0]);
				Gps_Report_Disp.DispStringWithType(rtu_chan_num, &value[0], Disp_Recv_Type, 6, (char)0);
				sync_flag=-1;
				sync_count=0;
				val_offset=-1;
				val_len=0;
			}
			break;
		case 1:
			if( val_len>=6 )
			{
				New_TrueTime_Data1_Proc(rtu_chan_num, &value[0]);
				Gps_Report_Disp.DispStringWithType(rtu_chan_num, &value[0], Disp_Recv_Type, 6, (char)0);
				sync_flag=-1;
				sync_count=0;
				val_offset=-1;
				val_len=0;
			}
			break;
		case 2:
			if( val_len>=7 )
			{
				New_TrueTime_Data2_Proc(rtu_chan_num, &value[0]);
				Gps_Report_Disp.DispStringWithType(rtu_chan_num, &value[0], Disp_Recv_Type, 7, (char)0);
				sync_flag=-1;
				sync_count=0;
				val_offset=-1;
				val_len=0;
			}
			break;
		case 3:
			if( val_len>=12 )
			{
				New_TrueTime_Data3_Proc(rtu_chan_num, &value[0]);
				Gps_Report_Disp.DispStringWithType(rtu_chan_num, &value[0], Disp_Recv_Type, 12, (char)0);
				sync_flag=-1;
				sync_count=0;
				val_offset=-1;
				val_len=0;
			}
			break;
		case 4:
			if( val_len>=13 )
			{
				New_TrueTime_Data4_Proc(rtu_chan_num, &value[0]);
				Gps_Report_Disp.DispStringWithType(rtu_chan_num, &value[0], Disp_Recv_Type, 12, (char)0);
				sync_flag=-1;
				sync_count=0;
				val_offset=-1;
				val_len=0;
			}
			break;
		default:
			sync_flag=-1;
			sync_count=0;
			val_offset=-1;
			val_len=0;
			break;
	}
}

void New_TrueTime_Search_Sync_Head(char ch)
{
	if( sync_count==0 )
	{
		if( ch==0x0d )
		{
			sync_count=1;
			return;
		}
	}
	else if( sync_count==1 )
	{
		if( ch==0x0a )
		{
			sync_count=2;
			return;
		}
		else if( ch==0x0d )
		{
			sync_count=1;
			return;
		}
		else
		{
			sync_count=0;
			return;
		}
	}
	else if( sync_count==2 )
	{
		if( ch=='0' )
		{
			sync_count=3;
			return;
		}
		else if( ch==0x0d )
		{
			sync_count=1;
			return;
		}
		else
		{
			sync_count=0;
			return;
		}
	}
	else if( sync_count==3 )
	{
		if( ch=='2' )
		{
			sync_count=4;
			return;
		}
		else if( ch==0x0d )
		{
			sync_count=1;
			return;
		}
		else
		{
			sync_count=0;
			return;
		}
	}
	else if( sync_count==4 )
	{
		if( (ch>='0')&&(ch<='4') )
		{
			val_offset=ch-'0';
			sync_count=0;
			sync_flag=1;
			return;
		}
		else if( ch==0x0d )
		{
			sync_count=1;
			sync_flag=-1;
			val_offset=-1;
			return;
		}
		else
		{
			sync_count=0;
			sync_flag=-1;
			val_offset=-1;
			return;
		}
	}
}

void New_TrueTime_Data0_Proc(int rtu_chan_num, char *buff)
{
int	yc_int;
float	f_val;
char	c1, c2, c3, c4, c5;

	c1 = buff[0] - 0x30;
	if( c1==-16 )	c1=0;
	c2 = buff[1] - 0x30;
	if( c2==-16 )	c2=0;
	c3 = buff[3] - 0x30;
	if( c3==-16 )	c3=0;
	c4 = buff[4] - 0x30;
	if( c4==-16 )	c4=0;
	c5 = buff[5] - 0x30;
	if( c5==-16 )	c5=0;

	f_val = (c1*10000 + c2*1000 + c3*100 +c4*10 + c5)/1000.0;
//printf("!!!!!!!!!!!!!!!!!!!! 频率 : %2.3fHz\n",f_val);

	yc_int=(int)f_val;
	Gps_Refresh_Data(rtu_chan_num, yc_int, f_val, FREQUENCY_TYPE);

	if( (f_val<40.0) || (f_val>60.0) )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_error+=30;
	}
	else if( (fabs(f_val-freq))>0.0005 )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_total+=2;
	}
	freq=f_val;
}

void New_TrueTime_Data1_Proc(int rtu_chan_num, char *buff)
{
int	yc_int;
float	f_val;
char	c1, c2, c3, c4;

	c1 = buff[1] - 0x30;
	if( c1==-16 )	c1=0;
	c2 = buff[3] - 0x30;
	if( c2==-16 )	c2=0;
	c3 = buff[4] - 0x30;
	if( c3==-16 )	c3=0;
	c4 = buff[5] - 0x30;
	if( c4==-16 )	c4=0;

	f_val = (c1*1000 + c2*100 + c3*10 + c4)/1000.0;
	if( buff[0]=='-' )
		f_val *= (-1.0);
//printf("!!!!!!!!!!!!!!!!!!!! 频差 : %2.3fHz\n",f_val);

	yc_int=(int)f_val;
	Gps_Refresh_Data(rtu_chan_num, yc_int, f_val, FREQ_DIFF_TYPE);
}

void New_TrueTime_Data2_Proc(int rtu_chan_num, char *buff)
{
int	yc_int;
float	f_val;
char	c1, c2, c3, c4, c5, c6;

	c1 = buff[1] - 0x30;
	if( c1==-16 )	c1=0;
	c2 = buff[2] - 0x30;
	if( c2==-16 )	c2=0;
	c3 = buff[3] - 0x30;
	if( c3==-16 )	c3=0;
	c4 = buff[4] - 0x30;
	if( c4==-16 )	c4=0;
	c5 = buff[5] - 0x30;
	if( c5==-16 )	c5=0;
	c6 = buff[6] - 0x30;
	if( c6==-16 )	c6=0;

	f_val = (c1*10000 + c2*1000 + c4*100 +c5*10 + c6)/1000.0;
	if( c2==-2 )
	{
		f_val = (c1*1000 + c3*100 + c4*10 +c5)/1000.0;
	}
	if( buff[0]=='-' )
		f_val *= (-1.0);
//printf("!!!!!!!!!!!!!!!!!!! 时差 : %3.3f秒\n",f_val);

	yc_int=(int)f_val;
	Gps_Refresh_Data(rtu_chan_num, yc_int, f_val, TIME_DIFF_TYPE);
}

void New_TrueTime_Data3_Proc(int rtu_chan_num, char *buff)
{
int	hour, min, yc_int;
float	f_val, yc_flo;
char	c1, c2, c3, c4, c5;

	c1 = buff[0] - 0x30;
	c2 = buff[1] - 0x30;
	hour = c1*10 + c2;

	c1 = buff[3] - 0x30;
	c2 = buff[4] - 0x30;
	min = c1*10 + c2;

	c1 = buff[6] - 0x30;
	c2 = buff[7] - 0x30;
	c3 = buff[9] - 0x30;
	c4 = buff[10] - 0x30;
	c5 = buff[11] - 0x30;

	f_val = (c1*10000 + c2*1000 + c3*100 +c4*10 + c5)/1000.0;
//printf("!!!!!!!!!!!!!! 电钟 : %d时%d分%2.3f秒\n",hour,min,f_val);

	yc_flo=(float)hour;
	Gps_Refresh_Data(rtu_chan_num, hour, yc_flo, ELECTRO_HOUR_TYPE);

	yc_flo=(float)min;
	Gps_Refresh_Data(rtu_chan_num, min, yc_flo, ELECTRO_MINUTE_TYPE);

	yc_int=(int)f_val;
	Gps_Refresh_Data(rtu_chan_num, yc_int, f_val, ELECTRO_SECOND_TYPE);
}

void New_TrueTime_Data4_Proc(int rtu_chan_num, char *buff)
{
int		day, hour, min, sec, yc_int, ret;
float		yc_flo;
unsigned char	c_1, c_2, c_3, duty;
struct	tm	*phytime_t;
time_t		gpstime, machtime;
int		net_gps_time;

	c_1 = buff[0] - 0x30;
	c_2 = buff[1] - 0x30;
	c_3 = buff[2] - 0x30;
	day = c_1*100 + c_2*10 + c_3;

	c_1 = buff[4] - 0x30;
	c_2 = buff[5] - 0x30;
	hour = c_1*10 + c_2;

	c_1 = buff[7] - 0x30;
	c_2 = buff[8] - 0x30;
	min = c_1*10 + c_2;

	c_1 = buff[10] - 0x30;
	c_2 = buff[11] - 0x30;
	sec = c_1*10 + c_2;

//printf("!!!!!!!!!!!!!! 时间 : %d天%d时%d分%d秒\n",day,hour,min,sec);

	yc_flo=(float)day;
	Gps_Refresh_Data(rtu_chan_num, day, yc_flo, DAY_TYPE);

	yc_flo=(float)hour;
	Gps_Refresh_Data(rtu_chan_num, hour, yc_flo, HOUR_TYPE);

	yc_flo=(float)min;
	Gps_Refresh_Data(rtu_chan_num, min, yc_flo, MINUTE_TYPE);

	yc_flo=(float)sec;
	Gps_Refresh_Data(rtu_chan_num, sec, yc_flo, SECOND_TYPE);

	switch( buff[12] )
	{
		case	' ':
			yc_int=0;
			yc_flo=0.0;
			break;
		case	'.':
			yc_int=1;
			yc_flo=1.0;
			break;
		case	'*':
			yc_int=2;
			yc_flo=2.0;
			break;
		case	'#':
			yc_int=3;
			yc_flo=3.0;
			break;
		default:
			yc_int=4;
			yc_flo=4.0;
			break;
	}
	Gps_Refresh_Data(rtu_chan_num, yc_int, yc_flo, TIME_QUALITY_TYPE);

	if( buff[12]==' ' )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_total+=3;
	}
	else
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_error+=3;
		return;
	}
	duty=SysManTabPtr->fcheck[rtu_chan_num].if_duty;
	duty &= 0x07;
	if( duty!=DUTY )
	{
		return;
	}

	time(&machtime);
	phytime_t=localtime(&machtime);
	if( day!=(phytime_t->tm_yday+1) )
	{
		return;
	}
	phytime_t->tm_hour	= hour;
	phytime_t->tm_min	= min;
	phytime_t->tm_sec	= sec;
	gpstime=mktime(phytime_t);
	net_gps_time = htonl(gpstime);

	ret=sendto(send_sock, (char *)&net_gps_time, sizeof(net_gps_time), 0,
			(struct sockaddr *)&fdst_sendto, sizeof(fdst_sendto));
printf("!!!!!!!!!!!!!!!!!!! sendto===%d\n",ret);
}

//------------------------------------------------------------------------------------------------

void Old_TrueTime_Gps_Interfase(int rtu_chan_num, char ch)
{
	if( ch<0x0a )
		return;
	if( ch==0x0a )
	{
		sync_flag=-1;
		sync_count=0;
		val_offset=-1;
		val_len=0;
		return;
	}
	value[val_len]=ch;
	val_len++;
	if( val_len<27 )
		return;
	Old_TrueTime_Gps_Data_Proc(rtu_chan_num, (unsigned char *)&value[0]);
	Gps_Report_Disp.DispStringWithType(rtu_chan_num, &value[0], Disp_Recv_Type, 27, (char)0);
	sync_flag=-1;
	sync_count=0;
	val_offset=-1;
	val_len=0;
}

int Old_TrueTime_Gps_Data_Proc(int rtu_chan_num, unsigned char *buff)
{
int		day, hour, min, sec, yc_int, ret;
unsigned char	duty, c_1, c_2, c_3;
float		pc_value, zb_value, yc_flo;
struct	tm	*phytime_t;
time_t		gpstime, machtime;
int		net_gps_time;

	if( !(IsValidNumber(buff[0]) && IsValidNumber(buff[1])
	   && IsValidNumber(buff[2]) && IsValidNumber(buff[4])
	   && IsValidNumber(buff[5]) && IsValidNumber(buff[7])
	   && IsValidNumber(buff[8]) && IsValidNumber(buff[10])
	   && IsValidNumber(buff[11])) )
	{
		return(-1);
	}

	if( (buff[3]!=':') || (buff[6]!=':') || (buff[9]!=':') )
	{
		printf("wrong format, should be : \n");
		return(-2);
	}
	if( (buff[13]!='T') && (buff[13]!='t') )
	{
		printf("wrong format, should be T/t\n");
		return(-3);
	}
	if( (buff[20]!='F') && (buff[20]!='f') )
	{
		printf("wrong format, should be F/f \n");
		return(-4);
	}

	c_1 = buff[0] - 0x30;
	c_2 = buff[1] - 0x30;
	c_3 = buff[2] - 0x30;
	day = c_1*100 + c_2*10 + c_3;

	c_1 = buff[4] - 0x30;
	c_2 = buff[5] - 0x30;
	hour = c_1*10 + c_2;

	c_1 = buff[7] - 0x30;
	c_2 = buff[8] - 0x30;
	min = c_1*10 + c_2;

	c_1 = buff[10] - 0x30;
	c_2 = buff[11] - 0x30;
	sec = c_1*10 + c_2;

	yc_flo=(float)day;
	Gps_Refresh_Data(rtu_chan_num, day, yc_flo, DAY_TYPE);
	yc_flo=(float)hour;
	Gps_Refresh_Data(rtu_chan_num, hour, yc_flo, HOUR_TYPE);
	yc_flo=(float)min;
	Gps_Refresh_Data(rtu_chan_num, min, yc_flo, MINUTE_TYPE);
	yc_flo=(float)sec;
	Gps_Refresh_Data(rtu_chan_num, sec, yc_flo, SECOND_TYPE);

	switch( buff[12] )
	{
		case	' ':
			yc_int=0;
			yc_flo=0.0;
			break;
		case	'.':
			yc_int=1;
			yc_flo=1.0;
			break;
		case	'*':
			yc_int=2;
			yc_flo=2.0;
			break;
		case	'#':
			yc_int=3;
			yc_flo=3.0;
			break;
		default:
			yc_int=4;
			yc_flo=4.0;
			break;
	}
	Gps_Refresh_Data(rtu_chan_num, yc_int, yc_flo, TIME_QUALITY_TYPE);

	pc_value = GetFloat(buff[15], buff[16], buff[17], buff[18], buff[19]);
	if( buff[14] == '-' )
		pc_value = 0 - pc_value;
	yc_int=(int)pc_value;
	Gps_Refresh_Data(rtu_chan_num, yc_int, pc_value, TIME_DIFF_TYPE);

	zb_value = GetFloat(buff[22], buff[23], buff[24], buff[25], buff[26]);
	if( buff[21] == '-' )
		zb_value = 0 - zb_value;
	yc_int=(int)zb_value;
	Gps_Refresh_Data(rtu_chan_num, yc_int, zb_value, FREQ_DIFF_TYPE);

	zb_value += 50.0;
	yc_int=(int)zb_value;
	Gps_Refresh_Data(rtu_chan_num, yc_int, zb_value, FREQUENCY_TYPE);

	if( (zb_value<40.0) || (zb_value>60.0) )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_error+=30;
	}
	else if( (fabs(zb_value-freq))>0.0005 )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_total+=2;
	}
	freq=zb_value;

	if( buff[12]==' ' )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_total+=3;
	}
	else
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_error+=3;
		return(3);
	}

	duty = 0;
	duty=SysManTabPtr->fcheck[rtu_chan_num].if_duty;
	duty &= 0x07;
	if( duty!=DUTY )
	{
		return(4);
	}

	time(&machtime);
	phytime_t=localtime(&machtime);
	if( day!=(phytime_t->tm_yday+1) )
	{
		return(5);
	}
	phytime_t->tm_hour	= hour;
	phytime_t->tm_min	= min;
	phytime_t->tm_sec	= sec;
	gpstime=mktime(phytime_t);
	net_gps_time = htonl(gpstime);

	ret=sendto(send_sock, (char *)&net_gps_time, sizeof(net_gps_time), 0,
			(struct sockaddr *)&fdst_sendto, sizeof(fdst_sendto));
printf("!!!!!!!!!!!!!!!!!!! sendto===%d\n",ret);

	return(0);
}

//************************ 南京英奥科技有限公司SZ系列GPS标准时间同步钟 ************************

void NARI_Gps_Interfase(int rtu_chan_num, char ch)
{
	if( ch==0x0d )
	{
		sync_count=0;
		sync_flag=-1;
		val_len=0;
	}
	if( sync_flag<=0 )
	{
		NARI_Gps_Search_Sync_Head(ch);
		return;
	}
	value[val_len]=ch;
	if( (value[0]!=0x42) || ((val_len==7)&&(value[7]!=0x46)) )
	{
		sync_flag=-1;
		sync_count=0;
		val_len=0;
		return;
	}
	val_len++;

	if( val_len==11 )
	{
		NARI_Gps_Data1_Proc(rtu_chan_num, (unsigned char *)&value[0]);
	}

	if( val_len<15 )
		return;
	NARI_Gps_Data2_Proc(rtu_chan_num, (unsigned char *)&value[11]);
	Gps_Report_Disp.DispAllWithType(rtu_chan_num, (unsigned	char*)&value[0], Disp_Recv_Type, val_len, (char)0);

	sync_flag=-1;
	sync_count=0;
	val_len=0;
}

void NARI_Gps_Search_Sync_Head(char ch)
{
	if( sync_count==0 )
	{
		if( ch==0x0d )
		{
			sync_count=1;
			val_len=0;
			return;
		}
	}
	else if( sync_count==1 )
	{
		if( ch=='B' )
		{
			sync_count=0;
			sync_flag=1;
			value[0]=ch;
			val_len=1;
			return;
		}
		else if( ch==0x0d )
		{
			sync_count=1;
			val_len=1;
			return;
		}
		else
		{
			sync_count=0;
			val_len=0;
			return;
		}
	}
}

int NARI_Gps_Data1_Proc(int rtu_chan_num, unsigned char *buff)
{
unsigned char	duty;
int 		hour, min, sec, year, month, day, sum, yc_int, retcode;
float		zb_value=0.0, yc_float=0.0;
struct	tm	*phytime_t;
time_t		gpstime, machtime;
int		net_gps_time;
/*
	hour  = BcdToInt(buff[1]);
	yc_float=(float)hour;
	Gps_Refresh_Data(rtu_chan_num, hour, yc_float, HOUR_TYPE);

	min   = BcdToInt(buff[2]);
	yc_float=(float)min;
	Gps_Refresh_Data(rtu_chan_num, min, yc_float, MINUTE_TYPE);

	sec   = BcdToInt(buff[3]);
	yc_float=(float)sec;
	Gps_Refresh_Data(rtu_chan_num, sec, yc_float, SECOND_TYPE);

	year  = BcdToInt(buff[4]);
	yc_float=(float)year;
	Gps_Refresh_Data(rtu_chan_num, year, yc_float, YEAR_TYPE);

	month = BcdToInt(buff[5]);
	yc_float=(float)month;
	Gps_Refresh_Data(rtu_chan_num, month, yc_float, MONTH_TYPE);

	day   = BcdToInt(buff[6]);
	yc_float=(float)day;
	Gps_Refresh_Data(rtu_chan_num, day, yc_float, DAY_TYPE);

	yc_int=BcdToInt(buff[9]);
	if( yc_int<0 )
		return(-1);
	sum=yc_int*100;
	yc_int=BcdToInt(buff[10]);
	if( yc_int<0 )
		return(-2);
	sum+=yc_int;
	yc_int=BcdToInt(buff[8]);
	if( yc_int<0 )
		return(-3);
	zb_value=(float)yc_int+(float)sum/10000.0;
i*/


year  =  (buff[5]-0x30)*1000+ (buff[6]-0x30)*100+(buff[7]-0x30)*10+(buff[8]-0x30);
        yc_float=(float)year;
        Gps_Refresh_Data(rtu_chan_num, year, yc_float, YEAR_TYPE);

        month =  (buff[9]-0x30)*10+ (buff[10]-0x30);
        yc_float=(float)month;
        Gps_Refresh_Data(rtu_chan_num, month, yc_float, MONTH_TYPE);

        day   = (buff[11]-0x30)*10+ (buff[12]-0x30);
        yc_float=(float)day;
        Gps_Refresh_Data(rtu_chan_num, day, yc_float, DAY_TYPE);

        hour  = (buff[13]-0x30)*10+ (buff[14]-0x30);
        yc_float=(float)hour;
        Gps_Refresh_Data(rtu_chan_num, hour, yc_float, HOUR_TYPE);
        
        min   = (buff[15]-0x30)*10+ (buff[16]-0x30);
        yc_float=(float)min;
        Gps_Refresh_Data(rtu_chan_num, min, yc_float, MINUTE_TYPE);
        
        sec   = (buff[17]-0x30)*10+ (buff[18]-0x30);
        yc_float=(float)sec;
        Gps_Refresh_Data(rtu_chan_num, sec, yc_float, SECOND_TYPE);

        yc_int=(buff[20]-0x30)*10+ (buff[21]-0x30);
        if( yc_int<0 )
                yc_int=0;
        sum=(buff[22]-0x30)*1000+ (buff[23]-0x30)*100+(buff[24]-0x30)*10+(buff[25]-0x30);
        zb_value=(float)yc_int+(float)sum/10000.0;


printf(_("ldl!!!!!!!!!!!!!! 时间 : %d年%d月%d日%d时%d分%d秒\n"),year,month,day,hour,min,sec);

//周波处理
	yc_int=(int)zb_value;
	Gps_Refresh_Data(rtu_chan_num, yc_int, zb_value, FREQUENCY_TYPE);
printf(_("!!!!!!!!!!!!!! 系统周波 :  %f \n"),zb_value);

	yc_float=zb_value-50.0;
	yc_int=(int)yc_float;
	Gps_Refresh_Data(rtu_chan_num, yc_int, yc_float, FREQ_DIFF_TYPE);

	if( (zb_value<40.0) || (zb_value>60.0) )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_error++;
	}
	else if( (fabs(zb_value-freq))>0.0005 )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_total++;
	}
	freq=zb_value;

	duty=SysManTabPtr->fcheck[rtu_chan_num].if_duty;
	duty &= 0x07;
	if( duty!=DUTY )
	{
		return(3);
	}

	if( (year<0) || (month<0) || (day<0) || (hour<0) || (min<0) || (sec<0) )
		return(-4);

	time(&machtime);
	phytime_t=localtime(&machtime);
	phytime_t->tm_year	= year+100;
	phytime_t->tm_mon	= month-1;
	phytime_t->tm_mday	= day;
	phytime_t->tm_hour	= hour;
	phytime_t->tm_min	= min;
	phytime_t->tm_sec	= sec;
	gpstime=mktime(phytime_t);
	net_gps_time = htonl(gpstime);

	retcode=sendto(send_sock, (char *)&net_gps_time, sizeof(net_gps_time), 0,
			(struct sockaddr *)&fdst_sendto, sizeof(fdst_sendto));
printf("!!!!!!!!!!!!!!!!!!! NARI GPS sendto===%d\n",retcode);

	return(retcode);
}

void NARI_Gps_Data2_Proc(int rtu_chan_num, unsigned char *buff)
{
int 	hour, min, sec;
float	yc_flo;

	hour = BcdToInt(buff[1]);
	min  = BcdToInt(buff[2]);
	sec  = BcdToInt(buff[3]);

	yc_flo=(float)hour;
	Gps_Refresh_Data(rtu_chan_num, hour, yc_flo, ELECTRO_HOUR_TYPE);
	yc_flo=(float)min;
	Gps_Refresh_Data(rtu_chan_num, min, yc_flo, ELECTRO_MINUTE_TYPE);
	yc_flo=(float)sec;
	Gps_Refresh_Data(rtu_chan_num, sec, yc_flo, ELECTRO_SECOND_TYPE);
printf(_("!!!!!!!!!!!!!! 周波钟 : %d时%d分%d秒\n"),hour,min,sec);
}

//**************************** 淄博正瑞电子有限公司GPS2U型卫星时钟 ****************************

void ZRM_Gps_Interfase(int rtu_chan_num, char ch)
{
	if( ch==0x0d )
	{
		sync_count=0;
		sync_flag=-1;
		val_len=0;
	}
	if( sync_flag<=0 )
	{
		ZRM_Gps_Search_Sync_Head(ch);
		return;
	}
	if( (val_len<=13) && !(IsValidNumber((unsigned char)ch)) )
	{
		sync_flag=-1;
		sync_count=0;
		val_len=0;
		return;
	}
	value[val_len]=ch;
	if( (val_len==15) && (value[15]!=',') )
	{
		sync_flag=-1;
		sync_count=0;
		val_len=0;
		return;
	}
	val_len++;

	if( val_len==16)
	{
		ZRM_Gps_Data1_Proc(rtu_chan_num, (unsigned char *)&value[0]);
	}

	if( val_len<38 )
		return;
	Gps_Report_Disp.DispStringWithType(rtu_chan_num, &value[0], Disp_Recv_Type, val_len, (char)0);
	ZRM_Gps_Data2_Proc(rtu_chan_num, (unsigned char *)&value[16]);

	sync_flag=-1;
	sync_count=0;
	val_len=0;
}

void ZRM_Gps_Search_Sync_Head(char ch)
{
	val_len=0;
	if( sync_count==0 )
	{
		sync_flag=-1;
		if( ch==0x0d )
		{
			sync_count=1;
			return;
		}
	}
	else if( sync_count==1 )
	{
		sync_flag=-1;
		if( ch==0x0a )
		{
			sync_count=2;
			return;
		}
		else if( ch==0x0d )
		{
			sync_count=1;
			return;
		}
		else
		{
			sync_count=0;
			return;
		}
	}
	else if( sync_count==2 )
	{
		if( ch==0x01 )
		{
			sync_count=0;
			sync_flag=1;
			return;
		}
		else if( ch==0x0d )
		{
			sync_count=1;
			return;
		}
		else if( ch==0x0a )
		{
			sync_count=2;
			return;
		}
		else
		{
			sync_count=0;
			return;
		}
	}
}

int ZRM_Gps_Data1_Proc(int rtu_chan_num, unsigned char *buff)
{
int		year, month, day, hour, min, sec, ret;
float		yc_flo;
unsigned char	c_1, c_2, c_3, c_4, duty;
struct	tm	*phytime_t;
time_t		gpstime, machtime;
int		net_gps_time;

	c_1 = buff[0] - 0x30;
	c_2 = buff[1] - 0x30;
	c_3 = buff[2] - 0x30;
	c_4 = buff[3] - 0x30;
	year = c_1*1000 + c_2*100 + c_3*10 + c_4;

	c_1 = buff[4] - 0x30;
	c_2 = buff[5] - 0x30;
	month = c_1*10 + c_2;

	c_1 = buff[6] - 0x30;
	c_2 = buff[7] - 0x30;
	day = c_1*10 + c_2;

	c_1 = buff[8] - 0x30;
	c_2 = buff[9] - 0x30;
	hour = c_1*10 + c_2;

	c_1 = buff[10] - 0x30;
	c_2 = buff[11] - 0x30;
	min = c_1*10 + c_2;

	c_1 = buff[12] - 0x30;
	c_2 = buff[13] - 0x30;
	sec = c_1*10 + c_2;

	yc_flo=(float)year;
	Gps_Refresh_Data(rtu_chan_num, year, yc_flo, YEAR_TYPE);
	yc_flo=(float)month;
	Gps_Refresh_Data(rtu_chan_num, month, yc_flo, MONTH_TYPE);
	yc_flo=(float)day;
	Gps_Refresh_Data(rtu_chan_num, day, yc_flo, DAY_TYPE);
	yc_flo=(float)hour;
	Gps_Refresh_Data(rtu_chan_num, hour, yc_flo, HOUR_TYPE);
	yc_flo=(float)min;
	Gps_Refresh_Data(rtu_chan_num, min, yc_flo, MINUTE_TYPE);
	yc_flo=(float)sec;
	Gps_Refresh_Data(rtu_chan_num, sec, yc_flo, SECOND_TYPE);

	if( buff[14]!='A' )
	{
		return(1);
	}
	SysManTabPtr->fcheck[rtu_chan_num].current_total++;

	duty=SysManTabPtr->fcheck[rtu_chan_num].if_duty;
	duty &= 0x07;
	if( duty!=DUTY )
	{
		return(-2);
	}

	if( (hour==0) && (min==0) && (sec==0) )
		return(-3);

	time(&machtime);
	phytime_t=localtime(&machtime);
	phytime_t->tm_hour	= hour;
	phytime_t->tm_min	= min;
	phytime_t->tm_sec	= sec;
	gpstime=mktime(phytime_t);
	net_gps_time = htonl(gpstime);

	ret=sendto(send_sock, (char *)&net_gps_time, sizeof(net_gps_time), 0,
			(struct sockaddr *)&fdst_sendto, sizeof(fdst_sendto));
printf("!!!!!!!!!!!!!!!!!!! ZRM GPS sendto===%d\n",ret);

	return(ret);
}

void ZRM_Gps_Data2_Proc(int rtu_chan_num, unsigned char *buff)
{
int		yc_int, yc_int2;
float		yc_flo;
unsigned char	c_1, c_2, c_3, c_4;

	c_1 = buff[1] - 0x30;
	c_2 = buff[2] - 0x30;
	yc_int = c_1*10 + c_2;
	yc_flo=(float)yc_int;
	Gps_Refresh_Data(rtu_chan_num, yc_int, yc_flo, LAT_DEGREE_TYPE);

	c_1 = buff[3] - 0x30;
	c_2 = buff[4] - 0x30;
	yc_int = c_1*10 + c_2;
	c_1 = buff[6] - 0x30;
	c_2 = buff[7] - 0x30;
	c_3 = buff[8] - 0x30;
	c_4 = buff[9] - 0x30;
	yc_int2 = c_1*1000 + c_2*100 + c_3*10 + c_4;
	yc_flo=(float)yc_int+(float)(yc_int2)/10000.0;
	Gps_Refresh_Data(rtu_chan_num, yc_int, yc_flo, LAT_MINUTE_TYPE);

	c_1 = buff[12] - 0x30;
	c_2 = buff[13] - 0x30;
	c_3 = buff[14] - 0x30;
	yc_int = c_1*100 + c_2*10 + c_3;
	yc_flo=(float)yc_int;
	Gps_Refresh_Data(rtu_chan_num, yc_int, yc_flo, LONG_DEGREE_TYPE);

	c_1 = buff[15] - 0x30;
	c_2 = buff[16] - 0x30;
	yc_int = c_1*10 + c_2;
	c_1 = buff[18] - 0x30;
	c_2 = buff[19] - 0x30;
	c_3 = buff[20] - 0x30;
	c_4 = buff[21] - 0x30;
	yc_int2 = c_1*1000 + c_2*100 + c_3*10 + c_4;
	yc_flo=(float)yc_int+(float)(yc_int2)/10000.0;
	Gps_Refresh_Data(rtu_chan_num, yc_int, yc_flo, LONG_MINUTE_TYPE);
}

//************************************ 美国XLi天文钟网络口 ************************************

void New_XLi_Gps_Net_Interfase(int rtu_chan_num, char ch)
{
char	temp_buff[64];

	if( sync_flag==-1 )
	{
		New_XLi_Gps_Net_Search_Sync_Head1(ch);
	}
	if( sync_flag==0 )
	{
		strcpy(temp_buff, "operator\n");
		send(ts_socket, temp_buff, strlen(temp_buff), 0);
		sync_flag=1;
		return;
	}
	if( sync_flag==1 )
	{
		New_XLi_Gps_Net_Search_Sync_Head2(ch);
	}
	if( sync_flag==2 )
	{
		strcpy(temp_buff, "janus\n");
		send(ts_socket, temp_buff, strlen(temp_buff), 0);
		sync_flag=3;
		return;
	}
	if( sync_flag==3 )
	{
		New_XLi_Gps_Net_Search_Sync_Head3(ch);
	}
	if( sync_flag==4 )
	{
		strcpy(temp_buff, "f27 b1\n");
		send(ts_socket, temp_buff, strlen(temp_buff), 0);
		sync_flag=5;
		sync_count=0;
		val_offset=-1;
		val_len=0;
		return;
	}

	if( (ch<0x0a) || (sync_flag!=5) )
		return;
	if( (ch==0x0a) || (ch==0x0d) )
	{
		sync_count=0;
		val_offset=-1;
		val_len=0;
		return;
	}
	value[val_len]=ch;
	val_len++;
	if( val_len==13 )
	{
		New_XLi_Gps_Net_Data_Proc1(rtu_chan_num, (unsigned char *)&value[0]);
	}
	if( val_len==21 )
	{
		New_XLi_Gps_Net_Data_Proc2(rtu_chan_num, (unsigned char *)&value[13]);
	}
	if( val_len==28 )
	{
		New_XLi_Gps_Net_Data_Proc3(rtu_chan_num, (unsigned char *)&value[21]);
	}
	if( val_len==36 )
	{
		New_XLi_Gps_Net_Data_Proc4(rtu_chan_num, (unsigned char *)&value[28]);
	}
	if( val_len==50 )
	{
		New_XLi_Gps_Net_Data_Proc5(rtu_chan_num, (unsigned char *)&value[36]);
	}
	if( val_len<50 )
		return;
	Gps_Report_Disp.DispStringWithType(rtu_chan_num, &value[0], Disp_Recv_Type, val_len, (char)0);
	sync_count=0;
	val_offset=-1;
	val_len=0;
}

void New_XLi_Gps_Net_Search_Sync_Head1(char ch)
{
char	sync_head[64];

	strcpy(sync_head, "USER NAME:   ");
	if( ch==sync_head[sync_count] )
	{
		sync_count++;
		if( sync_count==11 )
		{
			sync_count=0;
			sync_flag=0;
			val_offset=-1;
			return;
		}
	}
	else if( ch==sync_head[0] )
	{
		sync_count=1;
	}
	else
	{
		sync_count=0;
		sync_flag=-1;
		val_offset=-1;
	}
}

void New_XLi_Gps_Net_Search_Sync_Head2(char ch)
{
char	sync_head[64];

	strcpy(sync_head, "PASSWORD:   ");
	if( ch==sync_head[sync_count] )
	{
		sync_count++;
		if( sync_count==10 )
		{
			sync_count=0;
			sync_flag=2;
			val_offset=-1;
			return;
		}
	}
	else if( ch==sync_head[0] )
	{
		sync_count=1;
	}
	else
	{
		sync_count=0;
		sync_flag=1;
		val_offset=-1;
	}
}

void New_XLi_Gps_Net_Search_Sync_Head3(char ch)
{
char	sync_head[64];

	strcpy(sync_head, "SUCCESSFUL!  ");
	if( ch==sync_head[sync_count] )
	{
		sync_count++;
		if( sync_count==11 )
		{
			sync_count=0;
			sync_flag=4;
			val_offset=-1;
			return;
		}
	}
	else if( ch==sync_head[0] )
	{
		sync_count=1;
	}
	else
	{
		sync_count=0;
		sync_flag=3;
		val_offset=-1;
	}
}

int New_XLi_Gps_Net_Data_Proc1(int rtu_chan_num, unsigned char *buff)
{
int		day, hour, min, sec, yc_int, ret;
unsigned char	duty, c_1, c_2, c_3;
float		yc_flo;
struct	tm	*phytime_t;
time_t		gpstime, machtime;
int		net_gps_time;

	if( !(IsValidNumber(buff[0]) && IsValidNumber(buff[1])
	   && IsValidNumber(buff[2]) && IsValidNumber(buff[4])
	   && IsValidNumber(buff[5]) && IsValidNumber(buff[7])
	   && IsValidNumber(buff[8]) && IsValidNumber(buff[10])
	   && IsValidNumber(buff[11])) )
	{
		return(-1);
	}

	if( (buff[3]!=':') || (buff[6]!=':') || (buff[9]!=':') )
	{
		printf("wrong format, should be : \n");
		return(-2);
	}

	c_1 = buff[0] - 0x30;
	c_2 = buff[1] - 0x30;
	c_3 = buff[2] - 0x30;
	day = c_1*100 + c_2*10 + c_3;

	c_1 = buff[4] - 0x30;
	c_2 = buff[5] - 0x30;
	hour = c_1*10 + c_2;

	c_1 = buff[7] - 0x30;
	c_2 = buff[8] - 0x30;
	min = c_1*10 + c_2;

	c_1 = buff[10] - 0x30;
	c_2 = buff[11] - 0x30;
	sec = c_1*10 + c_2;

	yc_flo=(float)day;
	Gps_Refresh_Data(rtu_chan_num, day, yc_flo, DAY_TYPE);
	yc_flo=(float)hour;
	Gps_Refresh_Data(rtu_chan_num, hour, yc_flo, HOUR_TYPE);
	yc_flo=(float)min;
	Gps_Refresh_Data(rtu_chan_num, min, yc_flo, MINUTE_TYPE);
	yc_flo=(float)sec;
	Gps_Refresh_Data(rtu_chan_num, sec, yc_flo, SECOND_TYPE);

	switch( buff[12] )
	{
		case	' ':
			yc_int=0;
			yc_flo=0.0;
			break;
		case	'.':
			yc_int=1;
			yc_flo=1.0;
			break;
		case	'*':
			yc_int=2;
			yc_flo=2.0;
			break;
		case	'#':
			yc_int=3;
			yc_flo=3.0;
			break;
		default:
			yc_int=4;
			yc_flo=4.0;
			break;
	}
	Gps_Refresh_Data(rtu_chan_num, yc_int, yc_flo, TIME_QUALITY_TYPE);

printf(_("!!!!!!!!!!!!!! 时间 : %d天%d时%d分%d秒\n"),day,hour,min,sec);

	if( buff[12]==' ' )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_total+=3;
	}
	else
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_error+=3;
		return(-3);
	}
	duty = 0;
	duty=SysManTabPtr->fcheck[rtu_chan_num].if_duty;
	duty &= 0x07;
	if( duty!=DUTY )
	{
		return(-5);
	}
	duty = 0;
	duty=SysManTabPtr->fcheck[rtu_chan_num].status;
	duty &= 0x07;
	if( duty==STOPPED )
	{
		return(-6);
	}

	time(&machtime);
	phytime_t=localtime(&machtime);
	if( day!=(phytime_t->tm_yday+1) )
	{
		return(-7);
	}

	phytime_t->tm_hour	= hour;
	phytime_t->tm_min	= min;
	phytime_t->tm_sec	= sec;
	gpstime=mktime(phytime_t);
	net_gps_time = htonl(gpstime);

	ret=sendto(send_sock, (char *)&net_gps_time, sizeof(net_gps_time), 0,
			(struct sockaddr *)&fdst_sendto, sizeof(fdst_sendto));
printf("!!!!!!!!!!!!!!!!!!! sendto===%d\n",ret);

	return(0);
}

int New_XLi_Gps_Net_Data_Proc2(int rtu_chan_num, unsigned char *buff)
{
int	yc_int;
float	f_val;
char	c1, c2, c3, c4, c5, c6;

	if( (buff[0]!='T') && (buff[0]!='t') )
	{
		return(-1);
	}
	c1 = buff[2] - 0x30;
	if( c1==-16 )	c1=0;
	c2 = buff[3] - 0x30;
	if( c2==-16 )	c2=0;
	c3 = buff[4] - 0x30;
	if( c3==-16 )	c3=0;
	c4 = buff[5] - 0x30;
	if( c4==-16 )	c4=0;
	c5 = buff[6] - 0x30;
	if( c5==-16 )	c5=0;
	c6 = buff[7] - 0x30;
	if( c6==-16 )	c6=0;

	f_val = (c1*10000 + c2*1000 + c4*100 +c5*10 + c6)/1000.0;
	if( c2==-2 )
	{
		f_val = (c1*1000 + c3*100 + c4*10 +c5)/1000.0;
	}
	if( buff[1]=='-' )
		f_val *= (-1.0);
printf(_("!!!!!!!!!!!!!!!!!!! 时差 : %3.3f秒\n"),f_val);

	yc_int=(int)f_val;
	Gps_Refresh_Data(rtu_chan_num, yc_int, f_val, TIME_DIFF_TYPE);
	return(0);
}

int New_XLi_Gps_Net_Data_Proc3(int rtu_chan_num, unsigned char *buff)
{
int	yc_int;
float	f_val;
char	c1, c2, c3, c4;

	if( (buff[0]!='F') && (buff[0]!='f') )
	{
		return(-1);
	}
	c1 = buff[2] - 0x30;
	if( c1==-16 )	c1=0;
	c2 = buff[4] - 0x30;
	if( c2==-16 )	c2=0;
	c3 = buff[5] - 0x30;
	if( c3==-16 )	c3=0;
	c4 = buff[6] - 0x30;
	if( c4==-16 )	c4=0;

	f_val = (c1*1000 + c2*100 + c3*10 + c4)/1000.0;
	if( buff[1]=='-' )
		f_val *= (-1.0);
printf(_("!!!!!!!!!!!!!!!!!!!! 频差 : %2.3fHz\n"),f_val);

	yc_int=(int)f_val;
	Gps_Refresh_Data(rtu_chan_num, yc_int, f_val, FREQ_DIFF_TYPE);
	return(0);
}

int New_XLi_Gps_Net_Data_Proc4(int rtu_chan_num, unsigned char *buff)
{
int	yc_int;
float	f_val;
char	c1, c2, c3, c4, c5;

	if( (buff[0]!='S') && (buff[1]!='F') )
	{
		return(-1);
	}
	c1 = buff[2] - 0x30;
	if( c1==-16 )	c1=0;
	c2 = buff[3] - 0x30;
	if( c2==-16 )	c2=0;
	c3 = buff[5] - 0x30;
	if( c3==-16 )	c3=0;
	c4 = buff[6] - 0x30;
	if( c4==-16 )	c4=0;
	c5 = buff[7] - 0x30;
	if( c5==-16 )	c5=0;

	f_val = (c1*10000 + c2*1000 + c3*100 +c4*10 + c5)/1000.0;
printf(_("!!!!!!!!!!!!!!!!!!!! 频率 : %2.3fHz\n"),f_val);

	yc_int=(int)f_val;
	Gps_Refresh_Data(rtu_chan_num, yc_int, f_val, FREQUENCY_TYPE);

	if( (f_val<40.0) || (f_val>60.0) )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_error+=30;
	}
	if( (fabs(f_val-freq))>0.0005 )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_total+=2;
	}
	freq=f_val;
	return(0);
}

int New_XLi_Gps_Net_Data_Proc5(int rtu_chan_num, unsigned char *buff)
{
int	hour, min, yc_int;
float	f_val, yc_flo;
char	c1, c2, c3, c4, c5;

	if( (buff[0]!='S') && (buff[1]!='T') )
	{
		return(-1);
	}
	c1 = buff[2] - 0x30;
	c2 = buff[3] - 0x30;
	hour = c1*10 + c2;

	c1 = buff[5] - 0x30;
	c2 = buff[6] - 0x30;
	min = c1*10 + c2;

	c1 = buff[8] - 0x30;
	c2 = buff[9] - 0x30;
	c3 = buff[11] - 0x30;
	c4 = buff[12] - 0x30;
	c5 = buff[13] - 0x30;

	f_val = (c1*10000 + c2*1000 + c3*100 +c4*10 + c5)/1000.0;
printf(_("!!!!!!!!!!!!!! 电钟 : %d时%d分%2.3f秒\n"),hour,min,f_val);

	yc_flo=(float)hour;
	Gps_Refresh_Data(rtu_chan_num, hour, yc_flo, ELECTRO_HOUR_TYPE);

	yc_flo=(float)min;
	Gps_Refresh_Data(rtu_chan_num, min, yc_flo, ELECTRO_MINUTE_TYPE);

	yc_int=(int)f_val;
	Gps_Refresh_Data(rtu_chan_num, yc_int, f_val, ELECTRO_SECOND_TYPE);
	return(0);
}

//************************************ 中国Vcom天文钟 ************************************

int Vcom_Gps_Interfase(int rtu_chan_num, char *buff, int len)
{
int		year, month, day, hour, min, sec, hour1, min1, sec1, yc_int, ret;
unsigned char	duty, c1, c2, c3, c4, c5, c6;
float		zb_value, yc_flo;
struct	tm	phytime_t;
time_t		gpstime;
int		net_gps_time;

	Gps_Report_Disp.DispStringWithType(rtu_chan_num, &buff[0], Disp_Recv_Type, len, (char)0);
	if( len<41 )
		return(-1);

	if( (buff[0]!='T') && (buff[0]!='t') )
	{
		printf("wrong format, should be T/t\n");
		return(-2);
	}
	if( (buff[16]!='F') && (buff[16]!='f') )
	{
		printf("wrong format, should be F/f \n");
		return(-3);
	}
	if( (buff[24]!='C') && (buff[24]!='c') )
	{
		printf("wrong format, should be C/c \n");
		return(-4);
	}
	if( (buff[31]!='+') && (buff[31]!='-') )
	{
		printf("wrong format, should be +/- \n");
		return(-5);
	}
	if( buff[38]!='*' )
	{
		printf("wrong format, should be * \n");
		return(-6);
	}
	if( (buff[19]!='.') && (buff[35]!='.') )
	{
		printf("wrong format, should be . \n");
		return(-7);
	}
	if( !(IsValidNumber(buff[2]) && IsValidNumber(buff[3])
	   && IsValidNumber(buff[4]) && IsValidNumber(buff[5])
	   && IsValidNumber(buff[6]) && IsValidNumber(buff[7])
	   && IsValidNumber(buff[8]) && IsValidNumber(buff[9])
	   && IsValidNumber(buff[10]) && IsValidNumber(buff[11])
	   && IsValidNumber(buff[12]) && IsValidNumber(buff[13])
	   && IsValidNumber(buff[14]) && IsValidNumber(buff[15])
	   && IsValidNumber(buff[17]) && IsValidNumber(buff[18])
	   && IsValidNumber(buff[20]) && IsValidNumber(buff[21])
	   && IsValidNumber(buff[22]) && IsValidNumber(buff[23])) )
	{
		printf("wrong format, time \n");
		return(-8);
	}

	c1 = buff[2] - 0x30;
	c2 = buff[3] - 0x30;
	c3 = buff[4] - 0x30;
	c4 = buff[5] - 0x30;
	year = c1*1000 + c2*100 + c3*10 + c4;

	c1 = buff[6] - 0x30;
	c2 = buff[7] - 0x30;
	month = c1*10 + c2;

	c1 = buff[8] - 0x30;
	c2 = buff[9] - 0x30;
	day = c1*10 + c2;

	c1 = buff[10] - 0x30;
	c2 = buff[11] - 0x30;
	hour = c1*10 + c2;

	c1 = buff[12] - 0x30;
	c2 = buff[13] - 0x30;
	min = c1*10 + c2;

	c1 = buff[14] - 0x30;
	c2 = buff[15] - 0x30;
	sec = c1*10 + c2;
printf(_("!!!!!!!!!!!!!! 时间 : %d年%d月%d日%d时%d分%d秒\n"),year,month,day,hour,min,sec);

	yc_flo=(float)year;
	Gps_Refresh_Data(rtu_chan_num, year, yc_flo, YEAR_TYPE);
	yc_flo=(float)month;
	Gps_Refresh_Data(rtu_chan_num, month, yc_flo, MONTH_TYPE);
	yc_flo=(float)day;
	Gps_Refresh_Data(rtu_chan_num, day, yc_flo, DAY_TYPE);
	yc_flo=(float)hour;
	Gps_Refresh_Data(rtu_chan_num, hour, yc_flo, HOUR_TYPE);
	yc_flo=(float)min;
	Gps_Refresh_Data(rtu_chan_num, min, yc_flo, MINUTE_TYPE);
	yc_flo=(float)sec;
	Gps_Refresh_Data(rtu_chan_num, sec, yc_flo, SECOND_TYPE);

	switch( buff[1] )
	{
		case	'S':
		case	's':
			yc_int=0;
			yc_flo=0.0;
			break;
		case	'A':
		case	'a':
		default:
			yc_int=1;
			yc_flo=1.0;
			break;
	}
	Gps_Refresh_Data(rtu_chan_num, yc_int, yc_flo, TIME_QUALITY_TYPE);

	c1 = buff[17] - 0x30;
	c2 = buff[18] - 0x30;
	c3 = buff[20] - 0x30;
	c4 = buff[21] - 0x30;
	c5 = buff[22] - 0x30;
	c6 = buff[23] - 0x30;
	zb_value = c1*100000.0+c2*10000.0+c3*1000.0+c4*100.0+c5*10.0+c6;
	zb_value = zb_value/10000.0;
printf(_("!!!!!!!!!!!!!!!!!!!! 频率 : %2.4fHz\n"),zb_value);
	yc_int=(int)zb_value;
	Gps_Refresh_Data(rtu_chan_num, yc_int, zb_value, FREQUENCY_TYPE);

	if( (zb_value<40.0) || (zb_value>60.0) )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_error+=30;
	}
	else if( (fabs(zb_value-freq))>0.0005 )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_total+=2;
	}
	freq=zb_value;

	c1 = buff[25] - 0x30;
	c2 = buff[26] - 0x30;
	hour1 = c1*10 + c2;

	c1 = buff[27] - 0x30;
	c2 = buff[28] - 0x30;
	min1 = c1*10 + c2;

	c1 = buff[29] - 0x30;
	c2 = buff[30] - 0x30;
	sec1 = c1*10 + c2;
printf(_("!!!!!!!!!!!!!! 工频钟 : %d时%d分%d秒\n"),hour1,min1,sec1);

	yc_flo=(float)hour1;
	Gps_Refresh_Data(rtu_chan_num, hour1, yc_flo, ELECTRO_HOUR_TYPE);
	yc_flo=(float)min1;
	Gps_Refresh_Data(rtu_chan_num, min1, yc_flo, ELECTRO_MINUTE_TYPE);
	yc_flo=(float)sec1;
	Gps_Refresh_Data(rtu_chan_num, sec1, yc_flo, ELECTRO_SECOND_TYPE);

	c1 = buff[32] - 0x30;
	c2 = buff[33] - 0x30;
	c3 = buff[34] - 0x30;
	c4 = buff[36] - 0x30;
	c5 = buff[37] - 0x30;
	yc_flo = (c1*10000 + c2*1000 + c3*100 +c4*10 + c5)/100.0;
	if( buff[31]=='-' )
		yc_flo *= (-1.0);
printf(_("!!!!!!!!!!!!!!!!!!! 工频钟差 : %3.2f秒\n"),yc_flo);

	yc_int=(int)yc_flo;
	Gps_Refresh_Data(rtu_chan_num, yc_int, yc_flo, TIME_DIFF_TYPE);

	if( (buff[1]=='S') || (buff[1]=='s') )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_total+=3;
	}
	else
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_error+=3;
		return(3);
	}

	duty = 0;
	duty=SysManTabPtr->fcheck[rtu_chan_num].if_duty;
	duty &= 0x07;
	if( duty!=DUTY )
	{
		return(4);
	}

	phytime_t.tm_year	= year-1900;
	phytime_t.tm_mon	= month-1;
	phytime_t.tm_mday	= day;
	phytime_t.tm_hour	= hour;
	phytime_t.tm_min	= min;
	phytime_t.tm_sec	= sec;
	gpstime=mktime(&phytime_t);
	net_gps_time = htonl(gpstime);

	ret=sendto(send_sock, (char *)&net_gps_time, sizeof(net_gps_time), 0,
			(struct sockaddr *)&fdst_sendto, sizeof(fdst_sendto));
printf("!!!!!!!!!!!!!!!!!!! sendto===%d\n",ret);

	return(ret);
}

void Vcom_Gps_Interfase2(int rtu_chan_num, char ch)
{
	if( ch==0x0d )
	{
		sync_count=0;
		sync_flag=-1;
		val_len=0;
	}
	if( sync_flag<=0 )
	{
		Vcom_Gps_Search_Sync_Head(ch);
		return;
	}
	value[val_len]=ch;
	if( (value[0]!=0x54) || ((val_len==16)&&(value[16]!=0x46))
	 || ((val_len==24)&&(value[24]!=0x43)) )
	{
		sync_flag=-1;
		sync_count=0;
		val_len=0;
		return;
	}
	val_len++;

	if( val_len==16 )
	{
		Vcom_Gps_Data1_Proc(rtu_chan_num, (unsigned char *)&value[0]);
	}
	if( val_len==24 )
	{
		Vcom_Gps_Data2_Proc(rtu_chan_num, (unsigned char *)&value[16]);
	}

	if( val_len<41 )
		return;
	Vcom_Gps_Data3_Proc(rtu_chan_num, (unsigned char *)&value[24]);
	Gps_Report_Disp.DispStringWithType(rtu_chan_num, &value[0], Disp_Recv_Type, val_len, (char)0);

	sync_flag=-1;
	sync_count=0;
	val_len=0;
}

void Vcom_Gps_Search_Sync_Head(char ch)
{
	if( sync_count==0 )
	{
		if( ch==0x0d )
		{
			sync_count=1;
			val_len=0;
			return;
		}
	}
	else if( sync_count==1 )
	{
		if( ch==0x0a )
		{
			sync_count=2;
			val_len=0;
			return;
		}
		else
		{
			sync_count=0;
			val_len=0;
			return;
		}
	}
	else if( sync_count==2 )
	{
		if( ch==0x54 )		//'T'
		{
			sync_count=0;
			sync_flag=1;
			value[0]=ch;
			val_len=1;
			return;
		}
		else if( ch==0x0d )
		{
			sync_count=1;
			val_len=0;
			return;
		}
		else
		{
			sync_count=0;
			val_len=0;
			return;
		}
	}
}

int Vcom_Gps_Data1_Proc(int rtu_chan_num, unsigned char *buff)
{
int 		hour, min, sec, year, month, day, yc_int, ret;
float		yc_flo=0.0;
unsigned char	c1, c2, c3, c4, duty;
struct	tm	phytime_t;
time_t		gpstime;
int		net_gps_time;

	if( !(IsValidNumber(buff[2]) && IsValidNumber(buff[3])
	   && IsValidNumber(buff[4]) && IsValidNumber(buff[5])
	   && IsValidNumber(buff[6]) && IsValidNumber(buff[7])
	   && IsValidNumber(buff[8]) && IsValidNumber(buff[9])
	   && IsValidNumber(buff[10]) && IsValidNumber(buff[11])
	   && IsValidNumber(buff[12]) && IsValidNumber(buff[13])
	   && IsValidNumber(buff[14]) && IsValidNumber(buff[15])) )
	{
		printf("wrong format, time \n");
		return(-1);
	}

	c1 = buff[2] - 0x30;
	c2 = buff[3] - 0x30;
	c3 = buff[4] - 0x30;
	c4 = buff[5] - 0x30;
	year = c1*1000 + c2*100 + c3*10 + c4;

	c1 = buff[6] - 0x30;
	c2 = buff[7] - 0x30;
	month = c1*10 + c2;

	c1 = buff[8] - 0x30;
	c2 = buff[9] - 0x30;
	day = c1*10 + c2;

	c1 = buff[10] - 0x30;
	c2 = buff[11] - 0x30;
	hour = c1*10 + c2;

	c1 = buff[12] - 0x30;
	c2 = buff[13] - 0x30;
	min = c1*10 + c2;

	c1 = buff[14] - 0x30;
	c2 = buff[15] - 0x30;
	sec = c1*10 + c2;
printf(_("!!!!!!!!!!!!!! 时间 : %d年%d月%d日%d时%d分%d秒\n"),year,month,day,hour,min,sec);

	yc_flo=(float)year;
	Gps_Refresh_Data(rtu_chan_num, year, yc_flo, YEAR_TYPE);
	yc_flo=(float)month;
	Gps_Refresh_Data(rtu_chan_num, month, yc_flo, MONTH_TYPE);
	yc_flo=(float)day;
	Gps_Refresh_Data(rtu_chan_num, day, yc_flo, DAY_TYPE);
	yc_flo=(float)hour;
	Gps_Refresh_Data(rtu_chan_num, hour, yc_flo, HOUR_TYPE);
	yc_flo=(float)min;
	Gps_Refresh_Data(rtu_chan_num, min, yc_flo, MINUTE_TYPE);
	yc_flo=(float)sec;
	Gps_Refresh_Data(rtu_chan_num, sec, yc_flo, SECOND_TYPE);

	switch( buff[1] )
	{
		case	'S':
		case	's':
			yc_int=0;
			yc_flo=0.0;
			break;
		case	'A':
		case	'a':
		default:
			yc_int=1;
			yc_flo=1.0;
			break;
	}
	Gps_Refresh_Data(rtu_chan_num, yc_int, yc_flo, TIME_QUALITY_TYPE);

	if( (buff[1]=='S') || (buff[1]=='s') )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_total+=3;
	}
	else
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_error+=3;
		return(3);
	}

	duty = 0;
	duty=SysManTabPtr->fcheck[rtu_chan_num].if_duty;
	duty &= 0x07;
	if( duty!=DUTY )
	{
		return(4);
	}

	phytime_t.tm_year	= year-1900;
	phytime_t.tm_mon	= month-1;
	phytime_t.tm_mday	= day;
	phytime_t.tm_hour	= hour;
	phytime_t.tm_min	= min;
	phytime_t.tm_sec	= sec;
	gpstime=mktime(&phytime_t);
	net_gps_time = htonl(gpstime);

	ret=sendto(send_sock, (char *)&net_gps_time, sizeof(net_gps_time), 0,
			(struct sockaddr *)&fdst_sendto, sizeof(fdst_sendto));
printf("!!!!!!!!!!!!!!!!!!! sendto===%d\n",ret);
	return(ret);
}

int Vcom_Gps_Data2_Proc(int rtu_chan_num, unsigned char *buff)
{
int 		yc_int;
float		zb_value=0.0;
unsigned char	c1, c2, c3, c4, c5, c6;

	if( !(IsValidNumber(buff[1]) && IsValidNumber(buff[2])
	   && IsValidNumber(buff[4]) && IsValidNumber(buff[5])
	   && IsValidNumber(buff[6]) && IsValidNumber(buff[7])) )
	{
		printf("wrong format, time \n");
		return(-1);
	}
	if( buff[3]!='.' )
	{
		printf("wrong format, should be . \n");
		return(-2);
	}

	c1 = buff[1] - 0x30;
	c2 = buff[2] - 0x30;
	c3 = buff[4] - 0x30;
	c4 = buff[5] - 0x30;
	c5 = buff[6] - 0x30;
	c6 = buff[7] - 0x30;
	zb_value = c1*100000.0+c2*10000.0+c3*1000.0+c4*100.0+c5*10.0+c6;
	zb_value = zb_value/10000.0;
printf(_("!!!!!!!!!!!!!!!!!!!! 频率 : %2.4fHz\n"),zb_value);
	yc_int=(int)zb_value;
	Gps_Refresh_Data(rtu_chan_num, yc_int, zb_value, FREQUENCY_TYPE);

	if( (zb_value<40.0) || (zb_value>60.0) )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_error+=30;
	}
	else if( (fabs(zb_value-freq))>0.0005 )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_total+=2;
	}
	freq=zb_value;

	return(1);
}

int Vcom_Gps_Data3_Proc(int rtu_chan_num, unsigned char *buff)
{
int 		hour1, min1, sec1, yc_int;
float		yc_flo=0.0;
unsigned char	c1, c2, c3, c4, c5;

	if( !(IsValidNumber(buff[1]) && IsValidNumber(buff[2])
	   && IsValidNumber(buff[3]) && IsValidNumber(buff[4])
	   && IsValidNumber(buff[5]) && IsValidNumber(buff[6])
	   && IsValidNumber(buff[8]) && IsValidNumber(buff[9])
	   && IsValidNumber(buff[10]) && IsValidNumber(buff[12])
	   && IsValidNumber(buff[13])) )
	{
		printf("wrong format, time \n");
		return(-1);
	}
	if( buff[11]!='.' )
	{
		printf("wrong format, should be . \n");
		return(-2);
	}

	c1 = buff[1] - 0x30;
	c2 = buff[2] - 0x30;
	hour1 = c1*10 + c2;

	c1 = buff[3] - 0x30;
	c2 = buff[4] - 0x30;
	min1 = c1*10 + c2;

	c1 = buff[5] - 0x30;
	c2 = buff[6] - 0x30;
	sec1 = c1*10 + c2;
printf(_("!!!!!!!!!!!!!! 工频钟 : %d时%d分%d秒\n"),hour1,min1,sec1);

	yc_flo=(float)hour1;
	Gps_Refresh_Data(rtu_chan_num, hour1, yc_flo, ELECTRO_HOUR_TYPE);
	yc_flo=(float)min1;
	Gps_Refresh_Data(rtu_chan_num, min1, yc_flo, ELECTRO_MINUTE_TYPE);
	yc_flo=(float)sec1;
	Gps_Refresh_Data(rtu_chan_num, sec1, yc_flo, ELECTRO_SECOND_TYPE);

	c1 = buff[8] - 0x30;
	c2 = buff[9] - 0x30;
	c3 = buff[10] - 0x30;
	c4 = buff[12] - 0x30;
	c5 = buff[13] - 0x30;
	yc_flo = (c1*10000 + c2*1000 + c3*100 +c4*10 + c5)/100.0;
	if( buff[7]=='-' )
		yc_flo *= (-1.0);
printf(_("!!!!!!!!!!!!!!!!!!! 工频钟差 : %3.2f秒\n"),yc_flo);

	yc_int=(int)yc_flo;
	Gps_Refresh_Data(rtu_chan_num, yc_int, yc_flo, TIME_DIFF_TYPE);

	return(1);
}

//************************************ 中国北斗天文钟 *************************************

void BD_Gps_Interfase(int rtu_chan_num, char ch)
{
	if( ch==0x0d )
	{
		sync_count=0;
		sync_flag=-1;
		val_len=0;
	}
	if( sync_flag<=0 )
	{
		BD_Gps_Search_Sync_Head(ch);
		return;
	}
	value[val_len]=ch;
	val_len++;

	if( val_len<9 )
		return;
	BD_Gps_Data1_Proc(rtu_chan_num, (unsigned char *)&value[0]);
	Gps_Report_Disp.DispStringWithType(rtu_chan_num, &value[0], Disp_Recv_Type, val_len, (char)0);

	sync_flag=-1;
	sync_count=0;
	val_len=0;
}

void BD_Gps_Search_Sync_Head(char ch)
{
	if( sync_count==0 )
	{
		if( ch=='$' )
		{
			sync_count=1;
			val_len=0;
			return;
		}
	}
	else if( sync_count==1 )
	{
		if( ch==0x01 )
		{
			sync_count=0;
			sync_flag=1;
			value[0]=ch;
			val_len=1;
			return;
		}
		else
		{
			sync_count=0;
			val_len=0;
			return;
		}
	}
	else
	{
		sync_count=0;
		val_len=0;
		return;
	}
}

void BD_Gps_Data1_Proc(int rtu_chan_num, unsigned char *buff)
{
int		year, day, hour, min, sec, ret;
float		yc_flo;
unsigned char	duty;
struct	tm	*phytime_t;
time_t		gpstime, machtime;
int		net_gps_time;

	year = buff[2]+2000;
	day = buff[3]*256+buff[4];
	hour = buff[5];
	min = buff[6];
	sec = buff[7];

printf(_("!!!!!!!!!!!!!! 时间 : %d年%d天%d时%d分%d秒\n"),year,day,hour,min,sec);

	yc_flo=(float)year;
	Gps_Refresh_Data(rtu_chan_num, year, yc_flo, YEAR_TYPE);

	yc_flo=(float)day;
	Gps_Refresh_Data(rtu_chan_num, day, yc_flo, DAY_TYPE);

	yc_flo=(float)hour;
	Gps_Refresh_Data(rtu_chan_num, hour, yc_flo, HOUR_TYPE);

	yc_flo=(float)min;
	Gps_Refresh_Data(rtu_chan_num, min, yc_flo, MINUTE_TYPE);

	yc_flo=(float)sec;
	Gps_Refresh_Data(rtu_chan_num, sec, yc_flo, SECOND_TYPE);


	SysManTabPtr->fcheck[rtu_chan_num].current_total+=3;
	duty=SysManTabPtr->fcheck[rtu_chan_num].if_duty;
	duty &= 0x07;
	if( duty!=DUTY )
	{
		return;
	}

	time(&machtime);
	phytime_t=localtime(&machtime);
	if( day!=(phytime_t->tm_yday+1) )
	{
		return;
	}
	phytime_t->tm_hour	= hour;
	phytime_t->tm_min	= min;
	phytime_t->tm_sec	= sec;
	gpstime=mktime(phytime_t);
	net_gps_time = htonl(gpstime);

	ret=sendto(send_sock, (char *)&net_gps_time, sizeof(net_gps_time), 0,
			(struct sockaddr *)&fdst_sendto, sizeof(fdst_sendto));
printf("!!!!!!!!!!!!!!!!!!! sendto===%d\n",ret);
}

//************************************ //YJD-2000卫星时钟系统频率协议1 (协议号031) ************************************
//modify by zhengqiang 2008.1.21
int YJD_Gps_Interfase(int rtu_chan_num, char *buff, int len)
{
int		year, month, day, hour, min, sec, hour1, min1, sec1, yc_int, ret;
unsigned char	duty, c1, c2, c3, c4, c5, c6,c7,c8;
float		zb_value, yc_flo;
struct	tm	phytime_t;
time_t		gpstime;
int		net_gps_time;

	Gps_Report_Disp.DispStringWithType(rtu_chan_num, &buff[0], Disp_Recv_Type, len, (char)0);
	if( len<44 )
		return(-1);

	if (buff[0]!=0x02)
	{
		printf("wrong format, should be 0x02\n");
		return(-2);
	}
	if( (buff[15]!=0x0a) || (buff[16]!=0x0d) )
	{
		printf("wrong format \n");
		return(-3);
	}
	if( (buff[22]!=0x0a) || (buff[23]!=0x0d) )
	{
		printf("wrong format \n");
		return(-4);
	}
	if( (buff[30]!=0x0a) || (buff[31]!=0x0d) )
	{
		printf("wrong format \n");
		return(-5);
	}
	if( (buff[41]!=0x0a) || (buff[42]!=0x0d) )
	{
		printf("wrong format \n");
		return(-6);
	}
	if (buff[43]!=0x03)
	{
		printf("wrong format, should be 0x03\n");
		return(-7);
	}
	if( !(IsValidNumber(buff[2]) && IsValidNumber(buff[3])
	   && IsValidNumber(buff[4]) && IsValidNumber(buff[5])
	   && IsValidNumber(buff[6]) && IsValidNumber(buff[7])
	   && IsValidNumber(buff[8]) && IsValidNumber(buff[9])
	   && IsValidNumber(buff[10]) && IsValidNumber(buff[11])
	   && IsValidNumber(buff[12]) && IsValidNumber(buff[13])
	   && IsValidNumber(buff[14]) && IsValidNumber(buff[17])
	   && IsValidNumber(buff[18]) && IsValidNumber(buff[19])
	   && IsValidNumber(buff[20]) && IsValidNumber(buff[21])
	   && IsValidNumber(buff[24]) && IsValidNumber(buff[25])
	   && IsValidNumber(buff[26]) && IsValidNumber(buff[27])
	   && IsValidNumber(buff[28]) && IsValidNumber(buff[29])
	   && IsValidNumber(buff[32]) && IsValidNumber(buff[33])
	   && IsValidNumber(buff[34]) && IsValidNumber(buff[35])
	   && IsValidNumber(buff[36]) && IsValidNumber(buff[37])
	   && IsValidNumber(buff[38]) && IsValidNumber(buff[39])
	   && IsValidNumber(buff[40]) ) )
	{
		printf("wrong format, time \n");
		return(-8);
	}

	c1 = buff[13] - 0x30;
	c2 = buff[14] - 0x30;

	year = 2000 + c1*10 + c2;

	c1 = buff[11] - 0x30;
	c2 = buff[12] - 0x30;
	month = c1*10 + c2;

	c1 = buff[9] - 0x30;
	c2 = buff[10] - 0x30;
	day = c1*10 + c2;

	c1 = buff[3] - 0x30;
	c2 = buff[4] - 0x30;
	hour = c1*10 + c2;

	c1 = buff[5] - 0x30;
	c2 = buff[6] - 0x30;
	min = c1*10 + c2;

	c1 = buff[7] - 0x30;
	c2 = buff[8] - 0x30;
	sec = c1*10 + c2;
printf(_("!!!!!!!!!!!!!! 时间 : %04d年%02d月%02d日%02d时%02d分%02d秒\n"),year,month,day,hour,min,sec);

	yc_flo=(float)year;
	Gps_Refresh_Data(rtu_chan_num, year, yc_flo, YEAR_TYPE);
	yc_flo=(float)month;
	Gps_Refresh_Data(rtu_chan_num, month, yc_flo, MONTH_TYPE);
	yc_flo=(float)day;
	Gps_Refresh_Data(rtu_chan_num, day, yc_flo, DAY_TYPE);
	yc_flo=(float)hour;
	Gps_Refresh_Data(rtu_chan_num, hour, yc_flo, HOUR_TYPE);
	yc_flo=(float)min;
	Gps_Refresh_Data(rtu_chan_num, min, yc_flo, MINUTE_TYPE);
	yc_flo=(float)sec;
	Gps_Refresh_Data(rtu_chan_num, sec, yc_flo, SECOND_TYPE);

	if( buff[1]==0x43 )
	{
            yc_int=0;
	    yc_flo=0.0;
	}
	else
	{    yc_int=1;
	     yc_flo=1.0;
	     }

	Gps_Refresh_Data(rtu_chan_num, yc_int, yc_flo, TIME_QUALITY_TYPE);

	c1 = buff[17] - 0x30;
	c2 = buff[18] - 0x30;
	c3 = buff[19] - 0x30;
	c4 = buff[20] - 0x30;
	c5 = buff[21] - 0x30;
	zb_value = c1*100000.0+c2*10000.0+c3*1000.0+c4*100.0+c5*10.0;
	zb_value = zb_value/10000.0;
printf(_("!!!!!!!!!!!!!!!!!!!! 频率 : %2.4fHz\n"),zb_value);
	yc_int=(int)zb_value;
	Gps_Refresh_Data(rtu_chan_num, yc_int, zb_value, FREQUENCY_TYPE);
        yc_flo=zb_value-50.0;
        yc_int=(int)yc_flo;
        Gps_Refresh_Data(rtu_chan_num, yc_int, yc_flo, FREQ_DIFF_TYPE);
	if( (zb_value<40.0) || (zb_value>60.0) )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_error+=30;
	}
	else if( (fabs(zb_value-freq))>0.0005 )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_total+=2;
	}
	freq=zb_value;

	c1 = buff[24] - 0x30;
	c2 = buff[25] - 0x30;
	hour1 = c1*10 + c2;

	c1 = buff[26] - 0x30;
	c2 = buff[27] - 0x30;
	min1 = c1*10 + c2;

	c1 = buff[28] - 0x30;
	c2 = buff[29] - 0x30;
	sec1 = c1*10 + c2;
printf(_("!!!!!!!!!!!!!! 工频钟 : %02d时%02d分%02d秒\n"),hour1,min1,sec1);

	yc_flo=(float)hour1;
	Gps_Refresh_Data(rtu_chan_num, hour1, yc_flo, ELECTRO_HOUR_TYPE);
	yc_flo=(float)min1;
	Gps_Refresh_Data(rtu_chan_num, min1, yc_flo, ELECTRO_MINUTE_TYPE);
	yc_flo=(float)sec1;
	Gps_Refresh_Data(rtu_chan_num, sec1, yc_flo, ELECTRO_SECOND_TYPE);

	c1 = buff[33] - 0x30;
	c2 = buff[34] - 0x30;
	c3 = buff[35] - 0x30;
	c4 = buff[36] - 0x30;
	c5 = buff[37] - 0x30;
	c6 = buff[38] - 0x30;
	c7 = buff[39] - 0x30;
	c8 = buff[40] - 0x30;


	yc_flo = (c1*3600000 + c2*600000 + c3*60000 +c4*10000 + c5*1000+c6*100+c7*10+c8)/1000.0;
	if( buff[32]==0x31 )
		yc_flo *= (-1.0);
printf(_("!!!!!!!!!!!!!!!!!!! 工频钟差 : %3.2f秒\n"),yc_flo);

	yc_int=(int)yc_flo;
	Gps_Refresh_Data(rtu_chan_num, yc_int, yc_flo, TIME_DIFF_TYPE);

	if( buff[1]==0x43 )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_total+=3;
	}
	else
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_error+=3;
		return(3);
	}

	duty = 0;
	duty=SysManTabPtr->fcheck[rtu_chan_num].if_duty;
	duty &= 0x07;
	if( duty!=DUTY )
	{
		return(4);
	}

	phytime_t.tm_year	= year-1900;
	phytime_t.tm_mon	= month-1;
	phytime_t.tm_mday	= day;
	phytime_t.tm_hour	= hour;
	phytime_t.tm_min	= min;
	phytime_t.tm_sec	= sec;
	gpstime=mktime(&phytime_t);
	net_gps_time = htonl(gpstime);

	ret=sendto(send_sock, (char *)&net_gps_time, sizeof(net_gps_time), 0,
			(struct sockaddr *)&fdst_sendto, sizeof(fdst_sendto));
printf("!!!!!!!!!!!!!!!!!!! sendto===%d\n",ret);

	return(ret);
}

//************************ NARI TSS系列GPS标准时间同步钟 ************************

 void NARI_TSS_Gps_Interfase(int rtu_chan_num, char ch)
  {
	    if( ch==0x0a )
        {
                sync_count=0;
                sync_flag=-1;
                val_len=0;
        }
        if( sync_flag<=0 )
        {
                NARI_TSS_Gps_Search_Sync_Head(ch);
                return;
        }
        value[val_len]=ch;
        if( (value[0]!=0x25) || ((val_len==19)&&(value[19]!=0x2A)) )
        {
                sync_flag=-1;
                sync_count=0;
                val_len=0;
                return;
		}
        val_len++;

        printf("val_len=%d\n",val_len);

        if( val_len==35 )
        {
            NARI_TSS_Gps_Data1_Proc(rtu_chan_num, (unsigned char *)&value[0]);

        Gps_Report_Disp.DispAllWithType(rtu_chan_num, (unsigned char*)&value[0], Disp_Recv_Type, val_len, (char)0);

        sync_flag=-1;
        sync_count=0;
        val_len=0;
        }
}

void NARI_TSS_Gps_Search_Sync_Head(char ch)
{
        if( sync_count==0 )
        {
                if( ch==0x0a)
                {
                        sync_count=1;
                        val_len=0;
                        return;
                }
        }
        else if( sync_count==1 )
        {
                if( ch=='%' )
                {
                        sync_count=0;
                        sync_flag=1;
                        value[0]=ch;
                        val_len=1;
                        return;
                }
                else if( ch==0x0a )
                {
                        sync_count=1;
                        val_len=1;
                        return;
                }
                else
                {
                        sync_count=0;
                        val_len=0;
                        return;
                }
        }
}

int NARI_TSS_Gps_Data1_Proc(int rtu_chan_num, unsigned char *buff)
{
	unsigned char   duty;
	int             hour, min, sec, year, month, day, sum, yc_int, retcode;
	float           zb_value=0.0, yc_float=0.0;
	struct  tm      *phytime_t;
	time_t          gpstime, machtime;
	int             net_gps_time;
	/*
	hour  = (buff[5]-0x30)*10+ (buff[6]-0x30);
	yc_float=(float)hour;
	Gps_Refresh_Data(rtu_chan_num, hour, yc_float, HOUR_TYPE);

	min   = (buff[7]-0x30)*10+ (buff[8]-0x30);
	yc_float=(float)min;
	Gps_Refresh_Data(rtu_chan_num, min, yc_float, MINUTE_TYPE);

	sec   = (buff[9]-0x30)*10+ (buff[10]-0x30);
	yc_float=(float)sec;
	Gps_Refresh_Data(rtu_chan_num, sec, yc_float, SECOND_TYPE);

	year  =  (buff[13]-0x30)*10+ (buff[14]-0x30);
	yc_float=(float)year;
	Gps_Refresh_Data(rtu_chan_num, year, yc_float, YEAR_TYPE);

	month =  (buff[15]-0x30)*10+ (buff[16]-0x30);
	yc_float=(float)month;
	Gps_Refresh_Data(rtu_chan_num, month, yc_float, MONTH_TYPE);

	day   = (buff[17]-0x30)*10+ (buff[18]-0x30);
	yc_float=(float)day;
	Gps_Refresh_Data(rtu_chan_num, day, yc_float, DAY_TYPE);

	yc_int=(buff[20]-0x30)*10+ (buff[21]-0x30);
	if( yc_int<0 )
	yc_int=0;
	sum=(buff[22]-0x30)*1000+ (buff[23]-0x30)*100+(buff[24]-0x30)*10+(buff[25]-0x30);
	zb_value=(float)yc_int+(float)sum/10000.0;
	*/


	/*2530303832_30303138_3033_3032_3130_3539_3234_2A35303037 393232303034 333632390D 
		*/
	year  =  (buff[5]-0x30)*1000+ (buff[6]-0x30)*100+(buff[7]-0x30)*10+(buff[8]-0x30);
	yc_float=(float)year;
	Gps_Refresh_Data(rtu_chan_num, year, yc_float, YEAR_TYPE);

	month =  (buff[9]-0x30)*10+ (buff[10]-0x30);
	yc_float=(float)month;
	Gps_Refresh_Data(rtu_chan_num, month, yc_float, MONTH_TYPE);

	day   = (buff[11]-0x30)*10+ (buff[12]-0x30);
	yc_float=(float)day;
	Gps_Refresh_Data(rtu_chan_num, day, yc_float, DAY_TYPE);

	hour  = (buff[13]-0x30)*10+ (buff[14]-0x30);
	yc_float=(float)hour;
	Gps_Refresh_Data(rtu_chan_num, hour, yc_float, HOUR_TYPE);

	min   = (buff[15]-0x30)*10+ (buff[16]-0x30);
	yc_float=(float)min;
	Gps_Refresh_Data(rtu_chan_num, min, yc_float, MINUTE_TYPE);

	sec   = (buff[17]-0x30)*10+ (buff[18]-0x30);
	yc_float=(float)sec;
	Gps_Refresh_Data(rtu_chan_num, sec, yc_float, SECOND_TYPE);

	yc_int=(buff[20]-0x30)*10+ (buff[21]-0x30);
	if( yc_int<0 )
		yc_int=0;
	sum=(buff[22]-0x30)*1000+ (buff[23]-0x30)*100+(buff[24]-0x30)*10+(buff[25]-0x30);
	zb_value=(float)yc_int+(float)sum/10000.0;
	printf(("ldldlldl!!!!!!!!!!!!!! 时间 : %d年%d月%d日%d时%d分%d秒\n"),year,month,day,hour,min,sec);
	//周波处理
	yc_int=(int)zb_value;
	Gps_Refresh_Data(rtu_chan_num, yc_int, zb_value, FREQUENCY_TYPE);
	printf(_("!!!!!!!!!!!!!! 系统周波 :  %f \n"),zb_value);

	yc_float=zb_value-50.0;
	yc_int=(int)yc_float;
	Gps_Refresh_Data(rtu_chan_num, yc_int, yc_float, FREQ_DIFF_TYPE);

	if( (zb_value<40.0) || (zb_value>60.0) )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_error++;
	}
	else if( (fabs(zb_value-freq))>0.0005 )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_total++;
	}
	freq=zb_value;

	duty=SysManTabPtr->fcheck[rtu_chan_num].if_duty;
	duty &= 0x07;
	if( duty!=DUTY )
	{
		return(3);
	}

	if( (year<0) || (month<0) || (day<0) || (hour<0) || (min<0) || (sec<0) )
		return(-4);

	time(&machtime);
	phytime_t=localtime(&machtime);
	phytime_t->tm_year      = year-1900;
	phytime_t->tm_mon       = month-1;
	phytime_t->tm_mday      = day;
	phytime_t->tm_hour      = hour;
	phytime_t->tm_min       = min;
	phytime_t->tm_sec       = sec;
	gpstime=mktime(phytime_t);
	net_gps_time = htonl(gpstime);

	retcode=sendto(send_sock, (char *)&net_gps_time, sizeof(net_gps_time), 0,
		(struct sockaddr *)&fdst_sendto, sizeof(fdst_sendto));
	printf("!!!!!!!!!!!!!!!!!!! NARI GPS sendto===%d\n",retcode);

	return(retcode);
}

//*****************************************************************************************

void Gps_Refresh_Data(int rtu_chan_num, int value1, float value2, int type)
{
int	offset;

	switch( type )
	{
		case	FREQUENCY_TYPE:
			offset=0;
			break;
		case	FREQ_DIFF_TYPE:
			offset=1;
			break;
		case	TIME_DIFF_TYPE:
			offset=2;
			break;
		case	ELECTRO_HOUR_TYPE:
			offset=3;
			break;
		case	ELECTRO_MINUTE_TYPE:
			offset=4;
			break;
		case	ELECTRO_SECOND_TYPE:
			offset=5;
			break;
		case	DAY_TYPE:
			offset=6;
			break;
		case	HOUR_TYPE:
			offset=7;
			break;
		case	MINUTE_TYPE:
			offset=8;
			break;
		case	SECOND_TYPE:
			offset=9;
			break;
		case	YEAR_TYPE:
			offset=10;
			break;
		case	MONTH_TYPE:
			offset=11;
			break;
		case	LAT_DEGREE_TYPE:
			offset=12;
			break;
		case	LAT_MINUTE_TYPE:
			offset=13;
			break;
		case	LONG_DEGREE_TYPE:
			offset=14;
			break;
		case	LONG_MINUTE_TYPE:
			offset=15;
			break;
		case	TIME_QUALITY_TYPE:
			offset=16;
			break;
		default:
			offset=17;
			break;
	}

	Fdb_io.Write_DB_Value(YC_TYPE, rtu_chan_num, offset, FIELD_YC_RAW,
					(unsigned char*)&value1);
	Fdb_io.Write_DB_Value(YC_TYPE, rtu_chan_num, offset, FIELD_YC_INTEGER,
					(unsigned char*)&value1);
	Data.YCDataProcess(rtu_chan_num, offset, value2);
}

int IsValidNumber(unsigned char ch)
{
	if( (ch>='0') && (ch<='9') )
		return(1);
	printf(_("报文格式错! %c \n"), ch);
	return(0);
}

float GetFloat(unsigned char c_1, unsigned char c_2, unsigned char c_3, unsigned char c_4, unsigned char c_5)
{
float	f_val;
char	c1, c2, c3, c4, c5;

	c1 = c_1 - 0x30;
	if( c1==-16 )	c1=0;
	c2 = c_2 - 0x30;
	if( c2==-16 )	c2=0;
	c3 = c_3 - 0x30;
	if( c3==-16 )	c3=0;
	c4 = c_4 - 0x30;
	if( c4==-16 )	c4=0;
	c5 = c_5 - 0x30;
	if( c5==-16 )	c5=0;

	// -2 means '.'
	if( c1==-2 )
	{
		f_val = (float)(c2*1000 + c3*100 + c4*10 + c5);
		f_val = f_val / 10000.00;
	}
	else if (c2 == -2)
	{
		f_val = (float)(c1*1000 + c3*100 + c4*10 + c5);
		f_val = f_val / 1000.00;
	}
	else if (c3 == -2)
	{
		f_val = (float)(c1*1000 + c2*100 + c4*10 + c5);
		f_val = f_val / 100.00;
	}
	else if (c4 == -2)
	{
		f_val = (float)(c1*1000 + c2*100 + c3*10 + c5);
		f_val = f_val / 10.00;
	}
	else if (c5 == -2)
	{
		f_val = (float)(c1*1000 + c2*100 + c3*10 + c4);
		f_val = f_val / 1.00;
	}
	else
	{
		f_val = (float)(c1*10000 + c2*1000 + c3*100 + c4*10 + c5);
	}
	return f_val;
}

int BcdToInt(unsigned char ch)
{
int	t1, t2, ret;

	t1=(ch>>4)&0x0f;
	t2=ch&0x0f;
	if( t1>9 || t2>9 )
		return(-1);

	ret=t1*10+t2;
	return(ret);
}

int CDKW_Gps_Interface(int rtu_chan_num, char *buff, int len)
{
	int		year, month, day, hour, min, sec, hour1, min1, sec1, yc_int, ret;
	unsigned char	duty, c1, c2, c3, c4, c5, c6;
	float		zb_value, yc_flo;
	//add for freq_diff time_diff
	unsigned char c7;
	float freq_diff;
	int time_diff;
	//add end
	struct	tm	phytime_t;
	time_t		gpstime;
	int		net_gps_time;
	//modify for trans
	//Gps_Report_Disp.DispStringWithType(rtu_chan_num, &buff[0], Disp_Recv_Type, len, (char)0);//modify for trans
	Iec_Disp.disp_flag=0;
	Iec_Disp.disp_count=0;	
	Iec_Disp.disp_flag = Gps_Report_Disp.Flag(rtu_chan_num);
	//modify end
	if( len<36 ) 
	{
		printf("recv buffer less 36\n");
		return(-1);
	}

	if( buff[0]!='#' )
	{
		printf("recv head is wrong! \n");
		return(-2);
	}
	/*
	if(  buff[19]!='*'  )
	{
	printf("recv Freq head is wrong! \n");
	return(-3);
	}

	if( !(IsValidNumber(buff[5])
	&& IsValidNumber(buff[6]) && IsValidNumber(buff[7])
	&& IsValidNumber(buff[8]) && IsValidNumber(buff[9])
	&& IsValidNumber(buff[10]) && IsValidNumber(buff[11])
	&& IsValidNumber(buff[12]) && IsValidNumber(buff[13])
	&& IsValidNumber(buff[14]) && IsValidNumber(buff[15])
	&& IsValidNumber(buff[17]) && IsValidNumber(buff[18])
	&& IsValidNumber(buff[20]) && IsValidNumber(buff[21])
	&& IsValidNumber(buff[22]) && IsValidNumber(buff[23])
	&& IsValidNumber(buff[24]) && IsValidNumber(buff[25])
	&& IsValidNumber(buff[26]) && IsValidNumber(buff[27])
	&& IsValidNumber(buff[28]) && IsValidNumber(buff[29])
	&& IsValidNumber(buff[30]) && IsValidNumber(buff[31]) 
	&& IsValidNumber(buff[40]) && IsValidNumber(buff[41]) //add for freq_diff time_diff
	&& IsValidNumber(buff[43]) && IsValidNumber(buff[44]) //add for freq_diff time_diff
	&& IsValidNumber(buff[45]) && IsValidNumber(buff[53]) //add for freq_diff time_diff
	&& IsValidNumber(buff[54]) && IsValidNumber(buff[55]) //add for freq_diff time_diff
	&& IsValidNumber(buff[56]) && IsValidNumber(buff[57])) ) //add for freq_diff time_diff 
	*/
	if( !(IsValidNumber(buff[5])
		&& IsValidNumber(buff[6]) && IsValidNumber(buff[7])
		&& IsValidNumber(buff[8]) && IsValidNumber(buff[9])
		&& IsValidNumber(buff[10]) && IsValidNumber(buff[11])
		&& IsValidNumber(buff[12]) && IsValidNumber(buff[13])
		&& IsValidNumber(buff[14]) && IsValidNumber(buff[15])
		&& IsValidNumber(buff[16])
		&& IsValidNumber(buff[17]) && IsValidNumber(buff[18])))
	{
		printf("wrong format, time \n");
		return(-4);
	}
	/*
	//add for freq_diff time_diff
	if(  buff[39] != '-' &&  buff[39] != '+' )
	{
	printf("recv freq diff head is wrong! \n");
	return(-5);
	}	

	if(  buff[52] != '-' &&  buff[52] != '+' )
	{
	printf("recv time diff head is wrong! \n");
	return(-6);
	}	
	//add end
	*/
	//add for trans
	if( Iec_Disp.disp_flag != 0 )
	{
		strncpy(&Iec_Disp.disp_buff[Iec_Disp.disp_count],buff,5);
		Iec_Disp.disp_count += 5;
		Iec_Disp.disp_buff[Iec_Disp.disp_count] = ' ';
		Iec_Disp.disp_count += 1;		
	}	
	//add end

	c1 = buff[5] - 0x30;
	c2 = buff[6] - 0x30;
	c3 = buff[7] - 0x30;
	c4 = buff[8] - 0x30;
	year = c1*1000 + c2*100 + c3*10 + c4;
	//add for trans
	if( Iec_Disp.disp_flag != 0 )
	{
		CDKW_Disp_Report(&buff[5],4,YEAR_TYPE,year,(float)year);
	}
	//add end

	c1 = buff[9] - 0x30;
	c2 = buff[10] - 0x30;
	month = c1*10 + c2;
	//add for trans	
	if( Iec_Disp.disp_flag != 0 )
	{
		CDKW_Disp_Report(&buff[9],2,MONTH_TYPE,month,(float)month);
	}
	//add end

	c1 = buff[11] - 0x30;
	c2 = buff[12] - 0x30;
	day = c1*10 + c2;
	//add for trans	
	if( Iec_Disp.disp_flag != 0 )
	{
		CDKW_Disp_Report(&buff[11],2,DAY_TYPE,day,(float)day);
	}
	//add end

	c1 = buff[13] - 0x30;
	c2 = buff[14] - 0x30;
	hour = c1*10 + c2;
	//add for trans	
	if( Iec_Disp.disp_flag != 0 )
	{
		CDKW_Disp_Report(&buff[13],2,HOUR_TYPE,hour,(float)hour);	
	}
	//add end

	c1 = buff[15] - 0x30;
	c2 = buff[16] - 0x30;
	min = c1*10 + c2;
	//add for trans	
	if( Iec_Disp.disp_flag != 0 )
	{
		CDKW_Disp_Report(&buff[15],2,MINUTE_TYPE,min,(float)min);	
	}
	//add end

	c1 = buff[17] - 0x30;
	c2 = buff[18] - 0x30;
	sec = c1*10 + c2;
	//add for trans	
	if( Iec_Disp.disp_flag != 0 )
	{
		CDKW_Disp_Report(&buff[17],2,SECOND_TYPE,sec,(float)sec);	
	}
	//add end
	printf("!!!!!!!!!!!!!! 时间 : %d年%d月%d日%d时%d分%d秒\n",year,month,day,hour,min,sec);

	yc_flo=(float)year;
	Gps_Refresh_Data(rtu_chan_num, year, yc_flo, YEAR_TYPE);
	yc_flo=(float)month;
	Gps_Refresh_Data(rtu_chan_num, month, yc_flo, MONTH_TYPE);
	yc_flo=(float)day;
	Gps_Refresh_Data(rtu_chan_num, day, yc_flo, DAY_TYPE);
	yc_flo=(float)hour;
	Gps_Refresh_Data(rtu_chan_num, hour, yc_flo, HOUR_TYPE);
	yc_flo=(float)min;
	Gps_Refresh_Data(rtu_chan_num, min, yc_flo, MINUTE_TYPE);
	yc_flo=(float)sec;
	Gps_Refresh_Data(rtu_chan_num, sec, yc_flo, SECOND_TYPE);

	SysManTabPtr->fcheck[rtu_chan_num].current_total+=2;
	if( Iec_Disp.disp_flag != 0 )
	{
		Gps_Report_Disp.DispStringWithType(rtu_chan_num, Iec_Disp.disp_buff, Disp_Recv_Type, Iec_Disp.disp_count, (char)0);
	}

#if 0
	c1 = buff[20] - 0x30;
	c2 = buff[21] - 0x30;
	c3 = buff[22] - 0x30;
	c4 = buff[23] - 0x30;
	c5 = buff[24] - 0x30;
	c6 = buff[25] - 0x30;
	zb_value = c1*100000.0+c2*10000.0+c3*1000.0+c4*100.0+c5*10.0+c6;
	zb_value = zb_value/10000.0;
	printf("!!!!!!!!!!!!!!!!!!!! 频率 : %2.4fHz\n",zb_value);
	yc_int=(int)zb_value;
	Gps_Refresh_Data(rtu_chan_num, yc_int, zb_value, FREQUENCY_TYPE);
	//add for trans	
	if( Iec_Disp.disp_flag != 0 )
	{
		CDKW_Disp_Report(&buff[19],7,FREQUENCY_TYPE,(int)zb_value,zb_value);//6->7
	}
	//add end

	if( (zb_value<40.0) || (zb_value>60.0) )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_error+=30;
	}
	else if( (fabs(zb_value-freq))>0.0005 )
	{
		SysManTabPtr->fcheck[rtu_chan_num].current_total+=2;
	}
	freq=zb_value;

	c1 = buff[26] - 0x30;
	c2 = buff[27] - 0x30;
	hour1 = c1*10 + c2;

	//add for trans	
	if( Iec_Disp.disp_flag != 0 )
	{
		CDKW_Disp_Report(&buff[26],2,ELECTRO_HOUR_TYPE,hour1,(float)hour1);	
	}
	//add end
	c1 = buff[28] - 0x30;
	c2 = buff[29] - 0x30;
	min1 = c1*10 + c2;
	//add for trans	
	if( Iec_Disp.disp_flag != 0 )
	{
		CDKW_Disp_Report(&buff[28],2,ELECTRO_MINUTE_TYPE,min1,(float)min1);	
	}
	//add end

	c1 = buff[30] - 0x30;
	c2 = buff[31] - 0x30;
	sec1 = c1*10 + c2;
	//add for trans	
	if( Iec_Disp.disp_flag != 0 )
	{
		CDKW_Disp_Report(&buff[30],2,ELECTRO_SECOND_TYPE,sec1,(float)sec1);	
		/* 屏蔽 for  freq_diff time_diff
		strncpy(&Iec_Disp.disp_buff[Iec_Disp.disp_count],&buff[32],len-32);
		Iec_Disp.disp_count += (len-32);
		Iec_Disp.disp_buff[Iec_Disp.disp_count] = ' ';
		Iec_Disp.disp_count += 1;

		Gps_Report_Disp.DispStringWithType(rtu_chan_num, Iec_Disp.disp_buff, Disp_Recv_Type, Iec_Disp.disp_count, (char)0);
		*/ 	
	}	

	//add end


	printf("!!!!!!!!!!!!!! 工频钟(电钟) : %d时%d分%d秒\n",hour1,min1,sec1);

	yc_flo=(float)hour1;
	Gps_Refresh_Data(rtu_chan_num, hour1, yc_flo, ELECTRO_HOUR_TYPE);
	yc_flo=(float)min1;
	Gps_Refresh_Data(rtu_chan_num, min1, yc_flo, ELECTRO_MINUTE_TYPE);
	yc_flo=(float)sec1;
	Gps_Refresh_Data(rtu_chan_num, sec1, yc_flo, ELECTRO_SECOND_TYPE);

	//add for freq_diff time_diff 

	if( Iec_Disp.disp_flag != 0 )
	{
		strncpy(&Iec_Disp.disp_buff[Iec_Disp.disp_count],&buff[32],7);
		Iec_Disp.disp_count += 7;
		Iec_Disp.disp_buff[Iec_Disp.disp_count] = ' ';
		Iec_Disp.disp_count += 1;		
	}

	c1 = buff[39];
	c2 = buff[40] - 0x30;
	c3 = buff[41] - 0x30;
	c5 = buff[43] - 0x30;
	c6 = buff[44] - 0x30;
	c7 = buff[45] - 0x30;
	freq_diff = c2 * 10.0 + c3 * 1.0 + c5 * 0.1 + c6 * 0.01 + c7 * 0.001;
	if( c1 == '-' )
		freq_diff = -1.0 * freq_diff;
	Gps_Refresh_Data(rtu_chan_num, (int)freq_diff, freq_diff, FREQ_DIFF_TYPE);

	if( Iec_Disp.disp_flag != 0 )
	{
		CDKW_Disp_Report(&buff[39],7,FREQ_DIFF_TYPE,(int)freq_diff,freq_diff);		
	}

	if( Iec_Disp.disp_flag != 0 )
	{
		strncpy(&Iec_Disp.disp_buff[Iec_Disp.disp_count],&buff[46],6);
		Iec_Disp.disp_count += 6;
		Iec_Disp.disp_buff[Iec_Disp.disp_count] = ' ';
		Iec_Disp.disp_count += 1;		
	}

	c1 = buff[52];
	c2 = buff[53] - 0x30;
	c3 = buff[54] - 0x30;
	c4 = buff[55] - 0x30;
	c5 = buff[56] - 0x30;
	c6 = buff[57] - 0x30;	

	time_diff = c2*10000 + c3 * 1000 + c4 * 100 + c5 * 10 + c6;	
	if( c1 == '-' )
	{
		time_diff = -1 * time_diff;
	}
	Gps_Refresh_Data(rtu_chan_num, time_diff, (float)time_diff, TIME_DIFF_TYPE);
	if( Iec_Disp.disp_flag != 0 )
	{
		CDKW_Disp_Report(&buff[52],6,TIME_DIFF_TYPE,time_diff, (float)time_diff);
		strncpy(&Iec_Disp.disp_buff[Iec_Disp.disp_count],&buff[58],len-58);
		Iec_Disp.disp_count += (len-58);
		Iec_Disp.disp_buff[Iec_Disp.disp_count] = ' ';
		Iec_Disp.disp_count += 1;

		Gps_Report_Disp.DispStringWithType(rtu_chan_num, Iec_Disp.disp_buff, Disp_Recv_Type, Iec_Disp.disp_count, (char)0);

	} 
	//add end	
#endif	
	duty = 0;
	duty=SysManTabPtr->fcheck[rtu_chan_num].if_duty;
	duty &= 0x07;
	if( duty!=DUTY )
	{
		return(4);
	}

	phytime_t.tm_year	= year-1900;
	phytime_t.tm_mon	= month-1;
	phytime_t.tm_mday	= day;
	phytime_t.tm_hour	= hour;
	phytime_t.tm_min	= min;
	phytime_t.tm_sec	= sec;
	gpstime=mktime(&phytime_t);
	net_gps_time = htonl(gpstime);

	ret=sendto(send_sock, (char *)&net_gps_time, sizeof(net_gps_time), 0,
		(struct sockaddr *)&fdst_sendto, sizeof(fdst_sendto));
	printf("!!!!!!!!!!!!!!!!!!! sendto===%d\n",ret);

	return(ret);
}

void CDKW_Disp_Report(char *buff,int len,int type,int i_value,float f_value)
{
 int	offset;
 char tmp_disp[64];
 strncpy(&Iec_Disp.disp_buff[Iec_Disp.disp_count],buff,len);
 Iec_Disp.disp_count += len;
 
 switch( type )
 {
		case	FREQUENCY_TYPE:
			offset=0;
			sprintf(tmp_disp,"<%d:%f 频率>",offset,f_value);
			break;
		case	FREQ_DIFF_TYPE:
			offset=1;
			sprintf(tmp_disp,"<%d:%f 频差>",offset,f_value);
			break;
		case	TIME_DIFF_TYPE:
			offset=2;
			sprintf(tmp_disp,"<%d:%f 时差>",offset,f_value);
			break;
		case	ELECTRO_HOUR_TYPE:
			offset=3;
			sprintf(tmp_disp,"<%d:%d 电钟时>",offset,i_value);
			break;
		case	ELECTRO_MINUTE_TYPE:
			offset=4;
			sprintf(tmp_disp,"<%d:%d 电钟分>",offset,i_value);
			break;
		case	ELECTRO_SECOND_TYPE:
			offset=5;
			sprintf(tmp_disp,"<%d:%d 电钟秒>",offset,i_value);
			break;
		case	DAY_TYPE:
			offset=6;
			sprintf(tmp_disp,"<%d:%d 日>",offset,i_value);
			break;
		case	HOUR_TYPE:
			offset=7;
			sprintf(tmp_disp,"<%d:%d 时>",offset,i_value);
			break;
		case	MINUTE_TYPE:
			offset=8;
			sprintf(tmp_disp,"<%d:%d 分>",offset,i_value);
			break;
		case	SECOND_TYPE:
			offset=9;
			sprintf(tmp_disp,"<%d:%d 秒>",offset,i_value);
			break;
		case	YEAR_TYPE:
			offset=10;
			sprintf(tmp_disp,"<%d:%d 年>",offset,i_value);
			break;
		case	MONTH_TYPE:
			offset=11;
			sprintf(tmp_disp,"<%d:%d 月>",offset,i_value);
			break;
		case	LAT_DEGREE_TYPE:
			offset=12;
			sprintf(tmp_disp,"<%d:%f 纬度度>",offset,f_value);
			break;
		case	LAT_MINUTE_TYPE:
			offset=13;
			sprintf(tmp_disp,"<%d:%f 纬度分>",offset,f_value);
			break;
		case	LONG_DEGREE_TYPE:
			offset=14;
			sprintf(tmp_disp,"<%d:%f 经度度>",offset,f_value);
			break;
		case	LONG_MINUTE_TYPE:
			offset=15;
			sprintf(tmp_disp,"<%d:%f 经度分>",offset,f_value);
			break;
		case	TIME_QUALITY_TYPE:
			offset=16;
			sprintf(tmp_disp,"<%d:%d 时间质量>",offset,i_value);
			break;
		default:
			break;
 }		

 strncpy(&Iec_Disp.disp_buff[Iec_Disp.disp_count],tmp_disp,strlen(tmp_disp));
 Iec_Disp.disp_count += strlen(tmp_disp);
 Iec_Disp.disp_buff[Iec_Disp.disp_count] = ' ';
 Iec_Disp.disp_count += 1;	
}
