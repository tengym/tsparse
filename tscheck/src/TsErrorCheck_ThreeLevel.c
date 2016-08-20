#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#include "TsErrorCheck_TwoLevel.h"
#include "TsErrorCheck_ThreeLevel.h"
#include "TsErrorCheck_Log.h"

static TSERROR_THREELEVEL_PARAM_S stThreeLevelParam[TSERROR_CHANNELID_MAX];

/*nit error check functions*/
static GOSTSR_S32 TsCheck_NitSectionsTimeError(TS_SECTION_INFO *sectionInfo);

/*si repetition rate error check functions*/
static GOSTSR_S32 TsCheck_SiSectionRepetitionRateError(TS_SECTION_INFO *sectionInfo);

/*buffer error check functions*/
static GOSTSR_S32 TsCheck_BufferError(GOSTSR_U8 *data, GOSTSR_U16 dataLen, GOSTSR_U32 time);

/*unreferenced pid error check functions*/
static GOSTSR_S32 TsCheck_UnreferencedPidError(TS_HEAD_INFO *tsHeadInfo);

/*sdt error check functions*/
static GOSTSR_S32 TsCheck_SdtActualSectionsTimeError(TS_SECTION_INFO *sectionInfo);
static GOSTSR_S32 TsCheck_SdtOtherSectionsTimeError(TS_SECTION_INFO *sectionInfo);

/*eit error check functions*/
static GOSTSR_S32 TsCheck_EitActualSectionsTimeError(TS_SECTION_INFO *sectionInfo);
static GOSTSR_S32 TsCheck_EitOtherSectionsTimeError(TS_SECTION_INFO *sectionInfo);

/*rst error check functions*/
static GOSTSR_S32 TsCheck_RstSectionsTimeError(TS_SECTION_INFO *sectionInfo);

/*tdt error check functions*/
static GOSTSR_S32 TsCheck_TdtSectionsTimeError(TS_SECTION_INFO *sectionInfo);

/*empty buffer error check functions*/
static GOSTSR_S32 TsCheck_EmptyBufferError(GOSTSR_U8 *data, GOSTSR_U16 dataLen, GOSTSR_U32 time);

/*date delay error check functions*/
static GOSTSR_S32 TsCheck_DataDelayError(GOSTSR_U8 *data, GOSTSR_U16 dataLen, GOSTSR_U32 time);

static GOSTSR_S32 TsCheck_GetCurrentSectionInfo(TS_SECTION_INFO *sectionInfo, GOSTSR_U8 *sectionNum, GOSTSR_U8 *lastSectionNum)
{
	GOSTSR_U8  *pData = GOSTSR_NULL;

	if ((GOSTSR_NULL == sectionInfo) || (GOSTSR_NULL == sectionInfo->sectionData) || (GOSTSR_NULL == sectionNum) || (GOSTSR_NULL == lastSectionNum))
	{
		return GOSTSR_SUCCESS;
	}
	
	pData = sectionInfo->sectionData;
	
    	pData += 6;

    	/*section number*/
    	*sectionNum = pData[0];
	pData += 1;

	/*last section number*/
	*lastSectionNum = pData[0];

	return GOSTSR_SUCCESS;
}

static GOSTSR_S32 TsCheck_TableOverTime(TS_SECTION_INFO *sectionInfo, SiRepetRateSections_Info *sectionInfo_Pre, GOSTSR_U32 overTime)
{
	GOSTSR_U8  sectionNum = 0;
	GOSTSR_U8  lastSectionNum = 0;
	GOSTSR_U32 timeOffset = 0;
	GOSTSR_S32 retVal = GOSTSR_FAILURE;
	GOSTSR_U8  maxNumber = 0;
	GOSTSR_U8 chanID = sectionInfo->stErrorInfo.chanID;

	if ((GOSTSR_NULL == sectionInfo) || (GOSTSR_NULL == sectionInfo_Pre) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	
	if (GOSTSR_SUCCESS != TsCheck_GetCurrentSectionInfo(sectionInfo, &sectionNum, &lastSectionNum))
	{
		return GOSTSR_FAILURE;
	}

	if (lastSectionNum == 0)
	{
		return GOSTSR_FAILURE;
	}
	
	if (sectionInfo_Pre->number == 0)
	{
		sectionInfo_Pre->startTime = sectionInfo->stErrorInfo.startTime;
		sectionInfo_Pre->lastSectionNum = lastSectionNum;
	}

	if (sectionInfo_Pre->sectionFlag[sectionNum] == 0x01)
	{
		return GOSTSR_FAILURE;
	}
	else
	{
		sectionInfo_Pre->sectionFlag[sectionNum] = 0x01;
		sectionInfo_Pre->number++;
	}

	if ((sectionInfo->tableID == EIT_TABLE_ID_ACTUAL_SHEDULE) || (sectionInfo->tableID == EIT_TABLE_ID_OTHER_SHEDULE))
	{
		maxNumber = sectionInfo_Pre->lastSectionNum / 8; 
	}
	else
	{
		maxNumber = sectionInfo_Pre->lastSectionNum;
	}
	
	if (sectionInfo_Pre->number > maxNumber)
	{
		if(sectionInfo->stErrorInfo.bCarryFlag)
		{
			timeOffset = 0xffffffff+sectionInfo->stErrorInfo.startTime - sectionInfo_Pre->startTime;
		}
		else
		{
			if(sectionInfo->stErrorInfo.startTime >=  sectionInfo_Pre->startTime)
			{
				timeOffset = sectionInfo->stErrorInfo.startTime - sectionInfo_Pre->startTime;
			}
			else
			{
				timeOffset =  sectionInfo_Pre->startTime - sectionInfo->stErrorInfo.startTime;
			}
		}
			
		if (timeOffset <= 25000)/*SI表的间隔<= 25ms*/
		{
			stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.siSectionLimitTimeError++;
			stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.totalError++;
			TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SIREPETITION,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
		}

		if (timeOffset > overTime)
		{
			retVal = GOSTSR_SUCCESS;
		}

		sectionInfo_Pre->startTime = 0;
		sectionInfo_Pre->lastSectionNum = 0;
		sectionInfo_Pre->number = 0;
		memset(sectionInfo_Pre->sectionFlag,0,sizeof(sectionInfo_Pre->sectionFlag));
	}
	
	return retVal;
}

static GOSTSR_S32 TsCheck_NitSectionsTimeError(TS_SECTION_INFO *sectionInfo)
{
	GOSTSR_U32 timeOffset = 0;
	GOSTSR_U8 chanID = sectionInfo->stErrorInfo.chanID;
		
	if ((GOSTSR_NULL == sectionInfo) || (NIT_PID != sectionInfo->PID) || (GOSTSR_NULL == sectionInfo->sectionData) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	
	/*check nit table ID error*/
	if ((NIT_TABLE_ID_ACTUAL != sectionInfo->tableID) && (NIT_TABLE_ID_OTHER != sectionInfo->tableID) && (SI_TABLE_ID != sectionInfo->tableID))
	{
		stThreeLevelParam[chanID].errorInfo.nitErrorInfo.tableIDError++;
		stThreeLevelParam[chanID].errorInfo.nitErrorInfo.totalError++;
		TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_NITACTUAL,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
	}

	/*check nit actual time error*/
	if (NIT_TABLE_ID_ACTUAL == sectionInfo->tableID)
	{
		if (stThreeLevelParam[chanID].nitActualPreTime != 0)
		{
			if(sectionInfo->stErrorInfo.bCarryFlag)
			{
				timeOffset = 0xffffffff+sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].nitActualPreTime;
			}
			else
			{
				if(sectionInfo->stErrorInfo.startTime >=  stThreeLevelParam[chanID].nitActualPreTime)
				{
					timeOffset = sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].nitActualPreTime;
				}
				else
				{
					timeOffset = stThreeLevelParam[chanID].nitActualPreTime - sectionInfo->stErrorInfo.startTime;
				}
			}

			/*timeoffset over 10s*/
			if (timeOffset > 10000000)
			{
				stThreeLevelParam[chanID].errorInfo.nitErrorInfo.actualOverTimeError++;
				stThreeLevelParam[chanID].errorInfo.nitErrorInfo.totalError++;
				TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_NITACTUAL,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
			}
			/*timeoffset within 25ms*/
			else if (timeOffset <= 25000)
			{
				stThreeLevelParam[chanID].errorInfo.nitErrorInfo.actualLimitTimeError++;
				stThreeLevelParam[chanID].errorInfo.nitErrorInfo.totalError++;
				TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_NITACTUAL,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
			}
		}
	}
	/*check nit other time error*/
	else if (NIT_TABLE_ID_OTHER == sectionInfo->tableID)
	{
		if (stThreeLevelParam[chanID].nitOtherPreTime != 0)
		{
			if(sectionInfo->stErrorInfo.bCarryFlag)
			{
				timeOffset = 0xffffffff+sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].nitOtherPreTime;
			}
			else
			{
				if(sectionInfo->stErrorInfo.startTime >=  stThreeLevelParam[chanID].nitOtherPreTime)
				{
					timeOffset = sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].nitOtherPreTime;
				}
				else
				{
					timeOffset = stThreeLevelParam[chanID].nitOtherPreTime - sectionInfo->stErrorInfo.startTime;
				}
			}

			/*timeoffset over 10s*/
			if (timeOffset > 10000000)
			{
				stThreeLevelParam[chanID].errorInfo.nitErrorInfo.otherOverTimeError++;
				stThreeLevelParam[chanID].errorInfo.nitErrorInfo.totalError++;
				TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_NITOTHER,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
			}
			/*timeoffset within 25ms*/
			else if (timeOffset <= 25000)
			{
				stThreeLevelParam[chanID].errorInfo.nitErrorInfo.otherLimitTimeError++;
				stThreeLevelParam[chanID].errorInfo.nitErrorInfo.totalError++;
				TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_NITOTHER,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
			}
		}
	}
	stThreeLevelParam[chanID].nitOtherPreTime = sectionInfo->stErrorInfo.startTime;
	
	return GOSTSR_SUCCESS;
}

static GOSTSR_S32 TsCheck_SiSectionRepetitionRateError(TS_SECTION_INFO *sectionInfo)
{
	GOSTSR_U32 timeOffset = 0;
	GOSTSR_U8  sectionNum = 0;
	GOSTSR_U8  lastSectionNum = 0;
	GOSTSR_U8 chanID = sectionInfo->stErrorInfo.chanID;
	
	if ((GOSTSR_NULL == sectionInfo) || (GOSTSR_NULL == sectionInfo->sectionData) || (sectionInfo->PID < SI_PID_MIN) \
		|| (sectionInfo->PID > SI_PID_MAX) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}

	switch(sectionInfo->PID)
	{
		case NIT_PID:
			if (GOSTSR_SUCCESS == TsCheck_GetCurrentSectionInfo(sectionInfo, &sectionNum, &lastSectionNum))
			{
				GOSTSR_U32 nitTime = 0x00;
				
				if (NIT_TABLE_ID_ACTUAL == sectionInfo->tableID)
				{
					if (lastSectionNum == 0)
					{
						break;
					}
					
					if (stThreeLevelParam[chanID].nitActualSectionInfo.number == 0)
					{						
						if (stThreeLevelParam[chanID].nitOtherSectionInfo.number == 0)
						{
							stThreeLevelParam[chanID].nitActualSectionInfo.startTime = sectionInfo->stErrorInfo.startTime;
						}
						stThreeLevelParam[chanID].nitActualSectionInfo.lastSectionNum = lastSectionNum;
					}

					if (stThreeLevelParam[chanID].nitActualSectionInfo.sectionFlag[sectionNum] == 0x01)
					{
						break;
					}
					else
					{
						stThreeLevelParam[chanID].nitActualSectionInfo.sectionFlag[sectionNum] = 0x01;
						stThreeLevelParam[chanID].nitActualSectionInfo.number++;
					}
				}
				else if (NIT_TABLE_ID_OTHER == sectionInfo->tableID)
				{
					if (lastSectionNum == 0)
					{
						break;
					}
					
					if (stThreeLevelParam[chanID].nitOtherSectionInfo.number == 0)
					{		
						if (stThreeLevelParam[chanID].nitActualSectionInfo.number == 0)
						{
							stThreeLevelParam[chanID].nitOtherSectionInfo.startTime = sectionInfo->stErrorInfo.startTime;
						}
						stThreeLevelParam[chanID].nitOtherSectionInfo.lastSectionNum = lastSectionNum;
					}

					if (stThreeLevelParam[chanID].nitOtherSectionInfo.sectionFlag[sectionNum] == 0x01)
					{
						break;
					}
					else
					{
						stThreeLevelParam[chanID].nitOtherSectionInfo.sectionFlag[sectionNum] = 0x01;
						stThreeLevelParam[chanID].nitOtherSectionInfo.number++;
					}
				}

				/*over 10s*/
				if (((stThreeLevelParam[chanID].nitActualSectionInfo.number > stThreeLevelParam[chanID].nitActualSectionInfo.lastSectionNum) && (stThreeLevelParam[chanID].nitOtherSectionInfo.number > stThreeLevelParam[chanID].nitOtherSectionInfo.lastSectionNum)) ||
					  ((stThreeLevelParam[chanID].nitActualSectionInfo.number == 0) && (stThreeLevelParam[chanID].nitOtherSectionInfo.number > stThreeLevelParam[chanID].nitOtherSectionInfo.lastSectionNum)) ||
					  ((stThreeLevelParam[chanID].nitActualSectionInfo.number > stThreeLevelParam[chanID].nitActualSectionInfo.lastSectionNum) && (stThreeLevelParam[chanID].nitOtherSectionInfo.number == 0)))
				{
					if (stThreeLevelParam[chanID].nitOtherSectionInfo.startTime != 0)
					{
						nitTime = stThreeLevelParam[chanID].nitOtherSectionInfo.startTime;
					}
					else
					{
						nitTime = stThreeLevelParam[chanID].nitActualSectionInfo.startTime;
					}
					
					if(sectionInfo->stErrorInfo.bCarryFlag)
					{
						timeOffset = 0xffffffff+sectionInfo->stErrorInfo.startTime - nitTime;
					}
					else
					{
						if(sectionInfo->stErrorInfo.startTime >=  nitTime)
						{
							timeOffset = sectionInfo->stErrorInfo.startTime - nitTime;
						}
						else
						{
							timeOffset = nitTime - sectionInfo->stErrorInfo.startTime;
						}
					}
	
					if (timeOffset <= 25000)/*SI表的间隔<= 25ms*/
					{
						stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.siSectionLimitTimeError++;
						stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.totalError++;
						TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SIREPETITION,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
					}
					if (timeOffset >= 10000000)
					{
						stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.nitOverTimeError++;
						stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.totalError++;
						TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SIREPETITION,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
					}

					memset(&stThreeLevelParam[chanID].nitOtherSectionInfo, 0x00, sizeof(SiRepetRateSections_Info));
					memset(&stThreeLevelParam[chanID].nitActualSectionInfo, 0x00, sizeof(SiRepetRateSections_Info));
				}
			}
			break;
			
		case BAT_SDT_PID:
			if (BAT_TABLE_ID == sectionInfo->tableID)
			{
				/*over 10s*/
				if (GOSTSR_SUCCESS == TsCheck_TableOverTime(sectionInfo, &stThreeLevelParam[chanID].batSectionInfo, 10000000))
				{
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.batOverTimeError++;
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.totalError++;
					TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SIREPETITION,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
				}
			}
			else if (SDT_TABLE_ID_ACTUAL == sectionInfo->tableID)
			{
				/*over 2s*/
				if (GOSTSR_SUCCESS == TsCheck_TableOverTime(sectionInfo, &stThreeLevelParam[chanID].sdtActualSectionInfo, 2000000))
				{
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.sdtActualOverTimeError++;
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.totalError++;
					TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SIREPETITION,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
				}
			}
			else if (SDT_TABLE_ID_OTEHR == sectionInfo->tableID)
			{
				/*over 10s*/
				if (GOSTSR_SUCCESS == TsCheck_TableOverTime(sectionInfo, &stThreeLevelParam[chanID].sdtOtherSectionInfo, 10000000))
				{
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.sdtOtherOverTimeError++;
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.totalError++;
					TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SIREPETITION,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
				}
			}
			break;

		case EIT_PID:
			if (EIT_TABLE_ID_ACTUAL == sectionInfo->tableID)
			{
				/*over 2s*/
				if (GOSTSR_SUCCESS == TsCheck_TableOverTime(sectionInfo, &stThreeLevelParam[chanID].batSectionInfo, 2000000))
				{
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.eitActualOverTimeError++;
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.totalError++;
					TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SIREPETITION,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
				}
			}
			else if (EIT_TABLE_ID_OTHER == sectionInfo->tableID)
			{
				/*over 10s*/
				if (GOSTSR_SUCCESS == TsCheck_TableOverTime(sectionInfo, &stThreeLevelParam[chanID].eitOtherSectionInfo, 10000000))
				{
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.eitOtherOverTimeError++;
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.totalError++;
					TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SIREPETITION,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
				}
			}
			else if (EIT_TABLE_ID_ACTUAL_SHEDULE == sectionInfo->tableID)
			{
				/*over 10s*/
				if (GOSTSR_SUCCESS == TsCheck_TableOverTime(sectionInfo, &stThreeLevelParam[chanID].eitActualScheduleSectionInfo, 10000000))
				{
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.eitActualScheduleOverTimeError++;
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.totalError++;
					TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SIREPETITION,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
				}
			}
			else if (EIT_TABLE_ID_OTHER_SHEDULE == sectionInfo->tableID)
			{
				/*over 30s*/
				if (GOSTSR_SUCCESS == TsCheck_TableOverTime(sectionInfo, &stThreeLevelParam[chanID].eitOtherScheduleSectionInfo, 30000000))
				{
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.eitOtherScheduleOverTimeError++;
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.totalError++;
					TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SIREPETITION,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
				}
			}
			break;

		case TDT_TOT_PID:
			if (TDT_TABLE_ID == sectionInfo->tableID)
			{
				/*over 30s*/
				if (GOSTSR_SUCCESS == TsCheck_TableOverTime(sectionInfo, &stThreeLevelParam[chanID].tdtSectionInfo, 30000000))
				{
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.tdtOverTimeError++;
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.totalError++;
					TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SIREPETITION,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
				}
			}
			else if (TOT_TABLE_ID == sectionInfo->tableID)
			{
				/*over 30s*/
				if (GOSTSR_SUCCESS == TsCheck_TableOverTime(sectionInfo, &stThreeLevelParam[chanID].totSectionInfo, 30000000))
				{
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.totOverTimeError++;
					stThreeLevelParam[chanID].errorInfo.siRepetitionRateErrorInfo.totalError++;
					TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SIREPETITION,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
				}
			}
			break;
			
		default:
			break;
	}

	return GOSTSR_SUCCESS;
}

static GOSTSR_S32 TsCheck_BufferError(GOSTSR_U8 *data, GOSTSR_U16 dataLen, GOSTSR_U32 time)
{
	if ((GOSTSR_NULL == data) || (dataLen == 0))
	{
		return GOSTSR_FAILURE;
	}

	/*TODO*/

	return GOSTSR_SUCCESS;
}

static GOSTSR_S32 TsCheck_UnreferencedPidError(TS_HEAD_INFO *tsHeadInfo)
{
	GOSTSR_U16 index = 0x00;
	GOSTSR_U32 timeOffset = 0;
	GOSTSR_S32 i = 0,j = 0;
	GOSTSR_U8 chanID = 0;

	if((GOSTSR_NULL == tsHeadInfo) || (tsHeadInfo->stErrorInfo.chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_NULL;
	}
	 chanID = tsHeadInfo->stErrorInfo.chanID;
	if ((tsHeadInfo->ts_pid >= 0x20) && (tsHeadInfo->ts_pid != 0x1fff))
	{
		/*check PMTs,PesPID,PcrPID*/
		for(i = 0; i < stThreeLevelParam[chanID].stSearchInfo.u16NbProg; i++)
		{
			if((tsHeadInfo->ts_pid  == stThreeLevelParam[chanID].stSearchInfo.stProgInfo[i].PmtPid) || ((tsHeadInfo->ts_pid  ==  stThreeLevelParam[chanID].stSearchInfo.stProgInfo[i].PcrPid)))
			{
				return GOSTSR_FAILURE;
			}
			for(j = 0; j < stThreeLevelParam[chanID].stSearchInfo.stProgInfo[i].u8NbPes; j++)
			{
				if(tsHeadInfo->ts_pid  == stThreeLevelParam[chanID].stSearchInfo.stProgInfo[i].PesPid[j])
				{
					return GOSTSR_FAILURE;
				}
			}
		}

		/*check CATs*/
		for(i = 0; i < stThreeLevelParam[chanID].stSearchInfo.u16NbCa; i++)
		{
			if(tsHeadInfo->ts_pid == stThreeLevelParam[chanID].stSearchInfo.stCaInfo[i].CaPid)
			{
				return GOSTSR_FAILURE;
			}
		}
		
		/*check unferenced pid list over time status*/
		for (i = 0; i < MAX_UNREFERPID_NUM; i++)
		{
			if ((!stThreeLevelParam[chanID].unreferPidInfo[i].flag) && (stThreeLevelParam[chanID].unreferPidInfo[i].pid != tsHeadInfo->ts_pid ))
			{
				continue;
			}
			if(tsHeadInfo->stErrorInfo.bCarryFlag)
			{
				timeOffset = 0xffffffff+tsHeadInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].unreferPidInfo[i].startTime;
			}
			else
			{
				if(tsHeadInfo->stErrorInfo.startTime >=  stThreeLevelParam[chanID].unreferPidInfo[i].startTime)
				{
					timeOffset = tsHeadInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].unreferPidInfo[i].startTime;
				}
				else
				{
					timeOffset = stThreeLevelParam[chanID].unreferPidInfo[i].startTime - tsHeadInfo->stErrorInfo.startTime;
				}
			}
			
			/*timeoffset over 0.5s*/
			if (timeOffset > 500000)
			{
				stThreeLevelParam[chanID].errorInfo.unreferencedPidErrorInfo.totalError++;
				TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_UNREFERENCEPID,tsHeadInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
			}
			stThreeLevelParam[chanID].unreferPidInfo[i].startTime = tsHeadInfo->stErrorInfo.startTime;
			return GOSTSR_SUCCESS;
		}
		
		if(i == MAX_UNREFERPID_NUM)
		{
			/*get empty unreferenced pid position*/
			for (index = 0; index < MAX_UNREFERPID_NUM; index++)
			{
				if (!stThreeLevelParam[chanID].unreferPidInfo[index].flag)
				{
					break;
				}
			}

			if (index != MAX_UNREFERPID_NUM)
			{
				stThreeLevelParam[chanID].unreferPidInfo[index].flag = 0x01;
				stThreeLevelParam[chanID].unreferPidInfo[index].pid = tsHeadInfo->ts_pid ;
				stThreeLevelParam[chanID].unreferPidInfo[index].startTime = tsHeadInfo->stErrorInfo.startTime;
			}
		}	
	}

	return GOSTSR_FAILURE;
}

static GOSTSR_S32 TsCheck_SdtActualSectionsTimeError(TS_SECTION_INFO *sectionInfo)
{
	GOSTSR_U32 timeOffset = 0;
	GOSTSR_U8 chanID = sectionInfo->stErrorInfo.chanID;
	
	if ((GOSTSR_NULL == sectionInfo) ||(BAT_SDT_PID != sectionInfo->PID) || (GOSTSR_NULL == sectionInfo->sectionData) \
			 || (sectionInfo->stErrorInfo.chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	/*check table ID error*/
	if ((SDT_TABLE_ID_ACTUAL != sectionInfo->tableID) && (SDT_TABLE_ID_OTEHR != sectionInfo->tableID) && \
		(BAT_TABLE_ID != sectionInfo->tableID) && (SI_TABLE_ID != sectionInfo->tableID))
	{
		stThreeLevelParam[chanID].errorInfo.sdtErrorInfo.tableIDError++;
		stThreeLevelParam[chanID].errorInfo.sdtErrorInfo.totalError++; 
		TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SDTACTUAL,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
	}
	
	if (SDT_TABLE_ID_ACTUAL == sectionInfo->tableID)
	{
		/*check nit actual time error*/
		if (stThreeLevelParam[chanID].sdtActualPreTime != 0)
		{
			if(sectionInfo->stErrorInfo.bCarryFlag)
			{
				timeOffset = 0xffffffff+sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].sdtActualPreTime;
			}
			else
			{
				if(sectionInfo->stErrorInfo.startTime >=  stThreeLevelParam[chanID].sdtActualPreTime)
				{
					timeOffset = sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].sdtActualPreTime;
				}
				else
				{
					timeOffset = stThreeLevelParam[chanID].sdtActualPreTime - sectionInfo->stErrorInfo.startTime;
				}
			}
			
			/*timeoffset over 2s*/
			if (timeOffset >= 2000000)
			{
				stThreeLevelParam[chanID].errorInfo.sdtErrorInfo.actualOverTimeError++;
				stThreeLevelParam[chanID].errorInfo.sdtErrorInfo.totalError++;
				TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SDTACTUAL,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
			}
			/*timeoffset within 25ms*/
			else if (timeOffset <= 25000)
			{
				stThreeLevelParam[chanID].errorInfo.sdtErrorInfo.actualLimitTimeError++;
				stThreeLevelParam[chanID].errorInfo.sdtErrorInfo.totalError++;
				TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SDTACTUAL,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
			}
		}

		stThreeLevelParam[chanID].sdtActualPreTime = sectionInfo->stErrorInfo.startTime;
	}
	
	return GOSTSR_SUCCESS;
}

static GOSTSR_S32 TsCheck_SdtOtherSectionsTimeError(TS_SECTION_INFO *sectionInfo)
{
	GOSTSR_U32 timeOffset = 0;
	GOSTSR_U8  sectionNum = 0;
	GOSTSR_U8  lastSectionNum = 0;
	GOSTSR_U8 chanID = 0;
	
	if ((GOSTSR_NULL == sectionInfo) ||(BAT_SDT_PID != sectionInfo->PID) || (GOSTSR_NULL == sectionInfo->sectionData) \
		|| (sectionInfo->stErrorInfo.chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	chanID = sectionInfo->stErrorInfo.chanID;
	if (SDT_TABLE_ID_OTEHR == sectionInfo->tableID)
	{
		/*get current section number*/
		if (GOSTSR_SUCCESS == TsCheck_GetCurrentSectionInfo(sectionInfo, &sectionNum, &lastSectionNum))
		{
			/*check nit other time error*/
			if (stThreeLevelParam[chanID].sdtOtherPreTime[sectionNum] != 0)
			{
				if(sectionInfo->stErrorInfo.bCarryFlag)
				{
					timeOffset = 0xffffffff+sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].sdtOtherPreTime[sectionNum];
				}
				else
				{
					if(sectionInfo->stErrorInfo.startTime >=  stThreeLevelParam[chanID].sdtOtherPreTime[sectionNum])
					{
						timeOffset = sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].sdtOtherPreTime[sectionNum];
					}
					else
					{
						timeOffset = stThreeLevelParam[chanID].sdtOtherPreTime[sectionNum] - sectionInfo->stErrorInfo.startTime;
					}
				}
				
				/*timeoffset over 10s*/
				if (timeOffset > 10000000)
				{
					stThreeLevelParam[chanID].errorInfo.sdtErrorInfo.otherOverTimeError++;
					stThreeLevelParam[chanID].errorInfo.sdtErrorInfo.totalError++;
					TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_SDTOTHER,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
				}
			}

			stThreeLevelParam[chanID].sdtOtherPreTime[sectionNum] = sectionInfo->stErrorInfo.startTime;
		}
	}
	
	return GOSTSR_SUCCESS;
}

static GOSTSR_S32 TsCheck_EitActualSectionsTimeError(TS_SECTION_INFO *sectionInfo)
{
	GOSTSR_U32 timeOffset = 0;
	GOSTSR_U8  sectionNum = 0;
	GOSTSR_U8  lastSectionNum = 0;
	GOSTSR_U8 chanID = 0;
	
	if ((GOSTSR_NULL == sectionInfo) ||(EIT_PID != sectionInfo->PID) || (GOSTSR_NULL == sectionInfo->sectionData) \
		|| (sectionInfo->stErrorInfo.chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	chanID = sectionInfo->stErrorInfo.chanID;
	/*check table ID error*/
	if ((sectionInfo->tableID < 0x4E) || ((sectionInfo->tableID > 0x6f) && (sectionInfo->tableID != 0x72)))
	{
		stThreeLevelParam[chanID].errorInfo.eitErrorInfo.tableIDError++;
		stThreeLevelParam[chanID].errorInfo.eitErrorInfo.totalError++;
		TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_EITACTUAL,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
	}
	
	if (EIT_TABLE_ID_ACTUAL == sectionInfo->tableID)
	{
		/*check eit actual time error*/
		if (stThreeLevelParam[chanID].eitActualPreTime != 0)
		{
			if(sectionInfo->stErrorInfo.bCarryFlag)
			{
				timeOffset = 0xffffffff+sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].eitActualPreTime;
			}
			else
			{
				if(sectionInfo->stErrorInfo.startTime >=  stThreeLevelParam[chanID].eitActualPreTime)
				{
					timeOffset = sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].eitActualPreTime;
				}
				else
				{
					timeOffset = stThreeLevelParam[chanID].eitActualPreTime - sectionInfo->stErrorInfo.startTime;
				}
			}

			/*timeoffset over 2s*/
			if (timeOffset > 2000000)
			{
				stThreeLevelParam[chanID].errorInfo.eitErrorInfo.actualOverTimeError++;
				stThreeLevelParam[chanID].errorInfo.eitErrorInfo.totalError++;
				TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_EITACTUAL,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
			}
			/*timeoffset within 25ms*/
			else if (timeOffset <= 25000)
			{
				stThreeLevelParam[chanID].errorInfo.eitErrorInfo.actualLimtTimeError++;
				stThreeLevelParam[chanID].errorInfo.eitErrorInfo.totalError++;
				TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_EITACTUAL,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
			}
		}
		stThreeLevelParam[chanID].eitActualPreTime = sectionInfo->stErrorInfo.startTime;

		if (GOSTSR_SUCCESS == TsCheck_GetCurrentSectionInfo(sectionInfo, &sectionNum, &lastSectionNum))
		{
			/*section 0 time error*/
			if (sectionNum == 0)
			{
				if (stThreeLevelParam[chanID].eitActualSec0PreTime != 0)
				{
					if(sectionInfo->stErrorInfo.bCarryFlag)
					{
						timeOffset = 0xffffffff+sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].eitActualSec0PreTime;
					}
					else
					{
						if(sectionInfo->stErrorInfo.startTime >=  stThreeLevelParam[chanID].eitActualSec0PreTime)
						{
							timeOffset = sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].eitActualSec0PreTime;
						}
						else
						{
							timeOffset = stThreeLevelParam[chanID].eitActualSec0PreTime - sectionInfo->stErrorInfo.startTime;
						}
					}

					/*timeoffset over 2s*/
					if (timeOffset > 2000000)
					{
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.actualSec0OverTimeError++;
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.totalError++;
						TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_EITACTUAL,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
					}
				}

				/*check eit actual pair error*/
				if (stThreeLevelParam[chanID].eitActualPairFlag != 3)
				{
					stThreeLevelParam[chanID].eitActualPairFlag++;
					if (stThreeLevelParam[chanID].eitActualPairFlag == 2)
					{
						stThreeLevelParam[chanID].eitActualPairFlag = 1;
						if (stThreeLevelParam[chanID].errorInfo.eitErrorInfo.actualPFPairError == 0)
						{
							stThreeLevelParam[chanID].errorInfo.eitErrorInfo.actualPFPairError = 1;
							stThreeLevelParam[chanID].errorInfo.eitErrorInfo.totalError++;
							TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_EITPF,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
						}
					}
				}
				else
				{
					if (stThreeLevelParam[chanID].errorInfo.eitErrorInfo.actualPFPairError == 1)
					{
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.actualPFPairError = 0;
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.totalError--;
						TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_EITPF,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
					}
				}
				
				stThreeLevelParam[chanID].eitActualSec0PreTime = sectionInfo->stErrorInfo.startTime;
			}
			/*section 1 time error*/
			else if (sectionNum == 1)
			{
				if (stThreeLevelParam[chanID].eitActualSec1PreTime != 0)
				{
					if(sectionInfo->stErrorInfo.bCarryFlag)
					{
						timeOffset = 0xffffffff+sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].eitActualSec1PreTime;
					}
					else
					{
						if(sectionInfo->stErrorInfo.startTime >=  stThreeLevelParam[chanID].eitActualSec1PreTime)
						{
							timeOffset = sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].eitActualSec1PreTime;
						}
						else
						{
							timeOffset = stThreeLevelParam[chanID].eitActualSec1PreTime - sectionInfo->stErrorInfo.startTime;
						}
					}

					/*timeoffset over 2s*/
					if (timeOffset >= 2000000)
					{
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.actualSec1OverTimeError++;
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.totalError++;
						TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_NITACTUAL,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
					}
				}

				/*check eit actual pair error*/
				if (stThreeLevelParam[chanID].eitActualPairFlag != 3)
				{
					stThreeLevelParam[chanID].eitActualPairFlag += 2;
					if (stThreeLevelParam[chanID].eitActualPairFlag == 4)
					{
						stThreeLevelParam[chanID].eitActualPairFlag = 1;
						if (stThreeLevelParam[chanID].errorInfo.eitErrorInfo.actualPFPairError == 0)
						{
							stThreeLevelParam[chanID].errorInfo.eitErrorInfo.actualPFPairError = 1;
							stThreeLevelParam[chanID].errorInfo.eitErrorInfo.totalError++;
							TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_EITACTUAL,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
						}
					}
				}
				else
				{
					if (stThreeLevelParam[chanID].errorInfo.eitErrorInfo.actualPFPairError == 1)
					{
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.actualPFPairError = 0;
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.totalError--;
						TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_NITACTUAL,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
					}
				}
				
				stThreeLevelParam[chanID].eitActualSec1PreTime = sectionInfo->stErrorInfo.startTime;
			}
		}
	}
	
	return GOSTSR_SUCCESS;
}

static GOSTSR_S32 TsCheck_EitOtherSectionsTimeError(TS_SECTION_INFO *sectionInfo)
{
	GOSTSR_U32 timeOffset = 0;
	GOSTSR_U8  sectionNum = 0;
	GOSTSR_U8  lastSectionNum = 0;
	GOSTSR_U8 chanID = 0;
	
	if ((GOSTSR_NULL == sectionInfo) ||(EIT_PID != sectionInfo->PID) || (GOSTSR_NULL == sectionInfo->sectionData) \
		|| (sectionInfo->stErrorInfo.chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	chanID = sectionInfo->stErrorInfo.chanID;
	if (EIT_TABLE_ID_OTHER == sectionInfo->tableID)
	{
		if (GOSTSR_SUCCESS == TsCheck_GetCurrentSectionInfo(sectionInfo, &sectionNum, &lastSectionNum))
		{
			/*section 0 time error*/
			if (sectionNum == 0)
			{
				if (stThreeLevelParam[chanID].eitOtherSec0PreTime != 0)
				{
					if(sectionInfo->stErrorInfo.bCarryFlag)
					{
						timeOffset = 0xffffffff+sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].eitOtherSec0PreTime;
					}
					else
					{
						if(sectionInfo->stErrorInfo.startTime >=  stThreeLevelParam[chanID].eitOtherSec0PreTime)
						{
							timeOffset = sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].eitOtherSec0PreTime;
						}
						else
						{
							timeOffset = stThreeLevelParam[chanID].eitOtherSec0PreTime - sectionInfo->stErrorInfo.startTime;
						}
					}
					
					/*timeoffset over 10s*/
					if (timeOffset >= 10000000)
					{
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.otherSec0OverTimeError++;
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.totalError++;
						TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_EITOTHER,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
					}
				}

				/*check eit other pf pair error*/
				if (stThreeLevelParam[chanID].eitOtherPairFlag != 3)
				{
					stThreeLevelParam[chanID].eitOtherPairFlag++;
					if (stThreeLevelParam[chanID].eitOtherPairFlag == 2)
					{
						stThreeLevelParam[chanID].eitOtherPairFlag = 1;
						if (stThreeLevelParam[chanID].errorInfo.eitErrorInfo.otherPFPairError == 0)
						{
							stThreeLevelParam[chanID].errorInfo.eitErrorInfo.otherPFPairError = 1;
							stThreeLevelParam[chanID].errorInfo.eitErrorInfo.totalError++;
							TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_EITOTHER,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
						}
					}
				}
				else
				{
					if (stThreeLevelParam[chanID].errorInfo.eitErrorInfo.otherPFPairError == 1)
					{
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.otherPFPairError = 0;
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.totalError--;
						TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_EITOTHER,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
					}
				}
				
				stThreeLevelParam[chanID].eitOtherSec0PreTime = sectionInfo->stErrorInfo.startTime;
			}
			/*section 1 time error*/
			else if (sectionNum == 1)
			{
				if (stThreeLevelParam[chanID].eitOtherSec1PreTime != 0)
				{
					if(sectionInfo->stErrorInfo.bCarryFlag)
					{
						timeOffset = 0xffffffff+sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].eitOtherSec1PreTime;
					}
					else
					{
						if(sectionInfo->stErrorInfo.startTime >=  stThreeLevelParam[chanID].eitOtherSec1PreTime)
						{
							timeOffset = sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].eitOtherSec1PreTime;
						}
						else
						{
							timeOffset = stThreeLevelParam[chanID].eitOtherSec1PreTime - sectionInfo->stErrorInfo.startTime;
						}
					}
					
					/*timeoffset over 10s*/
					if (timeOffset >= 10000000)
					{
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.otherSec1OverTimeError++;
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.totalError++;
						TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_EITOTHER,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
					}
				}

				/*check eit other pf pair error*/
				if (stThreeLevelParam[chanID].eitOtherPairFlag != 3)
				{
					stThreeLevelParam[chanID].eitOtherPairFlag += 2;
					if (stThreeLevelParam[chanID].eitOtherPairFlag == 4)
					{
						stThreeLevelParam[chanID].eitOtherPairFlag = 1;
						if (stThreeLevelParam[chanID].errorInfo.eitErrorInfo.otherPFPairError == 0)
						{
							stThreeLevelParam[chanID].errorInfo.eitErrorInfo.otherPFPairError = 1;
							stThreeLevelParam[chanID].errorInfo.eitErrorInfo.totalError++;
							TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_EITOTHER,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
						}
					}
				}
				else
				{
					if (stThreeLevelParam[chanID].errorInfo.eitErrorInfo.otherPFPairError == 1)
					{
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.otherPFPairError = 0;
						stThreeLevelParam[chanID].errorInfo.eitErrorInfo.totalError--;
						TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_EITOTHER,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
					}
				}
				
				stThreeLevelParam[chanID].eitOtherSec1PreTime = sectionInfo->stErrorInfo.startTime;
			}
		}
	}
	
	return GOSTSR_SUCCESS;
}

static GOSTSR_S32 TsCheck_RstSectionsTimeError(TS_SECTION_INFO *sectionInfo)
{
	GOSTSR_U32 timeOffset = 0;
	GOSTSR_U8 chanID = 0;
	
	if ((GOSTSR_NULL == sectionInfo) ||(RST_PID != sectionInfo->PID) || (GOSTSR_NULL == sectionInfo->sectionData) \
		|| (sectionInfo->stErrorInfo.chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	chanID = sectionInfo->stErrorInfo.chanID;
	/*check table ID error*/
	if ((RST_TABLE_ID != sectionInfo->tableID) && (SI_TABLE_ID != sectionInfo->tableID))
	{
		stThreeLevelParam[chanID].errorInfo.rstErrorInfo.tableIDError++;
		stThreeLevelParam[chanID].errorInfo.rstErrorInfo.totalError++;
		TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_RST,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
	}
	if (RST_TABLE_ID == sectionInfo->tableID)
	{
		if (stThreeLevelParam[chanID].rstSectionPreTime != 0)
		{
			if(sectionInfo->stErrorInfo.bCarryFlag)
			{
				timeOffset = 0xffffffff+sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].rstSectionPreTime;
			}
			else
			{
				if(sectionInfo->stErrorInfo.startTime >=  stThreeLevelParam[chanID].rstSectionPreTime)
				{
					timeOffset = sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].rstSectionPreTime;
				}
				else
				{
					timeOffset = stThreeLevelParam[chanID].rstSectionPreTime - sectionInfo->stErrorInfo.startTime;
				}
			}

			/*timeoffset within 25ms*/
			if (timeOffset <= 25000)
			{
				stThreeLevelParam[chanID].errorInfo.rstErrorInfo.limtTimeError++;
				stThreeLevelParam[chanID].errorInfo.rstErrorInfo.totalError++;
				TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_RST,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
			}
		}

		stThreeLevelParam[chanID].rstSectionPreTime = sectionInfo->stErrorInfo.startTime;
	}
	
	return GOSTSR_SUCCESS;
}

static GOSTSR_S32 TsCheck_TdtSectionsTimeError(TS_SECTION_INFO *sectionInfo)
{
	GOSTSR_U32 timeOffset = 0;
	GOSTSR_U8 chanID = 0;
	
	if ((GOSTSR_NULL == sectionInfo) ||(TDT_TOT_PID != sectionInfo->PID) || (GOSTSR_NULL == sectionInfo->sectionData) \
		|| (sectionInfo->stErrorInfo.chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	chanID = sectionInfo->stErrorInfo.chanID;
	/*check table ID error*/
	if ((TDT_TABLE_ID != sectionInfo->tableID) && (SI_TABLE_ID != sectionInfo->tableID) && (TOT_TABLE_ID != sectionInfo->tableID))
	{
		stThreeLevelParam[chanID].errorInfo.tdtErrorInfo.tableIDError++;
		stThreeLevelParam[chanID].errorInfo.tdtErrorInfo.totalError++;
		TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_TDT,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
	}
	if (TDT_TABLE_ID == sectionInfo->tableID)
	{
		/*check eit actual time error*/
		if (stThreeLevelParam[chanID].tdtSectionPreTime != 0)
		{
			if(sectionInfo->stErrorInfo.bCarryFlag)
			{
				timeOffset = 0xffffffff+sectionInfo->stErrorInfo.startTime - stThreeLevelParam[chanID].tdtSectionPreTime;
			}
			else
			{
				if(sectionInfo->stErrorInfo.startTime >=  stThreeLevelParam[chanID].tdtSectionPreTime)
				{
					timeOffset = sectionInfo->stErrorInfo.startTime -stThreeLevelParam[chanID].tdtSectionPreTime;
				}
				else
				{
					timeOffset = stThreeLevelParam[chanID].tdtSectionPreTime - sectionInfo->stErrorInfo.startTime;
				}
			}

			/*timeoffset over 30s*/
			if (timeOffset >= 30000000)
			{
				stThreeLevelParam[chanID].errorInfo.tdtErrorInfo.overTimeError++;
				stThreeLevelParam[chanID].errorInfo.tdtErrorInfo.totalError++;
				TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_TDT,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
			}
			/*timeoffset within 25ms*/
			else if (timeOffset <= 25000)
			{
				stThreeLevelParam[chanID].errorInfo.tdtErrorInfo.limitTimeError++;
				stThreeLevelParam[chanID].errorInfo.tdtErrorInfo.totalError++;
				TS_ERROR_LOG(chanID,TYPE_ERROR_THREE,TR101290_THREE_ERROR_TDT,sectionInfo->stErrorInfo.bytePos,GOSTSR_NULL,GOSTSR_NULL);
			}
		}

		stThreeLevelParam[chanID].tdtSectionPreTime = sectionInfo->stErrorInfo.startTime;
	}

	return GOSTSR_SUCCESS;
}

static GOSTSR_S32 TsCheck_EmptyBufferError(GOSTSR_U8 *data, GOSTSR_U16 dataLen, GOSTSR_U32 time)
{
	if ((GOSTSR_NULL == data) || (dataLen == 0))
	{
		return GOSTSR_FAILURE;
	}

	/*TODO*/
	
	return GOSTSR_SUCCESS;
}

static GOSTSR_S32 TsCheck_DataDelayError(GOSTSR_U8 *data, GOSTSR_U16 dataLen, GOSTSR_U32 time)
{
	if ((GOSTSR_NULL == data) || (dataLen == 0))
	{
		return GOSTSR_FAILURE;
	}

	/*TODO*/
	
	return GOSTSR_SUCCESS;
}
GOSTSR_S32 TsErrorCheck_ThreeLevel_setPid(GOSTSR_U8 chanID,SEARCH_INFO_S *pstProgInfo)
{
	if ((GOSTSR_NULL == pstProgInfo) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	memset(&stThreeLevelParam[chanID].stSearchInfo, 0x00, sizeof(SEARCH_INFO_S));
	memcpy(&stThreeLevelParam[chanID].stSearchInfo,pstProgInfo,sizeof(SEARCH_INFO_S));
	
	return GOSTSR_SUCCESS;
}
GOSTSR_S32 TsErrorCheck_ThreeLevel_UnreferencedPidError(TS_HEAD_INFO *tsHeadInfo)
{
	if((tsHeadInfo == GOSTSR_NULL) || (!tsHeadInfo->stErrorInfo.bisMatched))
	{
		return GOSTSR_FAILURE;
	}
	return TsCheck_UnreferencedPidError(tsHeadInfo);
}
GOSTSR_S32 TsErrorCheck_ThreeLevel_SectionsTimeError(TS_SECTION_INFO *sectionInfo)
{	
	if((sectionInfo == GOSTSR_NULL) || (!sectionInfo->stErrorInfo.bisMatched))
	{
		return GOSTSR_FAILURE;
	}
	
	TsCheck_NitSectionsTimeError(sectionInfo);
	TsCheck_SiSectionRepetitionRateError(sectionInfo);
	TsCheck_SdtActualSectionsTimeError(sectionInfo);
	TsCheck_SdtOtherSectionsTimeError(sectionInfo);
	TsCheck_EitActualSectionsTimeError(sectionInfo);
	TsCheck_EitOtherSectionsTimeError(sectionInfo);
	TsCheck_RstSectionsTimeError(sectionInfo);
	TsCheck_TdtSectionsTimeError(sectionInfo);

	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_ThreeLevel_BufferError(GOSTSR_U8 *data, GOSTSR_U16 dataLen, GOSTSR_U32 time)
{
	//[未使用
	TsCheck_BufferError(data, dataLen, time);
	TsCheck_EmptyBufferError(data, dataLen, time);
	TsCheck_DataDelayError(data, dataLen, time);
	
	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_ThreeLevel_GetErrorInfo(GOSTSR_U8 chanID,TsThreeLevelErrorCheck_Info *info)
{
	if ((GOSTSR_NULL == info) || (chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}

	memset(info, 0x00, sizeof(TsThreeLevelErrorCheck_Info));
	memcpy(info, &stThreeLevelParam[chanID].errorInfo, sizeof(TsThreeLevelErrorCheck_Info));

	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_ThreeLevel_Init()
{	
	memset(&stThreeLevelParam, 0x00, sizeof(TSERROR_THREELEVEL_PARAM_S) * TSERROR_CHANNELID_MAX);	
	
	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_ThreeLevel_DeInit()
{	
	memset(&stThreeLevelParam, 0x00, sizeof(TSERROR_THREELEVEL_PARAM_S) * TSERROR_CHANNELID_MAX);	

	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_ThreeLevel_ReInit(GOSTSR_U8 chanID)
{
	memset(&stThreeLevelParam[chanID], 0x00, sizeof(TSERROR_THREELEVEL_PARAM_S));	

	return GOSTSR_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /*  __cplusplus  */
