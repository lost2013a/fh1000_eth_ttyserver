#ifndef GPSINTERFACE_H
#define GPSINTERFACE_H
#include "gpscomdef.h"
/*���ܣ� gps�����ǿͻ���ͨ�Ŷ�̬�⵼���ӿڶ���
 ��д;   ������
 ʱ��:   2013-09-11
*/


//��̬���еĸ�����Ϣͨ�������Ϣ�ص�����
typedef int (*LPGPSINTERFACE_DLL)(void* ptr,GPSCOMDEF_UDP_COM *cmd_struct);
//���ļ���
typedef int (*LPGPSINTERFACE_MSG)(void *ptr, unsigned short stationid,  unsigned int type, void *data, int len, int direct, const char *from, const char *to);
//����������Ϣ��ʾ
typedef int (*LPGPSINTERFACE_TIP)(void *ptr,unsigned short stationid,  char *tip, int maxtype, int mintype);

//��ʼ��ʱ�Ĵ��붯̬���ͨ�Ų���
typedef struct tagGpsInterface_COM
{
	 unsigned short stationid;      //վ��id
     void *ptr;                     //�����ָ��
	 LPGPSINTERFACE_DLL fnInfoback;  
     LPGPSINTERFACE_MSG fnMsg;
     LPGPSINTERFACE_TIP fnTip;
	 char szDriver[32];  //����
	 char szHostName[48]; //������
	 char szDbName[32];    //���ݿ�����
	 char szUser[32];      //�û���
	 char szPasswd[32];    //���� 
	 unsigned short pgid;         //������

}GPSINTERACE_COM;


//��̬�����ĺ���
typedef int (*LPGPSINTERFACE_INITCOM)(GPSINTERACE_COM * pCom);

//���������̬���еĺ���
typedef int (*LPGPSINTERFACE_SENDCMD)(unsigned short stationid, GPSCOMDEF_UDP_COM *cmd_struct);

typedef int (*LPGPSINTERFACE_RELEASE)(unsigned short stationid);

//����ӿ����ڷ����
typedef int (*LPGPSINTERFACE_NEWINFO)(unsigned char *pASDU,  unsigned short len);

BEGIN_C_DECLS
/* ������������ʼ��������վ��ͨ����
���������
          stationid  վ�ı��
����ֵ��
        0   ---- �ɹ�
        1   -----ʧ��
*/
COM_DATA_EXPORT int  gpsinterface_initcom(GPSINTERACE_COM * pCom); 
/* ��������:��������
���������
      stationid   վ�ı��
	  cmd_struct  ������Ϣ�Ľṹָ��
����ֵ��
        0   ---- �ɹ�
        1   -----ʧ��
*/
COM_DATA_EXPORT int  gpsinterface_sendcmd(unsigned short stationid, GPSCOMDEF_UDP_COM *cmd_struct);
/*���ܣ��ͷ����վ��ȫ������Դ
 ������
        stationid  վ�ı��

*/
COM_DATA_EXPORT int  gpsinterface_release(unsigned short stationid);
/* ���ܣ��������յ����ݴ��䵽����̬���У�Ȼ���䵽��վ
  ������
        pASDU ���ݻ���
		len   ���ĳ���
*/
COM_DATA_EXPORT int  gpsinterface_newinfo(unsigned char *pASDU,  unsigned short len);

END_C_DECLS


#endif;
