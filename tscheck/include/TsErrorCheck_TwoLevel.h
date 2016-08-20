#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef __TSERRORCHECK_TWOLEVEL_H__
#define __TSERRORCHECK_TWOLEVEL_H__

#include "TsErrorCheck_Common.h"

#define PCR_VALUE_MAX 0x3ffffffffffULL /*42Bit*/
#define TWOLEVEL_PCRBASETIME_MAX 0x1ffffffffULL

#define TWOLEVEL_BYTEPOS_MAX (TSERROR_INVALID_U32)	/*字节位置的最大值*/
#define TWOLEVEL_TIME_INVALID 0
#define TWOLEVEL_TIME_MAX (TSERROR_INVALID_U32)

#define TWOLEVEL_PCR_TSRATE_MIN 	(1000000)	/*监测的最小码率:1M*/
#define TWOLEVEL_PCR_TSRATE_MAX 	(TWOLEVEL_PCR_TSRATE_MIN * 200) /*监测的最大码率:200M*/

/*CRC error information*/
typedef struct
{
	GOSTSR_U32 totalCrcErrorCount;
	GOSTSR_U32 patCrcErrorCount;
	GOSTSR_U32 catCrcErrorCount;
	GOSTSR_U32 pmCrctErrorCount;
	GOSTSR_U32 nitCrcErrorCount;
	GOSTSR_U32 batCrcErrorCount;
	GOSTSR_U32 sdtCrcErrorCount;
	GOSTSR_U32 eitCrcErrorCount;
}TwoLevel_CrcError_Info;

typedef enum
{
	TWOLEVEL_ERRORTYPE_PCR = 0x01,
	TWOLEVEL_ERRORTYPE_PES,
	
	TWOLEVEL_ERRORTYPE_MAX	
}TWOLEVEL_ERROR_TYPE_E;

typedef struct
{
	GOSTSR_U32 totalCatErrorCount;
	GOSTSR_U32 tableIDErrorCount;
	GOSTSR_U32 scambleErrorCount;
}TwoLevel_CatError_Info;

/*TsRate info*/
typedef struct
{
	GOSTSR_U32 u32BytePos;
	GOSTSR_U32 u32TransportRate;
	GOSTSR_U64 u64PcrBase;
	GOSTSR_U16 u16PcrExt;
	GOSTSR_U16 u16PcrPid;
	GOSTSR_BOOL bFirstUsed;
}TwoLevel_TsRateDis_Info;

/*pcr info*/
typedef struct
{
	GOSTSR_U32 u32BytePos;
	GOSTSR_U32 u32PcrValueNs;		/*PCR_discont Unit:ns*/
	GOSTSR_U64 u64PcrBase;
	GOSTSR_U16 u16PcrExt;
	GOSTSR_U16 u16PcrPid;
	GOSTSR_BOOL bFirstUsed;
}TwoLevel_PcrDis_Info;

/*pes info*/
typedef struct
{
	GOSTSR_U32 u32BytePos;
	GOSTSR_U16 u16PesPid;
	GOSTSR_U16 u16PcrPid;
	GOSTSR_BOOL bFirstUsed;
}TwoLevel_PesDis_Info;

/*ts two level error check information*/
typedef struct
{
	GOSTSR_U32 u32TStransmissionErrorCount;  //传输错误
	TwoLevel_CrcError_Info stCrcErrorInfo;
	GOSTSR_U32 u32PcrDisErrorCount;
	GOSTSR_U32 u32PcrJitErrorCount;
	GOSTSR_U32 u32PtsErrorCount;
	TwoLevel_CatError_Info stCatErrorInfo;

}TSERROR_TWOLEVEL_S;

/*二级错误的全局变量结构体*/
typedef struct
{
	GOSTSR_U32 bTsRateisRecved;
	GOSTSR_U32 TsRate_Valid;		/*有效码率*/
	
	TSERROR_TWOLEVEL_S stTwoLevel;
	TwoLevel_TsRateDis_Info stTsRateInfo[PCRPID_NUM_MAX];
	TwoLevel_PcrDis_Info stPcrInfo[PCRPID_NUM_MAX];
	TwoLevel_PesDis_Info stPesInfo[PESPID_NUM_MAX];
}TSERROR_TWOLEVEL_PARAM_S;

extern GOSTSR_S32 TsErrorCheck_TwoLevel_Init();
extern GOSTSR_S32 TsErrorCheck_TwoLevel_DeInit();
extern GOSTSR_S32 TsErrorCheck_TwoLevel_ReInit(GOSTSR_U8 chanID);
extern GOSTSR_S32 TsErrorCheck_TwoLevel_getTwoLevelError(GOSTSR_U8 chanID,TSERROR_TWOLEVEL_S *pTwoLevel);

extern GOSTSR_S32 TsErrorCheck_TwoLevel_checkTransportError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U8 u8Error_indicater);
extern GOSTSR_S32 TsErrorCheck_TwoLevel_checkCrcError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U8 u8TableID,GOSTSR_U32 u32TableCrc,GOSTSR_U32 u32CalCrc);
extern GOSTSR_S32 TsErrorCheck_TwoLevel_checkPcrDiscontError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U32 index,GOSTSR_U64 u64Pcr_Base, GOSTSR_U16 u64Pcr_Ex,GOSTSR_BOOL *bErrorFlagt);
extern GOSTSR_S32 TsErrorCheck_TwoLevel_checkPcrAccuracyError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U32 index,GOSTSR_U64 u64Pcr_Base, GOSTSR_U16 u64Pcr_Ex);
extern GOSTSR_S32 TsErrorCheck_TwoLevel_checkPcrError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U16 u16PcrPid,GOSTSR_U64 u64Pcr_Base, GOSTSR_U16 u64Pcr_Ext, GOSTSR_BOOL flag);
extern GOSTSR_S32 TsErrorCheck_TwoLevel_checkPtsError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U16 u16PesPid);
extern GOSTSR_S32 TsErrorCheck_TwoLevel_checkCatError(TR101290_ERROR_S *pstErrorInfo,GOSTSR_U16 u16Pid, GOSTSR_U8 u8TableID, GOSTSR_U8 u8Scramble);

extern GOSTSR_U32 TsErrorCheck_TwoLevel_getTimeUs_byBytePos(GOSTSR_U8 chanID,GOSTSR_U32 u32BytePos);
extern GOSTSR_U32  TsErrorCheck_TwoLevel_getAvTransportRate(GOSTSR_U8 chanID);
extern GOSTSR_S32 TsErrorCheck_TwoLevel_setPid(GOSTSR_U8 chanID,SEARCH_INFO_S *stSearchInfo);

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /*  __cplusplus  */

