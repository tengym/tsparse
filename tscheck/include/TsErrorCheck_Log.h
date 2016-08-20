#ifndef _TSERRORCHECK_LOG_H
#define _TSERRORCHECK_LOG_H

#include "TsErrorCheck_Common.h"

extern GOSTSR_S32 TsErrorCheck_Log_Init();
extern GOSTSR_S32 TsErrorCheck_Log_DeInit();
extern GOSTSR_S32 TsErrorCheck_Log_ReInit(GOSTSR_U8 chanID);

extern GOSTSR_S32 TS_DEBUG_LOG(char *str);
extern GOSTSR_S32 TS_ERROR_LOG(GOSTSR_U8 chanID,GOSTSR_U32 errorType, GOSTSR_U32 errorLevel,GOSTSR_U32 bytePos,void *pPrivate,const GOSTSR_U8 *pFormat);
extern GOSTSR_S32 TsErrorCheck_Log_SetErrorType(GOSTSR_U8 chanID,GOSTSR_U32 errorType);
extern GOSTSR_S32 TsErrorCheck_Log_SetErrorLevel(GOSTSR_U8 chanID,GOSTSR_U32 errorLevel);
extern GOSTSR_S32 TsErrorCheck_Log_SetNbProg(GOSTSR_U8 chanID,GOSTSR_U32 NbProg);

extern GOSTSR_S32 TsErrorCheck_Log_OneLevel_PrintInfo(GOSTSR_U8 chanID);
extern GOSTSR_S32 TsErrorCheck_Log_TwoLevel_PrintInfo(GOSTSR_U8 chanID);
extern GOSTSR_S32 TsErrorCheck_Log_ThreeLevel_PrintInfo(GOSTSR_U8 chanID);
extern GOSTSR_S32 TsErrorCheck_Log_PrintInfo(GOSTSR_U8 chanID);

extern GOSTSR_S32 TsErrorCheck_Log_ExportLog(char *pPathName,GOSTSR_U8 *pData, GOSTSR_U32 u32DataLen);

#endif /**/

