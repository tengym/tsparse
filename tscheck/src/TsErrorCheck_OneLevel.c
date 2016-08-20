#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "TsErrorCheck_OneLevel.h"
#include "TsErrorCheck_TwoLevel.h"
#include "TsErrorCheck_Log.h"

static TSERROR_ONELEVEL_PARAM_S stOneLevelParam[TSERROR_CHANNELID_MAX];

static GOSTSR_U32 Getts_Speed_Ms(GOSTSR_U8 chanID)
{
	if(chanID >= TSERROR_CHANNELID_MAX)
	{
		return GOSTSR_FAILURE;
	}
	return TsErrorCheck_TwoLevel_getAvTransportRate(chanID);
}

static GOSTSR_U32 Getts_TimeMs_byByteDiff(GOSTSR_U8 chanID,GOSTSR_U32 u32ByteDiff)
{
	GOSTSR_U32 timeMs = 0;
	GOSTSR_F32 f32timeMs = 0;
	GOSTSR_U32 u32AvTransportRate =  0;

	if(chanID >= TSERROR_CHANNELID_MAX)
	{
		return GOSTSR_FAILURE;
	}
	u32AvTransportRate  = TsErrorCheck_TwoLevel_getAvTransportRate(chanID);

	if(u32AvTransportRate > 0)
	{
		f32timeMs = (1.0*u32ByteDiff * 8 / u32AvTransportRate) *1000;
	}
	
	timeMs = f32timeMs;
	
	return timeMs;
}

static GOSTSR_S32 TsErrorCheck_OneLevel_setPmtPid(GOSTSR_U8 chanID,SEARCH_INFO_S *stSearchInfo)
{
	GOSTSR_S32 i = 0;

	if((stSearchInfo == GOSTSR_NULL) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	
	PMTPID_TIMEOUT_INFO *pPmtPidInfo = &stOneLevelParam[chanID].pmtPid_timeout_info;
	pPmtPidInfo->NbPmtInfo = (stSearchInfo->u16NbProg > ONE_ERROR_PMT_MAX? ONE_ERROR_PMT_MAX : stSearchInfo->u16NbProg);
	for(i = 0 ; i < stSearchInfo->u16NbProg ; i++)
	{
		pPmtPidInfo->pmtInfo[i].pid = stSearchInfo->stProgInfo[i].PmtPid;
	}
	
	return GOSTSR_SUCCESS;
}

static GOSTSR_S32 TsErrorCheck_OneLevel_setPesPid(GOSTSR_U8 chanID,SEARCH_INFO_S *stSearchInfo)
{
	GOSTSR_S32 i = 0, j =0, k = 0;

	if((stSearchInfo == GOSTSR_NULL) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	
	PESPID_TIMEOUT_INFO *pPesPidInfo = &stOneLevelParam[chanID].pesPid_timeout_info;
	stSearchInfo->u16NbProg = (stSearchInfo->u16NbProg > SEARCH_PROG_NUM_MAX ? SEARCH_PROG_NUM_MAX : stSearchInfo->u16NbProg);
	for(i = 0; i < stSearchInfo->u16NbProg;i++)
	{
		for(j =0; j  < stSearchInfo->stProgInfo[i].u8NbPes; j++)
		{	
			if(k < ONE_ERROR_PES_MAX)
			{
				pPesPidInfo->pesInfo[k++].pid = stSearchInfo->stProgInfo[i].PesPid[j];
			}
		}
	}
	pPesPidInfo->NbPesInfo = k;
	
	return GOSTSR_SUCCESS;
}

static GOSTSR_BOOL TsErrorCheck_OneLevel_checkisPesPid(GOSTSR_U8 chanID,GOSTSR_U16 Pid,GOSTSR_U32 *index)
{
	GOSTSR_S32 i = 0;

	if((GOSTSR_NULL == index) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FALSE;
	}
	
	PESPID_TIMEOUT_INFO *pPesPidInfo = &stOneLevelParam[chanID].pesPid_timeout_info;
	for(i = 0; i < pPesPidInfo->NbPesInfo; i++)
	{
		if(pPesPidInfo->pesInfo[i].pid == Pid)
		{
			*index = i;
			return GOSTSR_TRUE;
		}
	}
	return GOSTSR_FALSE;
}
static GOSTSR_BOOL TsErrorCheck_OneLevel_checkisPmtPid(GOSTSR_U8 chanID,GOSTSR_U16 Pid,GOSTSR_U32 *index)
{
	GOSTSR_S32 i = 0;
	if((GOSTSR_NULL == index) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_NULL;
	}
	
	PMTPID_TIMEOUT_INFO *pPmtPidInfo = &stOneLevelParam[chanID].pmtPid_timeout_info;
	for(i = 0; i < pPmtPidInfo->NbPmtInfo; i++)
	{
		if(pPmtPidInfo->pmtInfo[i].pid == Pid)
		{
			*index = i;
			return GOSTSR_TRUE;
		}
	}
	return GOSTSR_FALSE;
}


/*同步错误分为同步丢失错误和同步字节错*/
//(1).同步丢失错;返回值为总共统计的同步丢失错的个数

GOSTSR_S32 TsErrorCheck_OneLevel_SyncLossError(GOSTSR_U8 chanID,GOSTSR_BOOL bisMatched,GOSTSR_U32 tmpSyncOffset)
{	
	if((!bisMatched) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	
	stOneLevelParam[chanID].oneLevelError_record.tsSyncLossError++;//同步丢失
	TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_SYNCLOSS,tmpSyncOffset,GOSTSR_NULL,GOSTSR_NULL);
	return GOSTSR_SUCCESS;
}

//(2)同步字节错，返回的值是不断累积的结果，
GOSTSR_S32 TsErrorCheck_OneLevel_SyncByteError(GOSTSR_U8 chanID,GOSTSR_BOOL bisMatched,GOSTSR_U32 tmpSyncOffset)
{
	if((!bisMatched) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	stOneLevelParam[chanID].oneLevelError_record.tsSyncByteError++;//同步字节
	TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_SYNCBYTE,tmpSyncOffset,GOSTSR_NULL,GOSTSR_NULL);
	return GOSTSR_SUCCESS;
}
//(3)连续计数器错。返回的值是不断累积的结果
GOSTSR_S32 TsErrorCheck_OneLevel_ContinuityCounterError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U32 pid,GOSTSR_U8 Continuity,GOSTSR_U8  adapter_control,GOSTSR_U8 disContinuityIndicator)
{
	GOSTSR_S32 i = 0, j = 0;
	GOSTSR_BOOL bRepeat = GOSTSR_FALSE;
	GOSTSR_U8 chanID = 0;
	GOSTSR_U32 index = 0;
	
	if((pstErrorInfo == GOSTSR_NULL) || (!pstErrorInfo->bisMatched) || (pid == 0x1fff) || (0xff == pid) || (pstErrorInfo->chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	chanID = pstErrorInfo->chanID;
#if 1
	if(!TsErrorCheck_OneLevel_checkisPesPid(chanID,pid,&index))
	{
		return GOSTSR_FAILURE;
	}
#endif

	for(i = 0; i < PID_NUMBER_MAX; i++)
	{
		if((stOneLevelParam[chanID].stContinutyCount[i].pid == pid) && (stOneLevelParam[chanID].stContinutyCount[i].bUsed))
		{
			break;
		}
	}
	if(i == PID_NUMBER_MAX)
	{
		for(j = 0; j < PID_NUMBER_MAX; j++)
		{
			if(!stOneLevelParam[chanID].stContinutyCount[j].bUsed)
			{
				stOneLevelParam[chanID].stContinutyCount[j].bUsed = GOSTSR_TRUE;
				stOneLevelParam[chanID].stContinutyCount[j].pid = pid;
				stOneLevelParam[chanID].stContinutyCount[j].continutycount = Continuity;
				stOneLevelParam[chanID].stContinutyCount[j].predisContinuityIndicator= disContinuityIndicator;
				break;
			}
		}
		return GOSTSR_FAILURE;
	}

	if((stOneLevelParam[chanID].stContinutyCount[i].continutycount == Continuity) && (Continuity!=0))//待用
	{
		bRepeat = GOSTSR_TRUE;	
	}

	if(Continuity != 0)
	{
		if(adapter_control == 0)
		{
			if(stOneLevelParam[chanID].stContinutyCount[i].continutycount != Continuity)
			{
				//stOneLevelParam[chanID].oneLevelError_record.tsContinuityCounterError++;	
				//TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_CONTINUTYCOUNT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
			}
		}
		else if(adapter_control == 1)
		{
			if(stOneLevelParam[chanID].stContinutyCount[i].continutycount != Continuity - 1)
			{
				stOneLevelParam[chanID].oneLevelError_record.tsContinuityCounterError++;	
				//TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_CONTINUTYCOUNT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
			}
		}
		else if(adapter_control == 2)
		{
			if(disContinuityIndicator == 1)
			{
				if(stOneLevelParam[chanID].stContinutyCount[i].continutycount+1 == Continuity)
				{
					stOneLevelParam[chanID].oneLevelError_record.tsContinuityCounterError++;
					//TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_CONTINUTYCOUNT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
				}
			}


			else
			{
				if(stOneLevelParam[chanID].stContinutyCount[i].continutycount != Continuity)
				{
					stOneLevelParam[chanID].oneLevelError_record.tsContinuityCounterError++;
					//TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_CONTINUTYCOUNT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
				}
			}
		}
		else if(adapter_control == 3)
		{
			if(disContinuityIndicator == 0)
			{
				if(stOneLevelParam[chanID].stContinutyCount[i].predisContinuityIndicator == 0)
				{
					if(stOneLevelParam[chanID].stContinutyCount[i].continutycount != Continuity -1)
					{
						stOneLevelParam[chanID].oneLevelError_record.tsContinuityCounterError++;
						//TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_CONTINUTYCOUNT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
					}
				}
			}
		}
		
	}
	else
	{
		if(adapter_control == 0)
			{
				if(stOneLevelParam[chanID].stContinutyCount[i].continutycount != Continuity)
				{	
					//stOneLevelParam[chanID].oneLevelError_record.tsContinuityCounterError++;
					//TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_CONTINUTYCOUNT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
				}
			}
			else if(adapter_control == 1)
			{
				if(stOneLevelParam[chanID].stContinutyCount[i].continutycount != 0xf)
				{	
					stOneLevelParam[chanID].oneLevelError_record.tsContinuityCounterError++;
					//TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_CONTINUTYCOUNT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
				}
			}
			else if(adapter_control == 2)
			{
			if(disContinuityIndicator == 1)
			{
				if(stOneLevelParam[chanID].stContinutyCount[i].continutycount+1 == Continuity)
				{
					stOneLevelParam[chanID].oneLevelError_record.tsContinuityCounterError++;
					//TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_CONTINUTYCOUNT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
				}
			}
			else
			{
					if(stOneLevelParam[chanID].stContinutyCount[i].continutycount != Continuity)
					{
						stOneLevelParam[chanID].oneLevelError_record.tsContinuityCounterError++;
						//TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_CONTINUTYCOUNT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
					}
				}
			}
			else if(adapter_control == 3)
			{
				if(disContinuityIndicator == 0)
				{
					if(stOneLevelParam[chanID].stContinutyCount[i].predisContinuityIndicator == 0)
					{
						if(stOneLevelParam[chanID].stContinutyCount[i].continutycount != 0xf)
						{	
							stOneLevelParam[chanID].oneLevelError_record.tsContinuityCounterError++;
							//TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_CONTINUTYCOUNT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
						}
					}
				}
			}
	}
	if(((adapter_control == 1) || (adapter_control == 3)) && (disContinuityIndicator == 0))
	{
		stOneLevelParam[chanID].stContinutyCount[i].continutycount = Continuity;
	}
	else
	{
		//printf("**********---adapter_control = %d__disContinuityIndicator = %d\n",adapter_control,disContinuityIndicator);
	}
	
	stOneLevelParam[chanID].stContinutyCount[i].predisContinuityIndicator= disContinuityIndicator;
	
	return GOSTSR_FAILURE;
}

//(4)PAT错,三种错误，超时错，tableID错,加扰错
GOSTSR_S32 TsErrorCheck_OneLevel_PatError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U32 pid,GOSTSR_U32 tableId,GOSTSR_U8  scramble_control)
{
	GOSTSR_U32 	u32bytePosDiff = 0; //保存两个pat之间的包的个数
	GOSTSR_U8 	chanID = 0;
	
	if((pstErrorInfo == GOSTSR_NULL) || (!pstErrorInfo->bisMatched) || (pid != PAT_PID) || (pstErrorInfo->chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	chanID = pstErrorInfo->chanID;
	//比较时间检查是否超时
	if(stOneLevelParam[chanID].u32BytePos_PatBak != 0)
	{
		if(pstErrorInfo->bCarryFlag)
		{
			u32bytePosDiff = ONELEVEL_BYTEPOS_MAX+pstErrorInfo->bytePos -stOneLevelParam[chanID].u32BytePos_PatBak;
		}
		else
		{
			if(pstErrorInfo->bytePos >= stOneLevelParam[chanID].u32BytePos_PatBak)
			{
				u32bytePosDiff = pstErrorInfo->bytePos -stOneLevelParam[chanID].u32BytePos_PatBak;
			}
			else
			{
				u32bytePosDiff = stOneLevelParam[chanID].u32BytePos_PatBak - pstErrorInfo->bytePos;
			}
		}
			
		stOneLevelParam[chanID].u32BytePos_PatBak = pstErrorInfo->bytePos;
		/*超时检测*/	
		if(Getts_Speed_Ms(chanID) > 0)
		{
			if(Getts_TimeMs_byByteDiff(chanID,u32bytePosDiff) > 500)
			{		
				stOneLevelParam[chanID].oneLevelError_record.tsPatError.timeout_error++;
				stOneLevelParam[chanID].oneLevelError_record.tsPatError.total_error++;
				TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_PAT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
				return GOSTSR_SUCCESS;
			}
		}
			
	}
	else
	{
		stOneLevelParam[chanID].u32BytePos_PatBak = pstErrorInfo->bytePos;
	}
	
	if(pid == PAT_PID)
	{	
		if(tableId != PAT_TABLE_ID) //tabel id 错
		{
			stOneLevelParam[chanID].oneLevelError_record.tsPatError.tableid_error++;	
			stOneLevelParam[chanID].oneLevelError_record.tsPatError.total_error++;
			TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_PAT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
		}
		else if(scramble_control != 0) //加扰错
		{
			stOneLevelParam[chanID].oneLevelError_record.tsPatError.scramble_error++;
			stOneLevelParam[chanID].oneLevelError_record.tsPatError.total_error++;
			TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_PAT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
		}	
	}
	
	return GOSTSR_SUCCESS;		
}

//(5)PMT错两种情况，超时和加扰
GOSTSR_S32 TsErrorCheck_OneLevel_PmtError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U16 pid,GOSTSR_U8  scramble_control)
{
	GOSTSR_S32 i = 0;
	GOSTSR_U32 index = 0;
	GOSTSR_U32 u32BytePosDiff= 0;
	GOSTSR_U8 chanID = 0;
	PMTPID_TIMEOUT_INFO *pPmtPidInfo = GOSTSR_NULL;

	if((pstErrorInfo == GOSTSR_NULL) || (!pstErrorInfo->bisMatched) || (pstErrorInfo->chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	chanID = pstErrorInfo->chanID;
	pPmtPidInfo = &stOneLevelParam[chanID].pmtPid_timeout_info;
	
	/*检测PID是否是PMT_PID*/
	if(!TsErrorCheck_OneLevel_checkisPmtPid(pstErrorInfo->chanID,pid,&index))
	{
		return GOSTSR_FAILURE;
	}
	/*加扰检测*/
	if(scramble_control != 0)
	{
		stOneLevelParam[chanID].oneLevelError_record.tsPmtError.scramble_error++;
		stOneLevelParam[chanID].oneLevelError_record.tsPmtError.total_error++;
		TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_PMT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
	}
	
	if(pPmtPidInfo->pmtInfo[index].bytePos != 0)
	{
		if(pstErrorInfo->bCarryFlag)
		{
			u32BytePosDiff = ONELEVEL_BYTEPOS_MAX+pstErrorInfo->bytePos -pPmtPidInfo->pmtInfo[i].bytePos;
		}
		else
		{
			if(pstErrorInfo->bytePos >= pPmtPidInfo->pmtInfo[i].bytePos)
			{
				u32BytePosDiff = pstErrorInfo->bytePos -pPmtPidInfo->pmtInfo[i].bytePos;
			}
			else
			{
				u32BytePosDiff = pPmtPidInfo->pmtInfo[i].bytePos - pstErrorInfo->bytePos;
			}
		}
		
		pPmtPidInfo->pmtInfo[i].bytePos = pstErrorInfo->bytePos;
		
		/*超时检测*/
		if(Getts_Speed_Ms(chanID) > 0)
		{
			if(Getts_TimeMs_byByteDiff(chanID,u32BytePosDiff) > 500)
			{
			
				stOneLevelParam[chanID].oneLevelError_record.tsPmtError.timeout_error ++;
				stOneLevelParam[chanID].oneLevelError_record.tsPmtError.total_error ++;
				TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_PMT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
			}
		}
	}
	else
	{
		pPmtPidInfo->pmtInfo[index].bytePos = pstErrorInfo->bytePos;
	}

	return GOSTSR_FAILURE;
}

//(6)PID错音视频PID 的发送间隔超过5S
GOSTSR_S32 TsErrorCheck_OneLevel_pidMissError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U16 pid)
{
	GOSTSR_U32 u32BytePosDiff = 0;
	GOSTSR_U32 index = 0;
	GOSTSR_U8 chanID = 0;
	PESPID_TIMEOUT_INFO *pPesPidInfo = GOSTSR_NULL;

	if((pstErrorInfo == GOSTSR_NULL) ||(!pstErrorInfo->bisMatched) || (pstErrorInfo->chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}

	chanID = pstErrorInfo->chanID;
	pPesPidInfo = &stOneLevelParam[chanID].pesPid_timeout_info;
	
	if(!TsErrorCheck_OneLevel_checkisPesPid(pstErrorInfo->chanID,pid,&index))
	{
		return GOSTSR_FAILURE;
	}
	
	if(pPesPidInfo->pesInfo[index].bytePos != 0) 
	{
		if(pstErrorInfo->bCarryFlag)
		{
			u32BytePosDiff = ONELEVEL_BYTEPOS_MAX+pstErrorInfo->bytePos -pPesPidInfo->pesInfo[index].bytePos;
		}
		else
		{
			if(pstErrorInfo->bytePos >= pPesPidInfo->pesInfo[index].bytePos)
			{
				u32BytePosDiff = pstErrorInfo->bytePos -pPesPidInfo->pesInfo[index].bytePos;
			}
			else
			{
				u32BytePosDiff = pPesPidInfo->pesInfo[index].bytePos - pstErrorInfo->bytePos;
			}
		}	
		
		pPesPidInfo->pesInfo[index].bytePos = pstErrorInfo->bytePos;
		/*超时检测*/
		if(Getts_Speed_Ms(chanID) > 0)
		{
			if(Getts_TimeMs_byByteDiff(chanID,u32BytePosDiff) > 5000)
			{					
				stOneLevelParam[chanID].oneLevelError_record.tsPIDMissError++;
				TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_PID,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
				return GOSTSR_SUCCESS;
			}
		}
	}
	else
	{
		pPesPidInfo->pesInfo[index].recvFlag = GOSTSR_TRUE;
		pPesPidInfo->pesInfo[index].bytePos = pstErrorInfo->bytePos;
		return GOSTSR_FAILURE;
	}

	return GOSTSR_FAILURE;
}
/*检测没有到达的PID*/
GOSTSR_S32 TsErrorCheck_OneLevel_pidMissNotReach(GOSTSR_U8 chanID)
{
	GOSTSR_S32 i = 0;

	if(chanID >= TSERROR_CHANNELID_MAX)
	{
		return GOSTSR_FAILURE;
	}
	
	PESPID_TIMEOUT_INFO *pPesPidInfo = &stOneLevelParam[chanID].pesPid_timeout_info;
	
	for(i = 0 ; i < pPesPidInfo->NbPesInfo;i++)
	{
		if(!pPesPidInfo->pesInfo[i].recvFlag)
		{		
			stOneLevelParam[chanID].oneLevelError_record.tsPIDMissError++;
			TS_ERROR_LOG(chanID,TYPE_ERROR_ONE,TR101290_ONE_ERROR_PID,0,GOSTSR_NULL,GOSTSR_NULL);
		}
	}
	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_OneLevel_setPid(GOSTSR_U8 chanID,SEARCH_INFO_S *stSearchInfo)
{
	if((chanID >= TSERROR_CHANNELID_MAX) || (GOSTSR_NULL == stSearchInfo))
	{
        printf("data is NULL \n");
		return GOSTSR_FAILURE;
	}
	TsErrorCheck_OneLevel_setPmtPid(chanID,stSearchInfo);
	TsErrorCheck_OneLevel_setPesPid(chanID,stSearchInfo);
	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_OneLevel_GetErrorInfo(GOSTSR_U8 chanID,TSERROR_ONELEVEL_RECORD *oneLevelErrorInfo)
{
	if((oneLevelErrorInfo == GOSTSR_NULL) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	
	memcpy(oneLevelErrorInfo,&stOneLevelParam[chanID].oneLevelError_record,sizeof(TSERROR_ONELEVEL_RECORD));
	
	return GOSTSR_SUCCESS;
} 

GOSTSR_S32 TsErrorCheck_OneLevel_Init()
{
	GOSTSR_S32 i = 0, j = 0;
	
	memset(&stOneLevelParam, 0x00, sizeof(TSERROR_ONELEVEL_PARAM_S) * TSERROR_CHANNELID_MAX);

	for(i = 0; i < TSERROR_CHANNELID_MAX; i++)
	{
		for(j = 0; j < ONE_ERROR_PMT_MAX; j++)
		{
			stOneLevelParam[i].pmtPid_timeout_info.pmtInfo[j].pid = 0xffff;
			stOneLevelParam[i].pmtPid_timeout_info.pmtInfo[j].recvFlag = GOSTSR_FALSE;
		}
		for(j = 0; j < ONE_ERROR_PES_MAX; j++)
		{
			stOneLevelParam[i].pesPid_timeout_info.pesInfo[j].pid = 0xffff;
			stOneLevelParam[i].pesPid_timeout_info.pesInfo[j].recvFlag = GOSTSR_FALSE;
		}
		for(j = 0; j < PID_NUMBER_MAX; j++)
		{
			stOneLevelParam[i].stContinutyCount[j].bUsed = GOSTSR_FALSE;
			stOneLevelParam[i].stContinutyCount[j].pid = 0xffff;
		}
	}

	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_OneLevel_DeInit()
{
	GOSTSR_S32 i = 0, j = 0;
	
	memset(&stOneLevelParam, 0x00, sizeof(TSERROR_ONELEVEL_PARAM_S) * TSERROR_CHANNELID_MAX);

	for(i = 0; i < TSERROR_CHANNELID_MAX; i++)
	{
		for(j = 0; j < ONE_ERROR_PMT_MAX; j++)
		{
			stOneLevelParam[i].pmtPid_timeout_info.pmtInfo[j].pid = 0xffff;
			stOneLevelParam[i].pmtPid_timeout_info.pmtInfo[j].recvFlag = GOSTSR_FALSE;
		}
		for(j = 0; j < ONE_ERROR_PES_MAX; j++)
		{
			stOneLevelParam[i].pesPid_timeout_info.pesInfo[j].pid = 0xffff;
			stOneLevelParam[i].pesPid_timeout_info.pesInfo[j].recvFlag = GOSTSR_FALSE;
		}
		for(j = 0; j < PID_NUMBER_MAX; j++)
		{
			stOneLevelParam[i].stContinutyCount[j].bUsed = GOSTSR_FALSE;
			stOneLevelParam[i].stContinutyCount[j].pid = 0xffff;
		}
	}
	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_OneLevel_ReInit(GOSTSR_U8 chanID)
{
	GOSTSR_S32 j = 0;
	
	if(chanID >= TSERROR_CHANNELID_MAX)
	{
		return GOSTSR_FAILURE;
	}
	memset(&stOneLevelParam[chanID], 0x00, sizeof(TSERROR_ONELEVEL_PARAM_S));

	for(j = 0; j < ONE_ERROR_PMT_MAX; j++)
	{
		stOneLevelParam[chanID].pmtPid_timeout_info.pmtInfo[j].pid = 0xffff;
		stOneLevelParam[chanID].pmtPid_timeout_info.pmtInfo[j].recvFlag = GOSTSR_FALSE;
	}
	for(j = 0; j < ONE_ERROR_PES_MAX; j++)
	{
		stOneLevelParam[chanID].pesPid_timeout_info.pesInfo[j].pid = 0xffff;
		stOneLevelParam[chanID].pesPid_timeout_info.pesInfo[j].recvFlag = GOSTSR_FALSE;
	}
	for(j = 0; j < PID_NUMBER_MAX; j++)
	{
		stOneLevelParam[chanID].stContinutyCount[j].bUsed = GOSTSR_FALSE;
		stOneLevelParam[chanID].stContinutyCount[j].pid = 0xffff;
	}

	return GOSTSR_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /*  __cplusplus  */
