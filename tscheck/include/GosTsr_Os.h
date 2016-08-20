#ifndef __GOSTSR_OS_H__
#define __GOSTSR_OS_H__

#include "GosTsr_Common.h"


typedef struct 
{
	GOSTSR_U16 u32DataLen;
	GOSTSR_U8  msgType;	
	GOSTSR_U8 *pData;
	
}TABLE_MSG;

#if 0
typedef struct table_msg_st
{
	long int msgType;
	union 
	{
		GOSTSR_U8	u8Ts_188[PACKAGE_LEN_188];	/*188字节格式的packet*/
		GOSTSR_U8	u8Ts_204[PACKAGE_LEN_204];	/*204字节格式的packet*/
	}TS_TYPE;
}TABLE_MSG;
#endif
extern int PSISI_Msg_Init(void);
extern int PSISI_Msg_DeInit(void);
extern int PSISI_Msg_recvMsg(TABLE_MSG *msg);
extern int PSISI_Msg_sendMsg(TABLE_MSG *msg);
extern GOSTSR_BOOL OS_Packet_DeleteSemaphore(OS_Packet_Semaphore_t *Semaphore_p);
extern GOSTSR_BOOL OS_Packet_SignalSemaphore(OS_Packet_Semaphore_t *Semaphore_p);
extern GOSTSR_BOOL OS_Packet_WaitSemaphore(OS_Packet_Semaphore_t *Semaphore_p);
extern OS_Packet_Semaphore_t *OS_Packet_CreateSemaphore(const GOSTSR_S32 InitialValue);

#endif

