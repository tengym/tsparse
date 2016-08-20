#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef _TSERRORCHECK_ONELEVEL_H
#define _TSERRORCHECK_ONELEVEL_H

#include "TsErrorCheck_Common.h"
#include "GosTsr_AnalysisData.h"

#define ONE_ERROR_PMT_MAX	(PCRPID_NUM_MAX)
#define ONE_ERROR_PES_MAX	(PESPID_NUM_MAX)
#define PID_NUMBER_MAX		(32 * 8 * 2)			//码流中PID 的最大数(仅统计音视频)

#define CONTINUITYCOUNTER_MAX	15
#define ONELEVEL_BYTEPOS_MAX 0xffffffff

typedef struct
{
	GOSTSR_U32 timeout_error; //两个pat包之间的时间超过0.5秒
	GOSTSR_U32 tableid_error; //tabelId is not 0x000
	GOSTSR_U32 scramble_error; //pat 加扰
	GOSTSR_U32 total_error;
}ONELEVEL_PATERROR;

typedef struct
{
	GOSTSR_U32 timeout_error; //两个pmt包之间的时间超过0.5秒
	GOSTSR_U32 scramble_error; //pmt 加扰
	GOSTSR_U32 total_error;
}ONELEVEL_PMTERROR;


typedef struct
{
	GOSTSR_BOOL bUsed;
	GOSTSR_U16 pid;
	GOSTSR_U8 continutycount;
	GOSTSR_BOOL predisContinuityIndicator;
}CONTINUTY_COUNT_INFO;

typedef struct
{
	GOSTSR_U32	bytePos;
	GOSTSR_U16	pid;
	GOSTSR_BOOL	recvFlag;
}PID_TIMEOUT_INFO;

typedef struct
{
	GOSTSR_U16			NbPmtInfo;
	PID_TIMEOUT_INFO	pmtInfo[ONE_ERROR_PMT_MAX];
}PMTPID_TIMEOUT_INFO;

typedef struct
{
	GOSTSR_U16			NbPesInfo;
	PID_TIMEOUT_INFO	pesInfo[ONE_ERROR_PES_MAX];
}PESPID_TIMEOUT_INFO;

typedef struct
{
	GOSTSR_U32 tsSyncLossError;		//同步丢失错
	GOSTSR_U32 tsSyncByteError;	//同步字节错
	GOSTSR_U32 tsContinuityCounterError;//连续计数错

	ONELEVEL_PATERROR tsPatError;		// PAT错
	ONELEVEL_PMTERROR tsPmtError;		//PMT 错
	GOSTSR_U32 tsPIDMissError;			//PID丢失错
}TSERROR_ONELEVEL_RECORD;//一级错误的记录

/*一级错误的全局变量结构体*/
typedef struct
{
	GOSTSR_U32 u32BytePos_PatBak;
	
	CONTINUTY_COUNT_INFO 	stContinutyCount[PID_NUMBER_MAX];
	PMTPID_TIMEOUT_INFO 		pmtPid_timeout_info;
	PESPID_TIMEOUT_INFO 		pesPid_timeout_info;
	TSERROR_ONELEVEL_RECORD 	oneLevelError_record;  
}TSERROR_ONELEVEL_PARAM_S;

extern GOSTSR_S32 TsErrorCheck_OneLevel_Init();
extern GOSTSR_S32 TsErrorCheck_OneLevel_DeInit();
extern GOSTSR_S32 TsErrorCheck_OneLevel_ReInit(GOSTSR_U8 chanID);
extern GOSTSR_S32 TsErrorCheck_OneLevel_GetErrorInfo(GOSTSR_U8 chanID,TSERROR_ONELEVEL_RECORD *oneLevelErrorInfo);

extern GOSTSR_S32 TsErrorCheck_OneLevel_SyncLossError(GOSTSR_U8 chanID,GOSTSR_BOOL bisMatched,GOSTSR_U32 tmpSyncOffset);
extern GOSTSR_S32 TsErrorCheck_OneLevel_SyncByteError(GOSTSR_U8 chanID,GOSTSR_BOOL bisMatched,GOSTSR_U32 tmpSyncOffset);
extern GOSTSR_S32 TsErrorCheck_OneLevel_ContinuityCounterError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U32 pid,GOSTSR_U8 Continuity,GOSTSR_U8  adapter_control,GOSTSR_U8 disContinuityIndicator);
extern GOSTSR_S32 TsErrorCheck_OneLevel_PatError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U32 pid,GOSTSR_U32 tableId,GOSTSR_U8  scramble_control);
extern GOSTSR_S32 TsErrorCheck_OneLevel_PmtError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U16 pid,GOSTSR_U8  scramble_control);
extern GOSTSR_S32 TsErrorCheck_OneLevel_pidMissError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U16 pid);
extern GOSTSR_S32 TsErrorCheck_OneLevel_pidMissNotReach(GOSTSR_U8 chanID);

extern GOSTSR_S32 TsErrorCheck_OneLevel_setPid(GOSTSR_U8 chanID,SEARCH_INFO_S *stSearchInfo);


#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /*  __cplusplus  */
