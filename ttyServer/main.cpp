////****************************************************************************//
//* Copyright (c) 2007, ���ӵ������޹�˾                                      
//* All rights reserved.                                                     
//* �ļ� ttyS1000.h                                               
//* ���� 2015-5-18        
//* ���� rwp                 
//* ע��                                  
//****************************************************************************//
#include "manager.h"
#include <sched.h> 
#include <sys/prctl.h>


//ת������ʱ��
#define BUILD_MONTH_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_FEB (__DATE__[0] == 'F')
#define BUILD_MONTH_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define BUILD_MONTH_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define BUILD_MONTH_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define BUILD_MONTH_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define BUILD_MONTH_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define BUILD_MONTH_IS_SEP (__DATE__[0] == 'S')
#define BUILD_MONTH_IS_OCT (__DATE__[0] == 'O')
#define BUILD_MONTH_IS_NOV (__DATE__[0] == 'N')
#define BUILD_MONTH_IS_DEC (__DATE__[0] == 'D')

#define BUILD_M0 \
((BUILD_MONTH_IS_OCT || BUILD_MONTH_IS_NOV || BUILD_MONTH_IS_DEC) ? '1' : '0')
#define BUILD_M1 \
( \
(BUILD_MONTH_IS_JAN) ? '1' : \
(BUILD_MONTH_IS_FEB) ? '2' : \
(BUILD_MONTH_IS_MAR) ? '3' : \
(BUILD_MONTH_IS_APR) ? '4' : \
(BUILD_MONTH_IS_MAY) ? '5' : \
(BUILD_MONTH_IS_JUN) ? '6' : \
(BUILD_MONTH_IS_JUL) ? '7' : \
(BUILD_MONTH_IS_AUG) ? '8' : \
(BUILD_MONTH_IS_SEP) ? '9' : \
(BUILD_MONTH_IS_OCT) ? '0' : \
(BUILD_MONTH_IS_NOV) ? '1' : \
(BUILD_MONTH_IS_DEC) ? '2' : \
/* error default */ '?' \
)
#define BUILD_Y0 (__DATE__[ 7])
#define BUILD_Y1 (__DATE__[ 8])
#define BUILD_Y2 (__DATE__[ 9])
#define BUILD_Y3 (__DATE__[10])

#define BUILD_D0 ((__DATE__[4] >= '0') ? (__DATE__[4]) : '0')
#define BUILD_D1 (__DATE__[ 5])

int main(int argc, char *argv[])
{

#define SET_TO_SCHED_RR 	0	//û��������
#define PRIORITY_SCHED_RR 	50	//0-99��0=��ͨ����
#ifdef SET_TO_SCHED_RR 
	struct sched_param sched; 
	sched_getparam(0, &sched);				//0 = the calling process 
	printf("SCHED_RR: %d -> %d\n",sched.sched_priority,PRIORITY_SCHED_RR);
	sched.sched_priority = PRIORITY_SCHED_RR; 
	sched_setscheduler(0, SCHED_RR, &sched);//���ó�ʵʱ����ģʽ
#endif 
	
	char    PROGRAM_DATA[40];	//����汾����
	sprintf(PROGRAM_DATA,"FH1000-ETH���ڷ������ �汾����:%c%c%c%c-%c%c-%c%c "__TIME__"",BUILD_Y0,BUILD_Y1,BUILD_Y2,BUILD_Y3,
			BUILD_M0,BUILD_M1,BUILD_D0,BUILD_D1);
	printf("%s \r\n",PROGRAM_DATA);
	if (argc>=2){
		int tmp = atoi(argv[1]);
		tmp =~tmp;	//����������澯
	}
	//�߳�1 Main���������߳�
	prctl(PR_SET_NAME, (unsigned long)"TSERVER_0_Main");
	
	Cmanager fh1000;
	
	if (0 != fh1000.Start()){	//��������
		fh1000.End();
		return 0;
	}

	while(1){
		sleep(10);
	}
	printf("\n!!!!��������ֹ�����˳�!!!!\n");
	fh1000.End();
	return 0;
}
