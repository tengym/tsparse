#include "GosTsr_Os.h"
#include<mqueue.h>
#include <errno.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */

static GOSTSR_S32 msgId = -1;

OS_Packet_Semaphore_t *OS_Packet_CreateSemaphore(const GOSTSR_S32 InitialValue)
{
    OS_Packet_Semaphore_t *Semaphore_p = GOSTSR_NULL;

    Semaphore_p = (OS_Packet_Semaphore_t *)OS_Packet_AllocMemory(sizeof(OS_Packet_Semaphore_t));

    if (Semaphore_p != GOSTSR_NULL)
    {
        if (sem_init(Semaphore_p, 0, InitialValue))
        {
            OS_Packet_FreeMemory(Semaphore_p);
            return GOSTSR_NULL;
        }
    }

    return Semaphore_p;
}

GOSTSR_BOOL OS_Packet_DeleteSemaphore(OS_Packet_Semaphore_t *Semaphore_p)
{
    if (Semaphore_p != GOSTSR_NULL)
    {
        sem_destroy(Semaphore_p);
        OS_Packet_FreeMemory(Semaphore_p);
    }
    else
    {
        return GOSTSR_FALSE;
    }

    return GOSTSR_TRUE;
}

GOSTSR_BOOL OS_Packet_SignalSemaphore(OS_Packet_Semaphore_t *Semaphore_p)
{
    if (Semaphore_p != GOSTSR_NULL)
    {
        sem_post(Semaphore_p);
        return GOSTSR_TRUE;
    }

    return GOSTSR_FALSE;
}

GOSTSR_BOOL OS_Packet_WaitSemaphore(OS_Packet_Semaphore_t *Semaphore_p)
{
    if (Semaphore_p != GOSTSR_NULL)
    {
        return (sem_wait(Semaphore_p) == 0) ? GOSTSR_TRUE : GOSTSR_FALSE;
    }

    return GOSTSR_FALSE;
}

GOSTSR_S32 PSISI_Msg_Init(void)
{
	 msgId=msgget((key_t)1234,IPC_EXCL);  /*检查消息队列是否存在*/  
	if(msgId < 0)
	{  
	    msgId = msgget((key_t)1234, 0666|IPC_CREAT);
	}
	
    return msgId;
}

GOSTSR_S32 PSISI_Msg_DeInit(void)
{
    msgctl(msgId, IPC_RMID, 0);

    return GOSTSR_SUCCESS;
}

GOSTSR_S32 PSISI_Msg_recvMsg(TABLE_MSG *msg)
{
	GOSTSR_S32 retVal = GOSTSR_FAILURE;

	if (NULL != msg)
	{
		retVal = msgrcv(msgId, (void *)msg, sizeof(TABLE_MSG), 0, 0);
	}

	return retVal;
}

GOSTSR_S32 PSISI_Msg_sendMsg(TABLE_MSG *msg)
{
    GOSTSR_S32 retVal = GOSTSR_FAILURE;

    if (msg != NULL)
    {
        retVal = msgsnd(msgId, (void *)msg, sizeof(TABLE_MSG), IPC_NOWAIT);
    }

    return retVal;
}

inline void U16_to_U8(GOSTSR_U8 **q_ptr, GOSTSR_S32 val)
{
    GOSTSR_U8 *q;
    q = *q_ptr;
    *q++ = val >> 8;
    *q++ = val;
    *q_ptr = q;
}

/* notice: str == NULL is accepted for an empty string */
void Str16_to_Str8(GOSTSR_U8 **q_ptr, const GOSTSR_U8 *str)
{
    GOSTSR_U8 *q;
    GOSTSR_S32 len;

    q = *q_ptr;
    if (!str)
        len = 0;
    else
        len = strlen((char *)str);
    *q++ = len;
    memcpy(q, str, len);
    q += len;
    *q_ptr = q;
}