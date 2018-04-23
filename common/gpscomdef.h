#ifndef GPSCOMDEF_H
#define GPSCOMDEF_H
//#include <QtGlobal>
/*功能: gps测试仪内部通信规约定义
 编写： 陈自勇  
 时间： 2013-09-10
*/
//全部的结构按字节对齐


#ifdef __cplusplus
#define BEGIN_C_DECLS  extern "C" {
#define END_C_DECLS      } 
#else
#define BEGIN_C_DECLS
#define END_C_DECLS 
#endif


#if defined(WIN32) 
#define COM_DATA_EXPORT __declspec(dllexport)
#else
#define COM_DATA_EXPORT
#endif


#define  GPSCOMDEF_STATE_COUNT  82
#define  GPSCOMDEF_VALUE_COUNT  12 


//状态量组为4组，1开始 

//状态量组为4组，1开始 

static char *gpsState[14] = 
{
  " ",
  "第1路外部时源（BD）信号状态",
  "第2路外部时源（GPS）信号状态",
  "第3部时源（根据实际数量配置）信号状态",
  "天线状态（BD）",
  "天线状态（GPS）",
  "P1天线状态（根据实际数量配置）",
  "第1路接收模块（BD）状态",
  "第2路接收模块(GPS)状态",
  "第n路接收模块(根据实际数量配置)状态",
  "时间跳变侦测状态",
  "晶振驯服状态",
  "初始化状态",
  "电源模块状态"
};

//时实量5组  1开始
static char *gpsRealValue[2] =
{
   " ","时间源选择结果"
};

#define   GPSCOMDEF_GET_EQUIP_DEFINE       1  //调全部的配置
#define   GPSCOMDEF_GET_OVERALL   2  //总召唤
#define   GPSCOMDEF_GET_SETTING   3  //定值
#define   GPSCOMDEF_GET_ANALOG    4  //模拟量
#define   GPSCOMDEF_GET_REALVALUE 5  //事实值
#define   GPSCOMDEF_GET_TIME      6   //时间测量值
#define   GPSCOMDEF_SET_EVENT     7  //发送测试时间 


#define  GPSCOMDEF_SHOW_INFO       20   //显示信息换回结果用
#define  GPSCOMDEF_SHOW_EVENT      21   //界面显示事实信息用
#define  GPSCOMDEF_COM_START       31   //开始通信
#define  GPSCOMDEF_COM_STOP        32   //停止通信
#define  GPSCOMDEF_COM_UPDATE      33   //更新通信用
#define  GPSCOMDEF_NOTIFY          34   //跟新通信状态用
#define  GPSCOMDEF_CMD_DATA        35   //命令数据发送到客户端界面
#define  GPSCOMDEF_TEST_DATA       36   //测量时间的数据
#define  GPSCOMDEF_RESTART_PG      37   //重新启动程序


#define   GPSCOMDEF_CMD_FAILE              0     //命令失败
#define   GPSCOMDEF_CMD_OK                 1     //命令执行成功 
#define   GPSCOMDEF_CMD_ERROR              2     //如果命令服务中心，找不到对应的前置机，就用这个命令回答
#define   GPSCOMDEF_CMD_INFOR              3     //显示命令的执行信息
#define   GPSCOMDEF_CMD_TIMEOUT            4     //命令超时   
#define   GPSCOMDEF_CMD_CANCEL             5     //命令取消    

//数据的类型定义
#define  GPSCOMDEF_DATA_NULL               0   //无效的类型
#define  GPSCOMDEF_DATA_SETTING            1   //定值
#define  GPSCOMDEF_DATA_REALVALE           2   //实时量
#define  GPSCOMDEF_DATA_ANALOG             3   //模拟量
#define  GPSCOMDEF_DATA_STATE              4   //状态量       
#define  GPSCOMDEF_DATA_COM_STATE          5   //通信状态


//监视动态库的运行信息
#define  GPSCOMDEF_MSG                     1   //报文类型
#define  GPSCOMDEF_TIP                     2   //运行提示信息 

#define  GPSCOMDEF_TIP_CONNECT             1   //连接信息
#define  GPSCOMDEF_TIP_INFO                2   //提示信息


//数据类型定义
#define GPSCOMDEF_DATA_NULL                0  //不知道的类型
#define GPSCOMDEF_DATA_SPI                 1  //单点信息
#define GPSCOMDEF_DATA_DPI                 2  //双点信息
#define GPSCOMDEF_DATA_INT                 3  //整数 
#define GPSCOMDEF_DATA_FLOAT               4  //浮点
#define GPSCOMDEF_DATA_ASCII               5  //字符串


#pragma pack (push, 1) 
typedef struct  _gpscomdef_udp_com
{
   unsigned short   from;                 //发出程序编号 
   unsigned short   to;                   //接收报文的程序编号 如果希望全部的程序接收 0xffff
   unsigned short   stationid;   //站的编号
   unsigned short   comcode;      //通信代码
   char    szData[1024];

}GPSCOMDEF_UDP_COM;

//客户端发送到前置机的命令结构
typedef struct _gpscomdef_cmd
{
   unsigned int cmdindex;    //客户端的命令索引
   unsigned short stationid;   //站号


}GPSCOMDEF_CMD;


//将执行的结果返回的客户端
typedef struct _gpscomdef_client
{
    unsigned int  cmdindex;    //客户端发送的命令索引
	unsigned char    cmdResult;   //执行命令信息  
    char    szInfo[256]; //结果代执行信息

}GPSCOMDEF_CLIENT;


//传输串口传递的数据
typedef struct _gpscomdef_state
{
   unsigned char flag;       //有效标志  
   unsigned char grp;
   unsigned char itm;
   unsigned char state; //0--正常 1--异常 


}GPSCOMDEF_STATE;
typedef struct _gpscomdef_value
{
   unsigned char   grp;         //事实量组号
   unsigned char   itm;         //条目号 
   unsigned short  value; 

}GPSCOMDEF_VALE;

typedef struct _gpscomdef_data
{
      unsigned short  year;   //年
      unsigned short  month;   //月
	  unsigned short  day;     //日
      unsigned short  hour;     //时间
	  unsigned short  minute;  //分
	  unsigned short  sec;     //秒
      unsigned int    msec;   //毫秒
      unsigned char   manclock; //主时钟
	  unsigned char   statecount; //状态位的数量
      GPSCOMDEF_STATE state[GPSCOMDEF_STATE_COUNT];
	  unsigned char   valuecount;
	  GPSCOMDEF_VALE  value[GPSCOMDEF_VALUE_COUNT];
	

}GPSCOMDEF_DATA;
//发送信息到客户端显示的数据
typedef struct _gpscomdef_event
{
    unsigned  char infotype; //数据类型，状态量，、时实量
	unsigned  short grp;
    unsigned  short itm;
	char      szSrc[128];    //站名
	char      szName[128];   //点表定义
    char      szUnit[10];    //单位
    char      szValue[64];   //值
	char      szDispValue[64]; //显示的值
	char      szTime[32];
	unsigned  q;          //品质因数(61850使用)

}GPSCOMDEF_EVENT;

//召唤是发送的数据到界面,数据库中不保存数据
//GPSCOMDEF_CMD_DATA
//szData中   1个字节为GPSCOMDEF_SENDDATA的个数，下面为这个结构连续排列

typedef struct _gpscomdef_senddata
{
	unsigned  char infotype; //数据类型，状态量，、时实量
	unsigned  short grp;
    unsigned  short itm;
    char      szValue[32]; 	

}GPSCOMDEF_SENDDATA;
//内部传输的测量值结构GPSCOMDEF_TEST_DATA
typedef struct _gpscomdef_testvalue
{
	unsigned long long   t0;
	unsigned long long   t1;
	unsigned long long   t2;
	unsigned long long   t3;
	char szTime[32];    //时间值的表示

}GPSCOMDEF_TESTVALUE;


//测试仪客户界面器发送事件信息到61850服务端
typedef struct _gpscomdef_testevent
{
   unsigned short grp;  //组号
   unsigned short itm;  //条目号
   unsigned int   val;  //值 0-
   unsigned short q;     //q
   unsigned char sztime[48]; //时间,如果时间为0，由服务提供模拟事件的时间
   char szcomment[48];       //备注，保留使用

}GPSCOMDEF_TESTEVENT;
/*使用说明:动态库导出函数 
COM_DATA_EXPORT int  gpsinterface_sendcmd(unsigned short stationid, GPSCOMDEF_UDP_COM *cmd_struct); 导出接口，发送数据到动态库中
if (cmd_struct->comcode == GPSCOMDEF_SET_EVENT)
{
     GPSCOMDEF_TESTEVENT *pEvent = (GPSCOMDEF_TESTEVENT*)cmd_struct->szData;
     pEvent->sztime 的长度为0，由服务端提供时间
	 switch(pEvent->itm)
	 {
	 case 1:  //对时信号状态   
	 {
	 
	 }
     break;
	 case 2: //对时服务状态
	 {
	 
	 
	 }
	 break;
	 case 3: //时间跳变侦测状态
	 {
	 
	 }
	 break;
     default:


	 break;
	 
	 }
	 
	 
	 

}

*/

#pragma pack(pop)

#endif

