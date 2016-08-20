#include "TsErrorCheck_TwoLevel.h"
#include "TsErrorCheck_Log.h"
#include "GosTsr_AnalysisData.h"

static TSERROR_TWOLEVEL_PARAM_S stTwoLevelParam[TSERROR_CHANNELID_MAX];


GOSTSR_BOOL TsErrorCheck_TwoLevel_getEnableCheckFlag(GOSTSR_U8 chanID)
{
	if(chanID >= TSERROR_CHANNELID_MAX)
	{
		return GOSTSR_FAILURE;
	}
	return stTwoLevelParam[chanID].bTsRateisRecved;
}

/*获取平均码率，单位:bit/s*/
GOSTSR_U32 TsErrorCheck_TwoLevel_getAvTransportRate(GOSTSR_U8 chanID)
{
	GOSTSR_U64 u64TransportRate_All =  0;
	GOSTSR_U32 u32AvTransportRate =  0;
	GOSTSR_U32 u32TransportRate_Max =  0;
	GOSTSR_U32 u32TransportRate_Min=  0;
	GOSTSR_U32 i = 0;
	GOSTSR_U16 u16Count = 0;
	GOSTSR_U8 u8CarryBit = 0;

	if(chanID >= TSERROR_CHANNELID_MAX)
	{
		return GOSTSR_FAILURE;
	}
	for(i = 0; i < PCRPID_NUM_MAX; i++)
	{
		if((stTwoLevelParam[chanID].stTsRateInfo[i].u32TransportRate != 0) )
		{
			u32TransportRate_Max = stTwoLevelParam[chanID].stTsRateInfo[i].u32TransportRate;
			u32TransportRate_Min = u32TransportRate_Max;
			break;
		}
	}

	for(i = 0; i < PCRPID_NUM_MAX; i++)
	{
		if((stTwoLevelParam[chanID].stTsRateInfo[i].u32TransportRate == 0) || (stTwoLevelParam[chanID].stTsRateInfo[i].bFirstUsed) )
			continue;

		if(u32TransportRate_Max < stTwoLevelParam[chanID].stTsRateInfo[i].u32TransportRate)
		{
			u32TransportRate_Max = stTwoLevelParam[chanID].stTsRateInfo[i].u32TransportRate;
		}
		if(u32TransportRate_Min > stTwoLevelParam[chanID].stTsRateInfo[i].u32TransportRate)
		{
			u32TransportRate_Min = stTwoLevelParam[chanID].stTsRateInfo[i].u32TransportRate;
		}
		
		if((u64TransportRate_All+stTwoLevelParam[chanID].stTsRateInfo[i].u32TransportRate) >= TSERROR_INVALID_U32)
		{
			u64TransportRate_All = stTwoLevelParam[chanID].stTsRateInfo[i].u32TransportRate-(TSERROR_INVALID_U32-u64TransportRate_All);
			u8CarryBit++; /*进位标志*/
		}	
		else
		{
			u64TransportRate_All += stTwoLevelParam[chanID].stTsRateInfo[i].u32TransportRate;
		}

		u16Count++;	
		
	}
	
	if(u16Count != 0)
	{

		if(u16Count > 2)
		{
			/*除去最大值和最小值,再求平均值*/
			u32AvTransportRate = (u8CarryBit*TSERROR_INVALID_U32 + u64TransportRate_All-u32TransportRate_Min-u32TransportRate_Max) / (u16Count-2);		
			//printf("-->2----u32AvTransportRate = %d\n",u32AvTransportRate);
		}
		else
		{
			u32AvTransportRate = (u8CarryBit*TSERROR_INVALID_U32 + u64TransportRate_All) / (u16Count);	
			//printf("##############-count:%d--u32AvTransportRate = %d\n",u16Count,u32AvTransportRate);
		}			
	}
	if(u32AvTransportRate != 0)
	{
		stTwoLevelParam[chanID].TsRate_Valid = u32AvTransportRate;
	}
	
	//if(u32AvTransportRate != 0)
	//	printf("AvRate:  ChanID:%d---Count:%d------u32AvTransportRate = %d\n",chanID,u16Count,stTwoLevelParam[chanID].TsRate_Valid);
	return stTwoLevelParam[chanID].TsRate_Valid;
}

static GOSTSR_S32 TsErrorCheck_TwoLevel_setPcrPid(GOSTSR_U8 chanID,SEARCH_INFO_S *stSearchInfo)
{
	GOSTSR_S32 i = 0,k = 0, count = 0;

	if((GOSTSR_NULL == stSearchInfo) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}

	stSearchInfo->u16NbProg = (stSearchInfo->u16NbProg > SEARCH_PROG_NUM_MAX ? SEARCH_PROG_NUM_MAX : stSearchInfo->u16NbProg);
	for(i = 0; i < stSearchInfo->u16NbProg;i++)
	{
		for(k = 0; k < i; k++)
		{
			if(stTwoLevelParam[chanID].stPcrInfo[k].u16PcrPid == stSearchInfo->stProgInfo[i].PcrPid)
			{
				break;
			}
		}
		if(k == i)
		{
			if(count < PCRPID_NUM_MAX)
			{
				stTwoLevelParam[chanID].stPcrInfo[count].u16PcrPid = stSearchInfo->stProgInfo[i].PcrPid;
				count++;
			}
		}
	}
	//printf(">>>>>>>----chanID:%dPCR________Count = %d\n",chanID,count);

	return GOSTSR_SUCCESS;
}

static GOSTSR_S32 TsErrorCheck_TwoLevel_setPesPid(GOSTSR_U8 chanID,SEARCH_INFO_S *stSearchInfo)
{
	GOSTSR_S32 i = 0, j =0, k = 0;
	
	if((GOSTSR_NULL == stSearchInfo) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	
	stSearchInfo->u16NbProg = (stSearchInfo->u16NbProg > SEARCH_PROG_NUM_MAX ? SEARCH_PROG_NUM_MAX : stSearchInfo->u16NbProg);
	for(i = 0; i < stSearchInfo->u16NbProg;i++)
	{
		for(j =0; j  < stSearchInfo->stProgInfo[i].u8NbPes;j++)
		{	
			if(k < PESPID_NUM_MAX)
			{
				stTwoLevelParam[chanID].stPesInfo[k++].u16PesPid = stSearchInfo->stProgInfo[i].PesPid[j];
			}
		}
	}
	//printf("PES________Count = %d\n",k);
		
	return GOSTSR_SUCCESS;
}

static GOSTSR_BOOL TwoLevel_checkisPcrPid(GOSTSR_U8 chanID,GOSTSR_U16 Pid,GOSTSR_U32 *index)
{
	GOSTSR_S32 i = 0;
	
	if((GOSTSR_NULL == index) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FALSE;
	}
	for(i = 0; i < PCRPID_NUM_MAX; i++)
	{
		if(Pid == stTwoLevelParam[chanID].stPcrInfo[i].u16PcrPid)
		{
			*index = i;
			return GOSTSR_TRUE;
		}
	}
	
	return GOSTSR_FALSE;
}

static GOSTSR_BOOL TwoLevel_checkisPesPid(GOSTSR_U8 chanID,GOSTSR_U16 Pid,GOSTSR_U32 *index)
{
	GOSTSR_S32 i = 0;

	if((GOSTSR_NULL == index) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FALSE;
	}
	for(i = 0; i < PESPID_NUM_MAX; i++)
	{
		if(Pid == stTwoLevelParam[chanID].stPesInfo[i].u16PesPid)
		{
			*index = i;
			return GOSTSR_TRUE;
		}
	}
	
	return GOSTSR_FALSE;
}

static GOSTSR_F32 TwoLevel_getTsRateMs(GOSTSR_U8 chanID)
{
	GOSTSR_F32 f32AvTransportRate = 0.0;
	
	if((!stTwoLevelParam[chanID].bTsRateisRecved) || (chanID >= TSERROR_CHANNELID_MAX))
		return 0.0;
	
	f32AvTransportRate = 1.0 * TsErrorCheck_TwoLevel_getAvTransportRate(chanID) / 1000;
	//printf("_______ f32AvTransportRate = %f\n",f32AvTransportRate );
	return f32AvTransportRate;
}
static GOSTSR_F32 TwoLevel_getTsRateUs(GOSTSR_U8 chanID)
{
	GOSTSR_F32 f32AvTransportRate = 0.0;
	
	if((!stTwoLevelParam[chanID].bTsRateisRecved) || (chanID >= TSERROR_CHANNELID_MAX))
		return 0.0;
	
	f32AvTransportRate = 1.0 * TsErrorCheck_TwoLevel_getAvTransportRate(chanID) / 1000 / 1000;
	//printf("_______ f32AvTransportRate = %f\n",f32AvTransportRate );
	return f32AvTransportRate;
}

GOSTSR_S32 TsErrorCheck_TwoLevel_setPid(GOSTSR_U8 chanID,SEARCH_INFO_S *stSearchInfo)
{
	if((GOSTSR_NULL == stSearchInfo) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	TsErrorCheck_TwoLevel_setPcrPid(chanID,stSearchInfo);
	TsErrorCheck_TwoLevel_setPesPid(chanID,stSearchInfo);
	return GOSTSR_SUCCESS;
}

/*通过输入TS包的序号可以获取当前包的时间*/
GOSTSR_U32 TsErrorCheck_TwoLevel_getTimeUs_byBytePos(GOSTSR_U8 chanID,GOSTSR_U32 u32BytePos)
{
	GOSTSR_F32 f32TsRateUs= 0.0;
	GOSTSR_U32 u32TimeUsCur = 0; /*Unit:us*/

	if((!stTwoLevelParam[chanID].bTsRateisRecved) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return u32TimeUsCur;
	}
	
	f32TsRateUs = TwoLevel_getTsRateUs(chanID);
	if(f32TsRateUs > 0.0)
	{
		u32TimeUsCur = (1.0 * u32BytePos * 8) / f32TsRateUs;
	}
	
	return u32TimeUsCur;
}
GOSTSR_S32 TsErrorCheck_TwoLevel_setTransportRate(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U32 index,GOSTSR_U64 u64Pcr_Base, GOSTSR_U16 u64Pcr_Ext)
{
	GOSTSR_U32 u32PrePos = 0;
	GOSTSR_U32 u32CurPos = 0;
	GOSTSR_U32 u32AvTransportRate = 0;
	GOSTSR_U64 u64PrePcrBaseValue = 0;
	GOSTSR_U64 u64CurPcrBaseValue = 0;
	GOSTSR_U16 u16PrePcrExtenValue = 0;
	GOSTSR_U16 u16CurPcrExtenValue = 0;
	GOSTSR_U8 chanID = pstErrorInfo->chanID;

	if((GOSTSR_NULL == pstErrorInfo) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	if(stTwoLevelParam[chanID].stTsRateInfo[index].bFirstUsed)
	{
		stTwoLevelParam[chanID].stTsRateInfo[index].bFirstUsed = GOSTSR_FALSE;
		stTwoLevelParam[chanID].stTsRateInfo[index].u32BytePos= pstErrorInfo->bytePos;
		stTwoLevelParam[chanID].stTsRateInfo[index].u64PcrBase = u64Pcr_Base;
		stTwoLevelParam[chanID].stTsRateInfo[index].u16PcrExt = u64Pcr_Ext;
		return GOSTSR_FAILURE;
	}
	
	u32PrePos = stTwoLevelParam[chanID].stTsRateInfo[index].u32BytePos;
	u64PrePcrBaseValue = stTwoLevelParam[chanID].stTsRateInfo[index].u64PcrBase;
	u16PrePcrExtenValue = stTwoLevelParam[chanID].stTsRateInfo[index].u16PcrExt;
	
	u32CurPos = pstErrorInfo->bytePos;
	u64CurPcrBaseValue = u64Pcr_Base;
	u16CurPcrExtenValue = u64Pcr_Ext;
	
	if (u64CurPcrBaseValue < u64PrePcrBaseValue)
	{
		if(u32CurPos < u32PrePos)
		{	
			if((0x1ffffffffULL - u64PrePcrBaseValue + u64CurPcrBaseValue)*300 + u16CurPcrExtenValue - u16PrePcrExtenValue > 0)
			{
				u32AvTransportRate = (TWOLEVEL_BYTEPOS_MAX - u32PrePos +  u32CurPos)*8*27000000.0/((0x1ffffffffULL - u64PrePcrBaseValue + u64CurPcrBaseValue)*300 + u16CurPcrExtenValue - u16PrePcrExtenValue);
			}
		}
		else
		{	
			if((0x1ffffffffULL - u64PrePcrBaseValue + u64CurPcrBaseValue)*300 + u16CurPcrExtenValue - u16PrePcrExtenValue > 0)
			{
				u32AvTransportRate = (u32CurPos - u32PrePos)*8*27000000.0/((0x1ffffffffULL - u64PrePcrBaseValue + u64CurPcrBaseValue)*300 + u16CurPcrExtenValue - u16PrePcrExtenValue);
			}
		}
	}
	else
	{
		if(u32CurPos < u32PrePos)
		{	
			if((u64CurPcrBaseValue - u64PrePcrBaseValue)*300 + u16CurPcrExtenValue - u16PrePcrExtenValue > 0)
			{
				u32AvTransportRate = (GOSTSR_U32)((TWOLEVEL_BYTEPOS_MAX - u32PrePos +  u32CurPos)*8*27000000.0/((u64CurPcrBaseValue - u64PrePcrBaseValue)*300 + u16CurPcrExtenValue - u16PrePcrExtenValue));
			}
		}
		else
		{
			if(((u64CurPcrBaseValue -u64PrePcrBaseValue)*300 + u16CurPcrExtenValue - u16PrePcrExtenValue) > 0)
			{
				u32AvTransportRate = (u32CurPos - u32PrePos)*8*27000000.0/((u64CurPcrBaseValue -u64PrePcrBaseValue)*300 + u16CurPcrExtenValue - u16PrePcrExtenValue);
			}
		}
	}
	#if 0
	if (((stTwoLevelParam[chanID].stTsRateInfo[index].u32TransportRate != 0) && (abs(stTwoLevelParam[chanID].stTsRateInfo[index].u32TransportRate - u32AvTransportRate) < 1000000))
		|| (u32AvTransportRate != 0))
	{		
		return GOSTSR_FAILURE;
	}
	#endif

	if ((u32AvTransportRate < TWOLEVEL_PCR_TSRATE_MIN) || (u32AvTransportRate > TWOLEVEL_PCR_TSRATE_MAX))
	{		
		return GOSTSR_FAILURE;
	}
	
	stTwoLevelParam[chanID].stTsRateInfo[index].u32BytePos = pstErrorInfo->bytePos;
	stTwoLevelParam[chanID].stTsRateInfo[index].u64PcrBase = u64Pcr_Base;
	stTwoLevelParam[chanID].stTsRateInfo[index].u16PcrExt = u64Pcr_Ext;
	stTwoLevelParam[chanID].stTsRateInfo[index].u32TransportRate = u32AvTransportRate;	/*保存这一路节目的码率*/	
	stTwoLevelParam[chanID].bTsRateisRecved = GOSTSR_TRUE;
	
	//printf("chanID = %d-->PCR:%#x-----u32AvTransportRate = %d\n",chanID,stTwoLevelParam[chanID].stTsRateInfo[index].u16PcrPid,u32AvTransportRate);
	return GOSTSR_SUCCESS;
}


GOSTSR_S32 TsErrorCheck_TwoLevel_checkTransportError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U8 u8Error_indicater)
{
	GOSTSR_U8 chanID = 0;
	
	if((pstErrorInfo == GOSTSR_NULL) || (!pstErrorInfo->bisMatched) || (u8Error_indicater == 0) || (pstErrorInfo->chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	if(u8Error_indicater == 1)
	{
		chanID = pstErrorInfo->chanID;	
		stTwoLevelParam[chanID].stTwoLevel.u32TStransmissionErrorCount++;
	}
	//TS_ERROR_LOG(chanID,TYPE_ERROR_TWO,TR101290_TWO_ERROR_TRANSPORT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_TwoLevel_checkCrcError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U8 u8TableID,GOSTSR_U32 u32TableCrc,GOSTSR_U32 u32CalCrc)
{
	GOSTSR_U8 chanID = 0;
	GOSTSR_U8 flag = 0;
	
	if((pstErrorInfo == GOSTSR_NULL) || (u32TableCrc == u32CalCrc) || (pstErrorInfo->chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	chanID = pstErrorInfo->chanID;
	switch(u8TableID)
	{
		case PAT_TABLE_ID:
			stTwoLevelParam[chanID].stTwoLevel.stCrcErrorInfo.patCrcErrorCount++;
			stTwoLevelParam[chanID].stTwoLevel.stCrcErrorInfo.totalCrcErrorCount++; 
			flag = 1;
			break;
		case PMT_TABLE_ID:
			stTwoLevelParam[chanID].stTwoLevel.stCrcErrorInfo.pmCrctErrorCount++;
			stTwoLevelParam[chanID].stTwoLevel.stCrcErrorInfo.totalCrcErrorCount++;
			flag = 1;
			break;
		case CAT_TABLE_ID:
			stTwoLevelParam[chanID].stTwoLevel.stCrcErrorInfo.catCrcErrorCount++;
			stTwoLevelParam[chanID].stTwoLevel.stCrcErrorInfo.totalCrcErrorCount++;
			flag = 1;
			break;
		case NIT_TABLE_ID_ACTUAL:
		case NIT_TABLE_ID_OTHER:
			stTwoLevelParam[chanID].stTwoLevel.stCrcErrorInfo.nitCrcErrorCount++;
			stTwoLevelParam[chanID].stTwoLevel.stCrcErrorInfo.totalCrcErrorCount++;
			flag = 1;
			break;
		case BAT_TABLE_ID:
			stTwoLevelParam[chanID].stTwoLevel.stCrcErrorInfo.batCrcErrorCount++;
			stTwoLevelParam[chanID].stTwoLevel.stCrcErrorInfo.totalCrcErrorCount++;
			flag = 1;
			break;
		case SDT_TABLE_ID_ACTUAL:
		case SDT_TABLE_ID_OTEHR:
			stTwoLevelParam[chanID].stTwoLevel.stCrcErrorInfo.sdtCrcErrorCount++;
			stTwoLevelParam[chanID].stTwoLevel.stCrcErrorInfo.totalCrcErrorCount++;
			flag = 1;
			break;
		case EIT_TABLE_ID_ACTUAL:
		case EIT_TABLE_ID_ACTUAL_SHEDULE:
		case EIT_TABLE_ID_OTHER:
		case EIT_TABLE_ID_OTHER_SHEDULE:
			stTwoLevelParam[chanID].stTwoLevel.stCrcErrorInfo.eitCrcErrorCount++;
			stTwoLevelParam[chanID].stTwoLevel.stCrcErrorInfo.totalCrcErrorCount++;
			flag = 1;
			break;
		case TDT_TABLE_ID:
		case TOT_TABLE_ID:	
		case RST_TABLE_ID:
		case SI_TABLE_ID:
			break;
		default:
			break;
	}
	if(flag == 1)
		TS_ERROR_LOG(chanID,TYPE_ERROR_TWO,TR101290_TWO_ERROR_CRC,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_TwoLevel_checkPcrDiscontError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U32 index,GOSTSR_U64 u64Pcr_Base, GOSTSR_U16 u16Pcr_Ext,GOSTSR_BOOL *bErrorFlag)
{
	GOSTSR_U32 u32PcrValueNs_Diff = 0;
	GOSTSR_U32 u32PcrValueMs_Diff = 0;
	GOSTSR_U8 chanID = 0;

	if((pstErrorInfo == GOSTSR_NULL) || (bErrorFlag == GOSTSR_NULL) || (pstErrorInfo->chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	chanID = pstErrorInfo->chanID;
	/*若是第一次使用，先备份数据*/
	if(stTwoLevelParam[chanID].stPcrInfo[index].bFirstUsed)
	{
		stTwoLevelParam[chanID].stPcrInfo[index].bFirstUsed = GOSTSR_FALSE;
		stTwoLevelParam[chanID].stPcrInfo[index].u64PcrBase = u64Pcr_Base;
		stTwoLevelParam[chanID].stPcrInfo[index].u16PcrExt = u16Pcr_Ext;
		stTwoLevelParam[chanID].stPcrInfo[index].u32PcrValueNs = TSERROR_INVALID_U32;
		*bErrorFlag = GOSTSR_FALSE;
		return GOSTSR_FAILURE;
	}
	else
	{
		if(u64Pcr_Base >= stTwoLevelParam[chanID].stPcrInfo[index].u64PcrBase)
		{
			if(u16Pcr_Ext >= stTwoLevelParam[chanID].stPcrInfo[index].u16PcrExt)
			{
				u32PcrValueNs_Diff = ((u64Pcr_Base-stTwoLevelParam[chanID].stPcrInfo[index].u64PcrBase)*300+(u16Pcr_Ext-stTwoLevelParam[chanID].stPcrInfo[index].u16PcrExt))*1000/27;
			}
			else
			{
				u32PcrValueNs_Diff = ((u64Pcr_Base-1-stTwoLevelParam[chanID].stPcrInfo[index].u64PcrBase)*300+(0x1ff+u16Pcr_Ext-stTwoLevelParam[chanID].stPcrInfo[index].u16PcrExt))*1000/27;
			}
		}
		else
		{
			if(u16Pcr_Ext >= stTwoLevelParam[chanID].stPcrInfo[index].u16PcrExt)
			{
				u32PcrValueNs_Diff = ((0x1ffffffffULL - stTwoLevelParam[chanID].stPcrInfo[index].u64PcrBase+u64Pcr_Base)*300+(u16Pcr_Ext-stTwoLevelParam[chanID].stPcrInfo[index].u16PcrExt))*1000/27;
			}
			else
			{			
				u32PcrValueNs_Diff = ((0x1ffffffffULL - stTwoLevelParam[chanID].stPcrInfo[index].u64PcrBase+(u64Pcr_Base-1))*300+(0x1ff+u16Pcr_Ext-stTwoLevelParam[chanID].stPcrInfo[index].u16PcrExt))*1000/27;
			}
		}

		stTwoLevelParam[chanID].stPcrInfo[index].u32PcrValueNs = u32PcrValueNs_Diff;
		stTwoLevelParam[chanID].stPcrInfo[index].u64PcrBase = u64Pcr_Base;
		stTwoLevelParam[chanID].stPcrInfo[index].u16PcrExt = u16Pcr_Ext;
		
		/*ns 转换为 ms*/
		u32PcrValueMs_Diff = u32PcrValueNs_Diff / 1000 /1000;
		if(u32PcrValueMs_Diff  > 40)
		{			
			stTwoLevelParam[chanID].stTwoLevel.u32PcrDisErrorCount++;
			TS_ERROR_LOG(chanID,TYPE_ERROR_TWO,TR101290_TWO_ERROR_PCRDISCONT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
			*bErrorFlag = GOSTSR_TRUE;
		}
	}
	
	return GOSTSR_SUCCESS;
}

//PCR抖动算法有待优化
GOSTSR_S32 TsErrorCheck_TwoLevel_checkPcrAccuracyError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U32 index,GOSTSR_U64 u64Pcr_Base, GOSTSR_U16 u64Pcr_Ext)
{
	GOSTSR_U32 u32PcrValuePcr_Ns = 0;	/*两个PCR之间的差值*/
	GOSTSR_U32 u32PcrValueBit_Ns = 0;	/*两个Bit之间的差值*/	
	GOSTSR_U32 u32PcrValueDiff_Ns = 0;
	GOSTSR_U32 u32byteOffset = 0;
	GOSTSR_F32 f32TsRateUs = 0.0;
	GOSTSR_U8 chanID = 0;

	if((pstErrorInfo == GOSTSR_NULL) || (pstErrorInfo->chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	chanID = pstErrorInfo->chanID;
	if((stTwoLevelParam[chanID].stPcrInfo[index].u32BytePos == 0) || (stTwoLevelParam[chanID].stPcrInfo[index].u32PcrValueNs == TSERROR_INVALID_U32)) /*第一个PCR的值，尚不能进行比较*/
	{
		stTwoLevelParam[chanID].stPcrInfo[index].u32BytePos = pstErrorInfo->bytePos;
		return GOSTSR_FAILURE;
	}
	else
	{
		/*获取连续两个PCR之间通过bit计算得到的PCR时间间隔,Unit:us*/
		u32PcrValuePcr_Ns =stTwoLevelParam[chanID].stPcrInfo[index].u32PcrValueNs;
		
		f32TsRateUs = TwoLevel_getTsRateUs(chanID); /*通过平均码率计算*/
		if(f32TsRateUs == 0.0)
		{	
			return GOSTSR_FAILURE;
		}	

		if(pstErrorInfo->bCarryFlag)
		{
			u32byteOffset = TWOLEVEL_BYTEPOS_MAX+pstErrorInfo->bytePos - stTwoLevelParam[chanID].stPcrInfo[index].u32BytePos;
		}
		else
		{
			if(pstErrorInfo->bytePos >=  stTwoLevelParam[chanID].stPcrInfo[index].u32BytePos)
			{
				u32byteOffset = pstErrorInfo->bytePos - stTwoLevelParam[chanID].stPcrInfo[index].u32BytePos;
			}
			else
			{
				u32byteOffset =  stTwoLevelParam[chanID].stPcrInfo[index].u32BytePos - pstErrorInfo->bytePos;
			}
		}
		stTwoLevelParam[chanID].stPcrInfo[index].u32BytePos = pstErrorInfo->bytePos;
		
		u32PcrValueBit_Ns = (GOSTSR_U32)(1.0*u32byteOffset*8*1000/f32TsRateUs);	
		
		if(abs(u32PcrValueBit_Ns - u32PcrValuePcr_Ns) > 500)
		{
			stTwoLevelParam[chanID].stTwoLevel.u32PcrJitErrorCount++;
			u32PcrValueDiff_Ns = abs(u32PcrValueBit_Ns - u32PcrValuePcr_Ns);
			TS_ERROR_LOG(chanID,TYPE_ERROR_TWO,TR101290_TWO_ERROR_ACCURACY,pstErrorInfo->bytePos,(GOSTSR_U8 *)&u32PcrValueDiff_Ns,GOSTSR_NULL);
		}
	}
	
	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_TwoLevel_checkPcrError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U16 u16PcrPid,GOSTSR_U64 u64Pcr_Base, GOSTSR_U16 u64Pcr_Ext, GOSTSR_BOOL flag)
{
	GOSTSR_U32 index = 0;
	GOSTSR_BOOL bErrorFlag = GOSTSR_FALSE; 
	GOSTSR_U8 chanID = pstErrorInfo->chanID;

	if((pstErrorInfo == GOSTSR_NULL) || (!pstErrorInfo->bisMatched || (pstErrorInfo->chanID >= TSERROR_CHANNELID_MAX)))
	{
		return GOSTSR_FAILURE;
	}
	/*检测是不是PCR_PID*/
	if(!TwoLevel_checkisPcrPid(chanID,u16PcrPid, &index))
	{
		return GOSTSR_FAILURE;
	}

	/*不连续状态标志为1*/
	if (!flag)
	{
		stTwoLevelParam[chanID].stTsRateInfo[index].bFirstUsed = GOSTSR_TRUE;
		stTwoLevelParam[chanID].stTsRateInfo[index].u32TransportRate = 0;
		stTwoLevelParam[chanID].stPcrInfo[index].bFirstUsed = GOSTSR_TRUE;
		stTwoLevelParam[chanID].stPcrInfo[index].u32BytePos = 0;
		return GOSTSR_FAILURE;
	}

	/*PCR间隔检测*/
	TsErrorCheck_TwoLevel_checkPcrDiscontError(pstErrorInfo,index,u64Pcr_Base,u64Pcr_Ext,&bErrorFlag);
	
	/*PCR抖动检测*/
	TsErrorCheck_TwoLevel_checkPcrAccuracyError(pstErrorInfo,index,u64Pcr_Base,u64Pcr_Ext);

	if(!bErrorFlag)
	{
		/*码率的设置*/
		stTwoLevelParam[chanID].stTsRateInfo[index].u16PcrPid = u16PcrPid;
		TsErrorCheck_TwoLevel_setTransportRate(pstErrorInfo,index,u64Pcr_Base,u64Pcr_Ext);		
	}

	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_TwoLevel_checkPtsError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U16 u16PesPid)
{
	GOSTSR_U32 u32PtsTime = 0; /*Unit: ms*/
	GOSTSR_F32 f32PcrMsRate = 0;
	GOSTSR_U32 index = 0;
	GOSTSR_U32 u32byteOffset = 0;
	GOSTSR_U8 chanID = pstErrorInfo->chanID;

	if((pstErrorInfo == GOSTSR_NULL) || (!pstErrorInfo->bisMatched) || (!stTwoLevelParam[chanID].bTsRateisRecved) || (pstErrorInfo->chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	/*检测是不是Pes_PID,返回索引*/
	if(!TwoLevel_checkisPesPid(chanID,u16PesPid, &index))
	{
		return GOSTSR_FAILURE;
	}
	
	if((stTwoLevelParam[chanID].stPesInfo[index].bFirstUsed) || (stTwoLevelParam[chanID].stPesInfo[index].u32BytePos == 0))	/*当前包的字节数*/
	{
		stTwoLevelParam[chanID].stPesInfo[index].bFirstUsed = GOSTSR_FALSE;
		stTwoLevelParam[chanID].stPesInfo[index].u32BytePos = pstErrorInfo->bytePos;
		return GOSTSR_FAILURE;
	}
	
	f32PcrMsRate = TwoLevel_getTsRateMs(chanID);
	if(f32PcrMsRate == 0.0)
	{	
		return GOSTSR_FAILURE;
	}
	
	if(pstErrorInfo->bCarryFlag)
	{
		u32byteOffset = TWOLEVEL_BYTEPOS_MAX+pstErrorInfo->bytePos - stTwoLevelParam[chanID].stPesInfo[index].u32BytePos;
	}
	else
	{
		if(pstErrorInfo->bytePos >=  stTwoLevelParam[chanID].stPesInfo[index].u32BytePos)
		{
			u32byteOffset = pstErrorInfo->bytePos - stTwoLevelParam[chanID].stPesInfo[index].u32BytePos;
		}
		else
		{
			u32byteOffset =  stTwoLevelParam[chanID].stPesInfo[index].u32BytePos - pstErrorInfo->bytePos;
		}
	}
	stTwoLevelParam[chanID].stPesInfo[index].u32BytePos  = pstErrorInfo->bytePos;

	u32PtsTime =(GOSTSR_U32)(1.0 * u32byteOffset * 8 / f32PcrMsRate);
	if(u32PtsTime > 700)
	{
		stTwoLevelParam[chanID].stTwoLevel.u32PtsErrorCount++;
		TS_ERROR_LOG(chanID,TYPE_ERROR_TWO,TR101290_TWO_ERROR_PTS,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
		return GOSTSR_SUCCESS;
	}
	return GOSTSR_FAILURE;
}

GOSTSR_S32 TsErrorCheck_TwoLevel_checkCatError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U16 u16Pid, GOSTSR_U8 u8TableID, GOSTSR_U8 u8Scramble)
{
	GOSTSR_U8 chanID = 0;
	
	if((pstErrorInfo == GOSTSR_NULL) || (!pstErrorInfo->bisMatched) || (u16Pid != CAT_PID) || (pstErrorInfo->chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	chanID = pstErrorInfo->chanID;
	if(u8TableID != CAT_TABLE_ID)
	{
		stTwoLevelParam[chanID].stTwoLevel.stCatErrorInfo.tableIDErrorCount++;
		stTwoLevelParam[chanID].stTwoLevel.stCatErrorInfo.totalCatErrorCount++;
		TS_ERROR_LOG(chanID,TYPE_ERROR_TWO,TR101290_TWO_ERROR_CAT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
	}

	if(u8Scramble != 0)
	{
		stTwoLevelParam[chanID].stTwoLevel.stCatErrorInfo.scambleErrorCount++;
		stTwoLevelParam[chanID].stTwoLevel.stCatErrorInfo.totalCatErrorCount++;
		TS_ERROR_LOG(chanID,TYPE_ERROR_TWO,TR101290_TWO_ERROR_CAT,pstErrorInfo->bytePos,GOSTSR_NULL,GOSTSR_NULL);
	}
	
	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_TwoLevel_getTwoLevelError(GOSTSR_U8 chanID,TSERROR_TWOLEVEL_S *pTwoLevel)
{
	if((pTwoLevel == GOSTSR_NULL)  || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	memcpy(pTwoLevel, &stTwoLevelParam[chanID].stTwoLevel, sizeof(TSERROR_TWOLEVEL_S));

	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_TwoLevel_Init()
{
	GOSTSR_S32 i = 0, j = 0;
	
	memset(&stTwoLevelParam, 0x00, sizeof(TSERROR_TWOLEVEL_PARAM_S) * TSERROR_CHANNELID_MAX);

	for(i = 0; i < TSERROR_CHANNELID_MAX; i++)
	{
		for(j = 0 ; j < PCRPID_NUM_MAX; j++)
		{
			stTwoLevelParam[i].stPcrInfo[j].bFirstUsed = GOSTSR_TRUE;
			stTwoLevelParam[i].stPcrInfo[j].u16PcrPid = TSERROR_INVALID_U16;

			stTwoLevelParam[i].stTsRateInfo[j].bFirstUsed = GOSTSR_TRUE;
			stTwoLevelParam[i].stTsRateInfo[j].u16PcrPid = TSERROR_INVALID_U16;
		}
		for(j = 0 ; j < PESPID_NUM_MAX; j++)
		{
			stTwoLevelParam[i].stPesInfo[j].bFirstUsed = GOSTSR_TRUE;
			stTwoLevelParam[i].stPesInfo[j].u16PesPid = TSERROR_INVALID_U16;
		}
		stTwoLevelParam[i].bTsRateisRecved = GOSTSR_FALSE;
	}
	
	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_TwoLevel_DeInit()
{
	GOSTSR_S32 i = 0, j = 0;
	
	memset(&stTwoLevelParam, 0x00, sizeof(TSERROR_TWOLEVEL_PARAM_S) * TSERROR_CHANNELID_MAX);

	for(i = 0; i < TSERROR_CHANNELID_MAX; i++)
	{
		for(j = 0 ; j < PCRPID_NUM_MAX; j++)
		{
			stTwoLevelParam[i].stPcrInfo[j].bFirstUsed = GOSTSR_TRUE;
			stTwoLevelParam[i].stPcrInfo[j].u16PcrPid = TSERROR_INVALID_U16;

			stTwoLevelParam[i].stTsRateInfo[j].bFirstUsed = GOSTSR_TRUE;
			stTwoLevelParam[i].stTsRateInfo[j].u16PcrPid = TSERROR_INVALID_U16;
		}
		for(j = 0 ; j < PESPID_NUM_MAX; j++)
		{
			stTwoLevelParam[i].stPesInfo[j].bFirstUsed = GOSTSR_TRUE;
			stTwoLevelParam[i].stPesInfo[j].u16PesPid = TSERROR_INVALID_U16;
		}
		stTwoLevelParam[i].bTsRateisRecved = GOSTSR_FALSE;
	}
	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_TwoLevel_ReInit(GOSTSR_U8 chanID)
{
	GOSTSR_S32 j = 0;

	if((chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	memset(&stTwoLevelParam[chanID], 0x00, sizeof(TSERROR_TWOLEVEL_PARAM_S));

	for(j = 0 ; j < PCRPID_NUM_MAX; j++)
	{
		stTwoLevelParam[chanID].stPcrInfo[j].bFirstUsed = GOSTSR_TRUE;
		stTwoLevelParam[chanID].stPcrInfo[j].u16PcrPid = TSERROR_INVALID_U16;

		stTwoLevelParam[chanID].stTsRateInfo[j].bFirstUsed = GOSTSR_TRUE;
		stTwoLevelParam[chanID].stTsRateInfo[j].u16PcrPid = TSERROR_INVALID_U16;
	}
	for(j = 0 ; j < PESPID_NUM_MAX; j++)
	{
		stTwoLevelParam[chanID].stPesInfo[j].bFirstUsed = GOSTSR_TRUE;
		stTwoLevelParam[chanID].stPesInfo[j].u16PesPid = TSERROR_INVALID_U16;
	}
	stTwoLevelParam[chanID].bTsRateisRecved = GOSTSR_FALSE;
	
	return GOSTSR_SUCCESS;
}


