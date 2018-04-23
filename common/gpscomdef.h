#ifndef GPSCOMDEF_H
#define GPSCOMDEF_H
//#include <QtGlobal>
/*����: gps�������ڲ�ͨ�Ź�Լ����
 ��д�� ������  
 ʱ�䣺 2013-09-10
*/
//ȫ���Ľṹ���ֽڶ���


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


//״̬����Ϊ4�飬1��ʼ 

//״̬����Ϊ4�飬1��ʼ 

static char *gpsState[14] = 
{
  " ",
  "��1·�ⲿʱԴ��BD���ź�״̬",
  "��2·�ⲿʱԴ��GPS���ź�״̬",
  "��3��ʱԴ������ʵ���������ã��ź�״̬",
  "����״̬��BD��",
  "����״̬��GPS��",
  "P1����״̬������ʵ���������ã�",
  "��1·����ģ�飨BD��״̬",
  "��2·����ģ��(GPS)״̬",
  "��n·����ģ��(����ʵ����������)״̬",
  "ʱ���������״̬",
  "����ѱ��״̬",
  "��ʼ��״̬",
  "��Դģ��״̬"
};

//ʱʵ��5��  1��ʼ
static char *gpsRealValue[2] =
{
   " ","ʱ��Դѡ����"
};

#define   GPSCOMDEF_GET_EQUIP_DEFINE       1  //��ȫ��������
#define   GPSCOMDEF_GET_OVERALL   2  //���ٻ�
#define   GPSCOMDEF_GET_SETTING   3  //��ֵ
#define   GPSCOMDEF_GET_ANALOG    4  //ģ����
#define   GPSCOMDEF_GET_REALVALUE 5  //��ʵֵ
#define   GPSCOMDEF_GET_TIME      6   //ʱ�����ֵ
#define   GPSCOMDEF_SET_EVENT     7  //���Ͳ���ʱ�� 


#define  GPSCOMDEF_SHOW_INFO       20   //��ʾ��Ϣ���ؽ����
#define  GPSCOMDEF_SHOW_EVENT      21   //������ʾ��ʵ��Ϣ��
#define  GPSCOMDEF_COM_START       31   //��ʼͨ��
#define  GPSCOMDEF_COM_STOP        32   //ֹͣͨ��
#define  GPSCOMDEF_COM_UPDATE      33   //����ͨ����
#define  GPSCOMDEF_NOTIFY          34   //����ͨ��״̬��
#define  GPSCOMDEF_CMD_DATA        35   //�������ݷ��͵��ͻ��˽���
#define  GPSCOMDEF_TEST_DATA       36   //����ʱ�������
#define  GPSCOMDEF_RESTART_PG      37   //������������


#define   GPSCOMDEF_CMD_FAILE              0     //����ʧ��
#define   GPSCOMDEF_CMD_OK                 1     //����ִ�гɹ� 
#define   GPSCOMDEF_CMD_ERROR              2     //�������������ģ��Ҳ�����Ӧ��ǰ�û��������������ش�
#define   GPSCOMDEF_CMD_INFOR              3     //��ʾ�����ִ����Ϣ
#define   GPSCOMDEF_CMD_TIMEOUT            4     //���ʱ   
#define   GPSCOMDEF_CMD_CANCEL             5     //����ȡ��    

//���ݵ����Ͷ���
#define  GPSCOMDEF_DATA_NULL               0   //��Ч������
#define  GPSCOMDEF_DATA_SETTING            1   //��ֵ
#define  GPSCOMDEF_DATA_REALVALE           2   //ʵʱ��
#define  GPSCOMDEF_DATA_ANALOG             3   //ģ����
#define  GPSCOMDEF_DATA_STATE              4   //״̬��       
#define  GPSCOMDEF_DATA_COM_STATE          5   //ͨ��״̬


//���Ӷ�̬���������Ϣ
#define  GPSCOMDEF_MSG                     1   //��������
#define  GPSCOMDEF_TIP                     2   //������ʾ��Ϣ 

#define  GPSCOMDEF_TIP_CONNECT             1   //������Ϣ
#define  GPSCOMDEF_TIP_INFO                2   //��ʾ��Ϣ


//�������Ͷ���
#define GPSCOMDEF_DATA_NULL                0  //��֪��������
#define GPSCOMDEF_DATA_SPI                 1  //������Ϣ
#define GPSCOMDEF_DATA_DPI                 2  //˫����Ϣ
#define GPSCOMDEF_DATA_INT                 3  //���� 
#define GPSCOMDEF_DATA_FLOAT               4  //����
#define GPSCOMDEF_DATA_ASCII               5  //�ַ���


#pragma pack (push, 1) 
typedef struct  _gpscomdef_udp_com
{
   unsigned short   from;                 //���������� 
   unsigned short   to;                   //���ձ��ĵĳ����� ���ϣ��ȫ���ĳ������ 0xffff
   unsigned short   stationid;   //վ�ı��
   unsigned short   comcode;      //ͨ�Ŵ���
   char    szData[1024];

}GPSCOMDEF_UDP_COM;

//�ͻ��˷��͵�ǰ�û�������ṹ
typedef struct _gpscomdef_cmd
{
   unsigned int cmdindex;    //�ͻ��˵���������
   unsigned short stationid;   //վ��


}GPSCOMDEF_CMD;


//��ִ�еĽ�����صĿͻ���
typedef struct _gpscomdef_client
{
    unsigned int  cmdindex;    //�ͻ��˷��͵���������
	unsigned char    cmdResult;   //ִ��������Ϣ  
    char    szInfo[256]; //�����ִ����Ϣ

}GPSCOMDEF_CLIENT;


//���䴮�ڴ��ݵ�����
typedef struct _gpscomdef_state
{
   unsigned char flag;       //��Ч��־  
   unsigned char grp;
   unsigned char itm;
   unsigned char state; //0--���� 1--�쳣 


}GPSCOMDEF_STATE;
typedef struct _gpscomdef_value
{
   unsigned char   grp;         //��ʵ�����
   unsigned char   itm;         //��Ŀ�� 
   unsigned short  value; 

}GPSCOMDEF_VALE;

typedef struct _gpscomdef_data
{
      unsigned short  year;   //��
      unsigned short  month;   //��
	  unsigned short  day;     //��
      unsigned short  hour;     //ʱ��
	  unsigned short  minute;  //��
	  unsigned short  sec;     //��
      unsigned int    msec;   //����
      unsigned char   manclock; //��ʱ��
	  unsigned char   statecount; //״̬λ������
      GPSCOMDEF_STATE state[GPSCOMDEF_STATE_COUNT];
	  unsigned char   valuecount;
	  GPSCOMDEF_VALE  value[GPSCOMDEF_VALUE_COUNT];
	

}GPSCOMDEF_DATA;
//������Ϣ���ͻ�����ʾ������
typedef struct _gpscomdef_event
{
    unsigned  char infotype; //�������ͣ�״̬������ʱʵ��
	unsigned  short grp;
    unsigned  short itm;
	char      szSrc[128];    //վ��
	char      szName[128];   //�����
    char      szUnit[10];    //��λ
    char      szValue[64];   //ֵ
	char      szDispValue[64]; //��ʾ��ֵ
	char      szTime[32];
	unsigned  q;          //Ʒ������(61850ʹ��)

}GPSCOMDEF_EVENT;

//�ٻ��Ƿ��͵����ݵ�����,���ݿ��в���������
//GPSCOMDEF_CMD_DATA
//szData��   1���ֽ�ΪGPSCOMDEF_SENDDATA�ĸ���������Ϊ����ṹ��������

typedef struct _gpscomdef_senddata
{
	unsigned  char infotype; //�������ͣ�״̬������ʱʵ��
	unsigned  short grp;
    unsigned  short itm;
    char      szValue[32]; 	

}GPSCOMDEF_SENDDATA;
//�ڲ�����Ĳ���ֵ�ṹGPSCOMDEF_TEST_DATA
typedef struct _gpscomdef_testvalue
{
	unsigned long long   t0;
	unsigned long long   t1;
	unsigned long long   t2;
	unsigned long long   t3;
	char szTime[32];    //ʱ��ֵ�ı�ʾ

}GPSCOMDEF_TESTVALUE;


//�����ǿͻ������������¼���Ϣ��61850�����
typedef struct _gpscomdef_testevent
{
   unsigned short grp;  //���
   unsigned short itm;  //��Ŀ��
   unsigned int   val;  //ֵ 0-
   unsigned short q;     //q
   unsigned char sztime[48]; //ʱ��,���ʱ��Ϊ0���ɷ����ṩģ���¼���ʱ��
   char szcomment[48];       //��ע������ʹ��

}GPSCOMDEF_TESTEVENT;
/*ʹ��˵��:��̬�⵼������ 
COM_DATA_EXPORT int  gpsinterface_sendcmd(unsigned short stationid, GPSCOMDEF_UDP_COM *cmd_struct); �����ӿڣ��������ݵ���̬����
if (cmd_struct->comcode == GPSCOMDEF_SET_EVENT)
{
     GPSCOMDEF_TESTEVENT *pEvent = (GPSCOMDEF_TESTEVENT*)cmd_struct->szData;
     pEvent->sztime �ĳ���Ϊ0���ɷ�����ṩʱ��
	 switch(pEvent->itm)
	 {
	 case 1:  //��ʱ�ź�״̬   
	 {
	 
	 }
     break;
	 case 2: //��ʱ����״̬
	 {
	 
	 
	 }
	 break;
	 case 3: //ʱ���������״̬
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

