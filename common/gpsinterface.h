#ifndef GPSINTERFACE_H
#define GPSINTERFACE_H
#include "gpscomdef.h"
/*功能： gps测试仪客户端通信动态库导出接口定义
 编写;   陈自勇
 时间:   2013-09-11
*/


//动态库中的各种信息通过这个信息回调函数
typedef int (*LPGPSINTERFACE_DLL)(void* ptr,GPSCOMDEF_UDP_COM *cmd_struct);
//报文监视
typedef int (*LPGPSINTERFACE_MSG)(void *ptr, unsigned short stationid,  unsigned int type, void *data, int len, int direct, const char *from, const char *to);
//程序运行信息显示
typedef int (*LPGPSINTERFACE_TIP)(void *ptr,unsigned short stationid,  char *tip, int maxtype, int mintype);

//初始化时的传入动态库的通信参数
typedef struct tagGpsInterface_COM
{
	 unsigned short stationid;      //站的id
     void *ptr;                     //保存的指针
	 LPGPSINTERFACE_DLL fnInfoback;  
     LPGPSINTERFACE_MSG fnMsg;
     LPGPSINTERFACE_TIP fnTip;
	 char szDriver[32];  //驱动
	 char szHostName[48]; //机器名
	 char szDbName[32];    //数据库名称
	 char szUser[32];      //用户名
	 char szPasswd[32];    //密码 
	 unsigned short pgid;         //程序编号

}GPSINTERACE_COM;


//动态导出的函数
typedef int (*LPGPSINTERFACE_INITCOM)(GPSINTERACE_COM * pCom);

//发送命令到动态库中的函数
typedef int (*LPGPSINTERFACE_SENDCMD)(unsigned short stationid, GPSCOMDEF_UDP_COM *cmd_struct);

typedef int (*LPGPSINTERFACE_RELEASE)(unsigned short stationid);

//这个接口用于服务端
typedef int (*LPGPSINTERFACE_NEWINFO)(unsigned char *pASDU,  unsigned short len);

BEGIN_C_DECLS
/* 功能描述，初始化连接子站的通信类
输入参数：
          stationid  站的编号
返回值：
        0   ---- 成功
        1   -----失败
*/
COM_DATA_EXPORT int  gpsinterface_initcom(GPSINTERACE_COM * pCom); 
/* 功能描述:发送命令
输入参数：
      stationid   站的编号
	  cmd_struct  发送信息的结构指针
返回值：
        0   ---- 成功
        1   -----失败
*/
COM_DATA_EXPORT int  gpsinterface_sendcmd(unsigned short stationid, GPSCOMDEF_UDP_COM *cmd_struct);
/*功能：释放这个站的全部的资源
 参数：
        stationid  站的编号

*/
COM_DATA_EXPORT int  gpsinterface_release(unsigned short stationid);
/* 功能：将串口收的数据传输到服务动态库中，然后传输到主站
  参数：
        pASDU 数据缓冲
		len   报文长度
*/
COM_DATA_EXPORT int  gpsinterface_newinfo(unsigned char *pASDU,  unsigned short len);

END_C_DECLS


#endif;
