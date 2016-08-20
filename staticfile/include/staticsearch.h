#ifndef __STATICSEARCH_H__
#define __STATICSEARCH_H__

#include "TsErrorCheck_Common.h"

#define STATIC_SEARCH_PROG_NUM_MAX		32
#define STATIC_SEARCH_CA_NUM_MAX		10
#define STATIC_SEARCH_PROG_PES_MAX	    8	

typedef struct
{
    char serviceName[256];
	GOSTSR_U8 u8Nbaudio;
	GOSTSR_U16 audio_pid[STATIC_SEARCH_PROG_PES_MAX];
	GOSTSR_U8 u8Nbvideo;
	GOSTSR_U16 video_pid[STATIC_SEARCH_PROG_PES_MAX];
	GOSTSR_U16 serviceID;
}STATIC_SERVICE_INFO_S;

typedef struct
{
	GOSTSR_BOOL	bUsed;

	GOSTSR_U16 PmtPid;
	GOSTSR_U16 PcrPid;
	GOSTSR_U8 u8NbPes;
	GOSTSR_U16 PesPid[STATIC_SEARCH_PROG_PES_MAX];

    STATIC_SERVICE_INFO_S stserviceinfo;
}STATIC_SEARCH_PROG_INFO_S;

typedef struct
{
	GOSTSR_U16 CaPid;
}STATIC_SEARCH_CA_INFO_S;

typedef struct
{
	GOSTSR_U16	u16NbProg;
	STATIC_SEARCH_PROG_INFO_S stProgInfo[STATIC_SEARCH_PROG_NUM_MAX];

	GOSTSR_U8	u16NbCa;
	STATIC_SEARCH_CA_INFO_S stCaInfo[STATIC_SEARCH_CA_NUM_MAX];	/*有可能有多个CA系统，最大不差过10*/
}STATIC_SEARCH_INFO_S;

GOSTSR_S32 Static_Search_getProgInfo(STATIC_SEARCH_INFO_S *pstProgInfo);

GOSTSR_S32 Static_Search_ProgSearch_Init(GOSTSR_U8 *filePath);
GOSTSR_S32 Static_Search_ProgSearch_DeInit();

#endif
