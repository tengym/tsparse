#include "TsErrorCheck_Log.h"

#include "TsErrorCheck_OneLevel.h"
#include "TsErrorCheck_TwoLevel.h"
#include "TsErrorCheck_ThreeLevel.h"


#define CHANID0_LOGERROR_PATH "TR101290_ErrorLog_ChanID0.txt"
#define CHANID1_LOGERROR_PATH "TR101290_ErrorLog_ChanID1.txt"


static GOSTSR_U32	u32ErrorLevel[TSERROR_CHANNELID_MAX];
static GOSTSR_U32	u32ErrorType[TSERROR_CHANNELID_MAX];
//static GOSTSR_U32 	u32ErrorNum[TSERROR_CHANNELID_MAX] = {1};

static GOSTSR_U32	u32LogNbProg[TSERROR_CHANNELID_MAX] = {0};

/*嶷仟兜兵晒*/
GOSTSR_S32 TsErrorCheck_Log_ReInit(GOSTSR_U8 chanID)
{
	/*5:Pmt_Pid Error、4:Pmt Error、3:Pat Error、2:ContinuCount Error、1:SyncByte Error、0:SyncLoss Error*/
	GOSTSR_U32 oneError_Level = (1 << 5 |1<< 4 | 1 << 3 | 1 << 2 | 1 << 1 | 1 << 0);
	
	/*5:Cat Error、4:Pts Error、3:Pcr_Accuracy Error、2:Pcr_Discont Error、1:Crc Error、0:Transport Error*/
	GOSTSR_U32 twoError_Level = (1 << 5 |1<< 4 | 0 << 3 | 1 << 2 | 1 << 1 | 1 << 0) << 6;

	/*10:Tdt Error、9:Rst Error、8:EitPF Error、7:EitOther Error、6:EitActual Error、5:SdtOther Error*/
	/*4:SdtActual Error、3:UnreferencePid Error、2:SiRepetition Error、1:NitOther Error、0:NitActual Error*/
	GOSTSR_U32 threeError_Level = (1 << 11 |1<< 10 | 1 << 9 | 1 << 8 | 1 << 7 | 1 << 6 |1 << 5 |1<< 4 | 1 << 3 | 1 << 2 | 1 << 1 | 1 << 0) << 12;
	
	u32ErrorType[chanID] = (0x01 | 0x02 | 0x04);
	u32ErrorLevel[chanID] = (oneError_Level | twoError_Level | threeError_Level);
#if 0
	if(chanID == 0)
		remove(CHANID0_LOGERROR_PATH);
	else
		remove(CHANID1_LOGERROR_PATH);
#endif
	u32LogNbProg[chanID] = 0;

	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_Log_Init()
{
	/*5:Pmt_Pid Error、4:Pmt Error、3:Pat Error、2:ContinuCount Error、1:SyncByte Error、0:SyncLoss Error*/
	GOSTSR_U32 oneError_Level = (1 << 5 |1<< 4 | 1 << 3 | 1 << 2 | 1 << 1 | 1 << 0);
	
	/*5:Cat Error、4:Pts Error、3:Pcr_Accuracy Error、2:Pcr_Discont Error、1:Crc Error、0:Transport Error*/
	GOSTSR_U32 twoError_Level = (1 << 5 |1<< 4 | 1 << 3 | 1 << 2 | 1 << 1 | 1 << 0) << 6;

	/*10:Tdt Error、9:Rst Error、8:EitPF Error、7:EitOther Error、6:EitActual Error、5:SdtOther Error*/
	/*4:SdtActual Error、3:UnreferencePid Error、2:SiRepetition Error、1:NitOther Error、0:NitActual Error*/
	GOSTSR_U32 threeError_Level = (1 << 11 |1<< 10 | 1 << 9 | 1 << 8 | 1 << 7 | 1 << 6 |1 << 5 |1<< 4 | 1 << 3 | 1 << 2 | 1 << 1 | 1 << 0) << 12;
	
	u32ErrorType[0] = (0x01 | 0x02 | 0x04);
	u32ErrorLevel[0] = (oneError_Level | twoError_Level | threeError_Level);
	u32ErrorType[1] = (0x01 | 0x02 | 0x04);
	u32ErrorLevel[1] = (oneError_Level | twoError_Level | threeError_Level);
	remove(CHANID0_LOGERROR_PATH);
	remove(CHANID1_LOGERROR_PATH);

	memset(u32LogNbProg,0,sizeof(GOSTSR_U32) *TSERROR_CHANNELID_MAX);
	
	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_Log_DeInit()
{
	/*5:Pmt_Pid Error、4:Pmt Error、3:Pat Error、2:ContinuCount Error、1:SyncByte Error、0:SyncLoss Error*/
	GOSTSR_U32 oneError_Level = (1 << 5 |1<< 4 | 1 << 3 | 1 << 2 | 1 << 1 | 1 << 0);
	
	/*5:Cat Error、4:Pts Error、3:Pcr_Accuracy Error、2:Pcr_Discont Error、1:Crc Error、0:Transport Error*/
	GOSTSR_U32 twoError_Level = (1 << 5 |1<< 4 | 0 << 3 | 1 << 2 | 1 << 1 | 1 << 0) << 6;

	/*10:Tdt Error、9:Rst Error、8:EitPF Error、7:EitOther Error、6:EitActual Error、5:SdtOther Error*/
	/*4:SdtActual Error、3:UnreferencePid Error、2:SiRepetition Error、1:NitOther Error、0:NitActual Error*/
	GOSTSR_U32 threeError_Level = (1 << 11 |1<< 10 | 1 << 9 | 1 << 8 | 1 << 7 | 1 << 6 |1 << 5 |1<< 4 | 1 << 3 | 1 << 2 | 1 << 1 | 1 << 0) << 12;
	
	u32ErrorType[0] = (0x01 | 0x02 | 0x04);
	u32ErrorLevel[0] = (oneError_Level | twoError_Level | threeError_Level);
	u32ErrorType[1] = (0x01 | 0x02 | 0x04);
	u32ErrorLevel[1] = (oneError_Level | twoError_Level | threeError_Level);
	remove(CHANID0_LOGERROR_PATH);
	remove(CHANID1_LOGERROR_PATH);
	memset(u32LogNbProg,0,sizeof(GOSTSR_U32) *TSERROR_CHANNELID_MAX);
	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_Log_SetNbProg(GOSTSR_U8 chanID,GOSTSR_U32 NbProg)
{
	if((chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	u32LogNbProg[chanID] = NbProg;

	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_Log_SetErrorType(GOSTSR_U8 chanID,GOSTSR_U32 errorType)
{
	if((chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	u32ErrorType[chanID] = errorType;

	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_Log_SetErrorLevel(GOSTSR_U8 chanID,GOSTSR_U32 errorLevel)
{
	if((chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	u32ErrorLevel[chanID] = errorLevel;
	
	return GOSTSR_SUCCESS;
}

static GOSTSR_S32 TsErrorCheck_Log_Debug(GOSTSR_U8 chanID,GOSTSR_U32 errorType, GOSTSR_U32 errorLevel,GOSTSR_U32 bytePos,GOSTSR_U8 *pPrivate,const GOSTSR_U8 *pFormat)
{
	char strError[256] = {0};
	//char strErrorLog[1024] = {0};
	
	if((chanID >= TSERROR_CHANNELID_MAX) || (!(errorType & u32ErrorType[chanID])) || (!(errorLevel & u32ErrorLevel[chanID])))
	{
		return GOSTSR_FAILURE;
	}

	switch(errorLevel & u32ErrorLevel[chanID])
	{
		case TR101290_ONE_ERROR_SYNCLOSS:
			{
				strcpy(strError, "[1] SyncLoss_Error");
			}
			break;
		case TR101290_ONE_ERROR_SYNCBYTE:
			{
				strcpy(strError, "[1] SyncByte_Error");
			}
			break;
		case TR101290_ONE_ERROR_PAT:
			{
				strcpy(strError, "[1] Pat_Error");
			}
			break;
		case TR101290_ONE_ERROR_PMT:
			{
				strcpy(strError, "[1] Pmt_Error");
			}
			break;
		case TR101290_ONE_ERROR_CONTINUTYCOUNT:
			{
				strcpy(strError, "[1] ContinutyCount_Error");
			}
			break;
		case TR101290_ONE_ERROR_PID:
			{
				strcpy(strError, "[1] PmtPid_Error");
			}
			break;
		case TR101290_TWO_ERROR_TRANSPORT:
			{
				strcpy(strError, "[2] Transport_Error");
			}
			break;
		case TR101290_TWO_ERROR_CRC:
			{
				strcpy(strError, "[2] Crc_Error");
			}
			break;
		case TR101290_TWO_ERROR_PCRDISCONT:
			{
				strcpy(strError, "[2] PcrDiscont_Error");
			}
			break;
		case TR101290_TWO_ERROR_ACCURACY:
			{
				GOSTSR_U32 *u32PcrValueDiff_Ns = (GOSTSR_U32 *)pPrivate;
				char strErrorTemp[256] = {0};
				strcpy(strErrorTemp, "PcrAccuracy_Error");
				sprintf(strError,"%s\t time:%d",strErrorTemp,*u32PcrValueDiff_Ns);
			}
			break;
		case TR101290_TWO_ERROR_PTS:
			{
				strcpy(strError, "[2] Pts_Error");
			}
			break;
		case TR101290_TWO_ERROR_CAT:
			{
				strcpy(strError, "[2] Cat_Error");
			}
			break;
		case TR101290_THREE_ERROR_NITACTUAL:
			{
				strcpy(strError, "[3] NitActual_Error");
			}
			break;
		case TR101290_THREE_ERROR_NITOTHER:
			{
				strcpy(strError, "[3] NitOther_Error");
			}
			break;
		case TR101290_THREE_ERROR_SIREPETITION:
			{
				strcpy(strError, "[3] SiRepetition_Error");
			}
			break;
		case TR101290_THREE_ERROR_UNREFERENCEPID:
			{
				strcpy(strError, "[3] UndeferencePid_Error");
			}
			break;
		case TR101290_THREE_ERROR_SDTACTUAL:
			{
				strcpy(strError, "[3] NitActual_Error");
			}
			break;
		case TR101290_THREE_ERROR_SDTOTHER:
			{
				strcpy(strError, "[3] SdtOther_Error");
			}
			break;
		case TR101290_THREE_ERROR_EITACTUAL:
			{
				strcpy(strError, "[3] EitActual_Error");
			}
			break;
		case TR101290_THREE_ERROR_EITOTHER:
			{
				strcpy(strError, "[3] EitOther_Error");
			}
			break;
		case TR101290_THREE_ERROR_EITPF:
			{
				strcpy(strError, "[3] EitPF_Error");
			}
			break;
		case TR101290_THREE_ERROR_RST:
			{
				strcpy(strError, "[3] Rst_Error");
			}
			break;
		case TR101290_THREE_ERROR_TDT:
			{
				strcpy(strError, "[3] Tdt_Error");
			}
			break;
		default:
			strcpy(strError, "NULL");
			break;
	}
	if(strcmp(strError,"NULL") != 0)
	{
#if TSERROR_LOG_DEBUG
		PRINT_DEBUG(strError,bytePos);
#endif
#if TSERROR_LOG_EXPORT
		if(chanID == 0)
		{
			sprintf(strErrorLog, "[%04d] %-30s\tBytePos = %-20d\tPkt = %d \n",u32ErrorNum[0],strError,bytePos,bytePos/188);
			TsErrorCheck_Log_ExportLog(CHANID0_LOGERROR_PATH, strErrorLog, strlen(strErrorLog));
			u32ErrorNum[0]++;
		}
		else if(chanID == 1)
		{
			sprintf(strErrorLog, "[%04d] %-30s\tBytePos = %-20d\tPkt = %d \n",u32ErrorNum[1],strError,bytePos,bytePos/188);
			TsErrorCheck_Log_ExportLog(CHANID1_LOGERROR_PATH, strErrorLog, strlen(strErrorLog));
			u32ErrorNum[1]++;
		}
		
#endif
	}	
	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TS_ERROR_LOG(GOSTSR_U8 chanID,GOSTSR_U32 errorType, GOSTSR_U32 errorLevel,GOSTSR_U32 bytePos,void *pPrivate,const GOSTSR_U8 *pFormat)
{
	return TsErrorCheck_Log_Debug(chanID,errorType,errorLevel,bytePos,pPrivate,pFormat);
}

GOSTSR_S32 TS_DEBUG_LOG(char *str)
{
	return ERROR_DEBUG(str);
}

GOSTSR_S32 TsErrorCheck_Log_ExportLog(char *pPathName,GOSTSR_U8 *pData, GOSTSR_U32 u32DataLen)
{
	FILE *fp_w = GOSTSR_NULL;
	GOSTSR_S32 	retNum = 0;

	fp_w = fopen(pPathName, "ab+") ;
	if(fp_w== GOSTSR_NULL)
	{
		fp_w = fopen(pPathName, "wb+");
		if(fp_w == GOSTSR_NULL)
		{
			printf("\nfopen failed\n");
			return GOSTSR_FAILURE;		
		}
	}
	
	retNum = fwrite(pData, u32DataLen, 1, fp_w);
	if (1 != retNum)
	{
		printf("\nfwrite failed\n");
	    	return GOSTSR_FAILURE;
	}
	fclose(fp_w);
	
	char	u8Cmd[256] = {0};
	sprintf(u8Cmd, "chmod 777 %s",pPathName);
	system(u8Cmd);
	
	return GOSTSR_SUCCESS;
}


GOSTSR_S32 TsErrorCheck_Log_OneLevel_PrintInfo(GOSTSR_U8 chanID)
{
	TSERROR_ONELEVEL_RECORD stAppOneLevel;
	memset(&stAppOneLevel,0,sizeof(TSERROR_ONELEVEL_RECORD));
	
	//printf("\nChanID:%d---------->TsErrorCheck_Log\n",chanID);
	//printf("\n>>>>>>>>>>>-------ChanID:%d------<<<<<<<<<<<<<<<\n",chanID);
	printf("AvTransportRate = %f\n",(GOSTSR_F32)TsErrorCheck_TwoLevel_getAvTransportRate(chanID) / 1000000);
	printf("_________One Level Error_________oneSize:%d\n",sizeof(TSERROR_ONELEVEL_RECORD));
	if(TsErrorCheck_OneLevel_GetErrorInfo(chanID,&stAppOneLevel) == GOSTSR_SUCCESS)
	{
	#if 1
		printf("SyncLossError = %d\n",stAppOneLevel.tsSyncLossError);
		printf("SyncByteError = %d\n",stAppOneLevel.tsSyncByteError);
		printf("ContinuityCounterError = %d\n",stAppOneLevel.tsContinuityCounterError);

		printf("PatErrorError = %d\n",stAppOneLevel.tsPatError.total_error);
		//printf("stAppOneLevel.tsPatError.timeout_error = %d\n",stAppOneLevel.tsPatError.timeout_error);
		//printf("stAppOneLevel.tsPatError.tableid_error = %d\n",stAppOneLevel.tsPatError.tableid_error);
		//printf("stAppOneLevel.tsPatError.scramble_error = %d\n\n",stAppOneLevel.tsPatError.scramble_error);

		printf("PmtErrorError = %d\n",stAppOneLevel.tsPmtError.total_error);
		//printf("stAppOneLevel.tsPmtError.timeout_error = %d\n",stAppOneLevel.tsPmtError.timeout_error);
		//printf("stAppOneLevel.tsPmtError.scramble_error = %d\n",stAppOneLevel.tsPmtError.scramble_error);
		

		printf("PIDMissError = %d\n",stAppOneLevel.tsPIDMissError);
	#endif
	}
	//printf("_________________________________\n\n");

	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_Log_TwoLevel_PrintInfo(GOSTSR_U8 chanID)
{
	TSERROR_TWOLEVEL_S stAppTwoLevel;
	memset(&stAppTwoLevel,0,sizeof(TSERROR_TWOLEVEL_S));
	printf("\n_________Two Level Error_________twoSize:%d\n",sizeof(TSERROR_TWOLEVEL_S));
	if(TsErrorCheck_TwoLevel_getTwoLevelError(chanID,&stAppTwoLevel) == GOSTSR_SUCCESS)
	{
	#if 1
		printf("TransportError = %d\n",stAppTwoLevel.u32TStransmissionErrorCount);
		printf("CrcError = %d\n",
			stAppTwoLevel.stCrcErrorInfo.totalCrcErrorCount);
	#if 0
		printf("( PAT=%d__PMT=%d__CAT=%d__NIT=%d__BAT=%d__SDT=%d__EIT=%d )\n",
			stAppTwoLevel.stCrcErrorInfo.patCrcErrorCount,stAppTwoLevel.stCrcErrorInfo.pmCrctErrorCount,
			stAppTwoLevel.stCrcErrorInfo.catCrcErrorCount,stAppTwoLevel.stCrcErrorInfo.nitCrcErrorCount,
			stAppTwoLevel.stCrcErrorInfo.batCrcErrorCount,stAppTwoLevel.stCrcErrorInfo.sdtCrcErrorCount,
			stAppTwoLevel.stCrcErrorInfo.eitCrcErrorCount);
	#endif
		printf("PcrDisError = %d\n",stAppTwoLevel.u32PcrDisErrorCount);
		printf("PcrJitError = %d\n",stAppTwoLevel.u32PcrJitErrorCount);
		printf("PtsError = %d\n",stAppTwoLevel.u32PtsErrorCount);
		printf("CatError = %d\n",
			stAppTwoLevel.stCatErrorInfo.totalCatErrorCount);
		//printf("(TableID = %d____scarmble = %d )\n",stAppTwoLevel.stCatErrorInfo.tableIDErrorCount,stAppTwoLevel.stCatErrorInfo.scambleErrorCount);
	#endif

	}
	//printf("_________________________________\n\n");

	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_Log_ThreeLevel_PrintInfo(GOSTSR_U8 chanID)
{
	TsThreeLevelErrorCheck_Info stAppThreeLevel;
	memset(&stAppThreeLevel,0,sizeof(TsThreeLevelErrorCheck_Info));
	printf("\n________Three Level Error________threeSize:%d\n",sizeof(TsThreeLevelErrorCheck_Info));
	if(TsErrorCheck_ThreeLevel_GetErrorInfo(chanID,&stAppThreeLevel) == GOSTSR_SUCCESS)
	{
#if 1
		printf("nitError = %d \n",stAppThreeLevel.nitErrorInfo.totalError);
#if 0

		printf("stAppThreeLevel.nitErrorInfo.tableIDError  = %d \n",stAppThreeLevel.nitErrorInfo.tableIDError);
		printf("stAppThreeLevel.nitErrorInfo.actualOverTimeError = %d \n",stAppThreeLevel.nitErrorInfo.actualOverTimeError);
		printf("stAppThreeLevel.nitErrorInfo.actualLimitTimeError  = %d \n",stAppThreeLevel.nitErrorInfo.actualLimitTimeError);
		printf("stAppThreeLevel.nitErrorInfo.otherOverTimeError  = %d \n",stAppThreeLevel.nitErrorInfo.otherOverTimeError);
		printf("stAppThreeLevel.nitErrorInfo.otherLimitTimeError  = %d \n\n",stAppThreeLevel.nitErrorInfo.otherLimitTimeError);
#endif
		printf("siRepetitionRateError  = %d \n",stAppThreeLevel.siRepetitionRateErrorInfo.totalError);
#if 0
		printf("stAppThreeLevel.siRepetitionRateErrorInfo.siSectionLimitTimeError = %d \n",stAppThreeLevel.siRepetitionRateErrorInfo.siSectionLimitTimeError);
		printf("stAppThreeLevel.siRepetitionRateErrorInfo.nitOverTimeError = %d \n",stAppThreeLevel.siRepetitionRateErrorInfo.nitOverTimeError);
		printf("stAppThreeLevel.siRepetitionRateErrorInfo.batOverTimeError = %d \n",stAppThreeLevel.siRepetitionRateErrorInfo.batOverTimeError);
		printf("stAppThreeLevel.siRepetitionRateErrorInfo.sdtActualOverTimeError = %d \n",stAppThreeLevel.siRepetitionRateErrorInfo.sdtActualOverTimeError);
		printf("stAppThreeLevel.siRepetitionRateErrorInfo.sdtOtherOverTimeError = %d \n",stAppThreeLevel.siRepetitionRateErrorInfo.sdtOtherOverTimeError);
		printf("stAppThreeLevel.siRepetitionRateErrorInfo.eitActualOverTimeError = %d \n",stAppThreeLevel.siRepetitionRateErrorInfo.eitActualOverTimeError);
		printf("stAppThreeLevel.siRepetitionRateErrorInfo.eitOtherOverTimeError = %d \n",stAppThreeLevel.siRepetitionRateErrorInfo.eitOtherOverTimeError);
		printf("stAppThreeLevel.siRepetitionRateErrorInfo.eitActualScheduleOverTimeError = %d \n",stAppThreeLevel.siRepetitionRateErrorInfo.eitActualScheduleOverTimeError);
		printf("stAppThreeLevel.siRepetitionRateErrorInfo.eitOtherScheduleOverTimeError = %d \n",stAppThreeLevel.siRepetitionRateErrorInfo.eitOtherScheduleOverTimeError);
		printf("stAppThreeLevel.siRepetitionRateErrorInfo.tdtOverTimeError = %d \n",stAppThreeLevel.siRepetitionRateErrorInfo.tdtOverTimeError);
		printf("stAppThreeLevel.siRepetitionRateErrorInfo.totOverTimeError = %d \n",stAppThreeLevel.siRepetitionRateErrorInfo.totOverTimeError);
#endif
		printf("bufferError = %d \n",stAppThreeLevel.bufferErrorInfo.totalError);
#if 0
		printf("stAppThreeLevel.bufferErrorInfo.TBBufferError = %d \n",stAppThreeLevel.bufferErrorInfo.TBBufferError);
		printf("stAppThreeLevel.bufferErrorInfo.TBsysBufferError = %d \n",stAppThreeLevel.bufferErrorInfo.TBsysBufferError);
		printf("stAppThreeLevel.bufferErrorInfo.MBBufferError = %d \n",stAppThreeLevel.bufferErrorInfo.MBBufferError);
		printf("stAppThreeLevel.bufferErrorInfo.EBBufferError = %d \n",stAppThreeLevel.bufferErrorInfo.EBBufferError);
		printf("stAppThreeLevel.bufferErrorInfo.BBufferError = %d \n",stAppThreeLevel.bufferErrorInfo.BBufferError);
		printf("stAppThreeLevel.bufferErrorInfo.BsysBufferError = %d \n\n",stAppThreeLevel.bufferErrorInfo.BsysBufferError);
#endif
		printf("unreferencedPidError = %d \n",stAppThreeLevel.unreferencedPidErrorInfo.totalError);
		printf("sdtError = %d\n",stAppThreeLevel.sdtErrorInfo.totalError);
#if 0

		printf("stAppThreeLevel.sdtErrorInfo.tableIDError = %d\n",stAppThreeLevel.sdtErrorInfo.tableIDError);
		printf("stAppThreeLevel.sdtErrorInfo.actualOverTimeError = %d\n",stAppThreeLevel.sdtErrorInfo.actualOverTimeError);
		printf("stAppThreeLevel.sdtErrorInfo.actualLimitTimeError = %d\n",stAppThreeLevel.sdtErrorInfo.actualLimitTimeError);
		printf("stAppThreeLevel.sdtErrorInfo.otherOverTimeError = %d\n\n",stAppThreeLevel.sdtErrorInfo.otherOverTimeError);
#endif

		printf("eitError = %d\n",stAppThreeLevel.eitErrorInfo.totalError);
#if 0
		printf("stAppThreeLevel.eitErrorInfo.tableIDError = %d\n",stAppThreeLevel.eitErrorInfo.tableIDError);
		printf("stAppThreeLevel.eitErrorInfo.actualOverTimeError = %d\n",stAppThreeLevel.eitErrorInfo.actualOverTimeError);
		printf("stAppThreeLevel.eitErrorInfo.actualLimtTimeError = %d\n",stAppThreeLevel.eitErrorInfo.actualLimtTimeError);
		printf("stAppThreeLevel.eitErrorInfo.actualSec0OverTimeError = %d\n",stAppThreeLevel.eitErrorInfo.actualSec0OverTimeError);
		printf("stAppThreeLevel.eitErrorInfo.actualSec1OverTimeError = %d\n",stAppThreeLevel.eitErrorInfo.actualSec1OverTimeError);
		printf("stAppThreeLevel.eitErrorInfo.actualPFPairError = %d\n",stAppThreeLevel.eitErrorInfo.actualPFPairError);
		printf("stAppThreeLevel.eitErrorInfo.otherSec0OverTimeError = %d\n",stAppThreeLevel.eitErrorInfo.otherSec0OverTimeError);
		printf("stAppThreeLevel.eitErrorInfo.otherSec1OverTimeError = %d\n",stAppThreeLevel.eitErrorInfo.otherSec1OverTimeError);
		printf("stAppThreeLevel.eitErrorInfo.otherPFPairError = %d\n\n",stAppThreeLevel.eitErrorInfo.otherPFPairError);
#endif

		printf("rstError = %d\n",stAppThreeLevel.rstErrorInfo.totalError);
#if 0

		printf("stAppThreeLevel.rstErrorInfo.tableIDError = %d\n",stAppThreeLevel.rstErrorInfo.tableIDError);
		printf("stAppThreeLevel.rstErrorInfo.limtTimeError = %d\n\n",stAppThreeLevel.rstErrorInfo.limtTimeError);
#endif

		printf("tdtError = %d\n",stAppThreeLevel.tdtErrorInfo.totalError);
#if 0
		printf("stAppThreeLevel.tdtErrorInfo.overTimeError = %d\n",stAppThreeLevel.tdtErrorInfo.overTimeError);
		printf("stAppThreeLevel.tdtErrorInfo.tableIDError = %d\n",stAppThreeLevel.tdtErrorInfo.tableIDError);
		printf("stAppThreeLevel.tdtErrorInfo.limitTimeError = %d\n\n",stAppThreeLevel.tdtErrorInfo.limitTimeError);
#endif
		printf("emptyBufferError = %d\n",stAppThreeLevel.emptyBufferErrorInfo.totalError);
#if 0
		printf("stAppThreeLevel.emptyBufferErrorInfo.TBBufferEmptyError = %d\n",stAppThreeLevel.emptyBufferErrorInfo.TBBufferEmptyError);
		printf("stAppThreeLevel.emptyBufferErrorInfo.TBsysBufferEmptyError = %d\n",stAppThreeLevel.emptyBufferErrorInfo.TBsysBufferEmptyError);
		printf("stAppThreeLevel.emptyBufferErrorInfo.MBBufferEmptyError = %d\n\n",stAppThreeLevel.emptyBufferErrorInfo.MBBufferEmptyError);
#endif
		printf("DataDelayError = %d\n",stAppThreeLevel.dataDelayErrorInfo.totalError);
#if 0
		printf("stAppThreeLevel.dataDelayErrorInfo.stillPicOverTimeError = %d\n",stAppThreeLevel.dataDelayErrorInfo.stillPicOverTimeError);
		printf("stAppThreeLevel.dataDelayErrorInfo.generalDataOverTimeError = %d\n\n",stAppThreeLevel.dataDelayErrorInfo.generalDataOverTimeError);
#endif

#endif

	}
	printf("\n\n");
	//printf("_________________________________\n\n");

	return GOSTSR_SUCCESS;
}

GOSTSR_S32 TsErrorCheck_Log_PrintInfo(GOSTSR_U8 chanID)
{
	/*峪嗤朴沫欺准朕嘉氏嬉咫潤惚*/

	if((chanID >= TSERROR_CHANNELID_MAX))
	{
		return GOSTSR_FAILURE;
	}
	if((u32LogNbProg[chanID] > 0) && (TsErrorCheck_TwoLevel_getAvTransportRate(chanID) > 0))
	{
		printf("chanID = %d------Rate = %d\n",chanID,TsErrorCheck_TwoLevel_getAvTransportRate(chanID));
		TsErrorCheck_Log_OneLevel_PrintInfo(chanID);
		TsErrorCheck_Log_TwoLevel_PrintInfo(chanID);
		TsErrorCheck_Log_ThreeLevel_PrintInfo(chanID);
	}
	else
	{
		printf("Log: chanID = %d---------->No Prog Info(Rate = %d)\n",chanID,TsErrorCheck_TwoLevel_getAvTransportRate(chanID));
	}
	return GOSTSR_SUCCESS;
}
