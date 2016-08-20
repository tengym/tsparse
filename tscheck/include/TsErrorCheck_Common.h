#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef __TSERRORCHECK_COMMON_H__
#define __TSERRORCHECK_COMMON_H__

#include "GosTsr_Common.h"

#define STREAM_TYPE_VIDEO_MPEG1     0x01
#define STREAM_TYPE_VIDEO_MPEG2     0x02
#define STREAM_TYPE_AUDIO_MPEG1     0x03
#define STREAM_TYPE_AUDIO_MPEG2     0x04
#define STREAM_TYPE_PRIVATE_SECTION 0x05
#define STREAM_TYPE_PRIVATE_DATA    0x06
#define STREAM_TYPE_AUDIO_AAC       0x0f
#define STREAM_TYPE_VIDEO_MPEG4     0x10
#define STREAM_TYPE_VIDEO_H264      0x1b
#define STREAM_TYPE_VIDEO_VC1       0xea
#define STREAM_TYPE_VIDEO_DIRAC     0xd1

#define STREAM_TYPE_AUDIO_AC3       0x81
#define STREAM_TYPE_AUDIO_DTS       0x8a

#define TSERROR_INVALID_U64 0xffffffffffffffffULL
#define TSERROR_INVALID_U32 0xffffffff
#define TSERROR_INVALID_U16 0xffff
#define TSERROR_INVALID_U8 0xff

#define SEARCH_PROG_NUM_MAX		32	//当前频点的最大节目数
#define SEARCH_CA_NUM_MAX		10	//CA最大数
#define EACH_PROG_PES_MAX		8	//每个节目的pes最大数

#define PCRPID_NUM_MAX	(SEARCH_PROG_NUM_MAX)
#define PESPID_NUM_MAX	(SEARCH_PROG_NUM_MAX * EACH_PROG_PES_MAX)

typedef struct
{
    char serviceName[256];
	GOSTSR_U8 u8Nbaudio;
	GOSTSR_U16 audio_pid[EACH_PROG_PES_MAX];
	GOSTSR_U8 u8Nbvideo;
	GOSTSR_U16 video_pid[EACH_PROG_PES_MAX];
	GOSTSR_U16 serviceID;
}SERVICE_INFO_S;

typedef struct
{
	GOSTSR_BOOL	bUsed;

	GOSTSR_U16 PmtPid;
	GOSTSR_U16 PcrPid;
	GOSTSR_U8 u8NbPes;
	GOSTSR_U16 PesPid[EACH_PROG_PES_MAX];

    /*单独定义变量，兼容之前版本*/
    SERVICE_INFO_S stserviceinfo;
}SEARCH_PROG_INFO_S;

typedef struct
{
	GOSTSR_U16 CaPid;
}SEARCH_CA_INFO_S;

typedef struct
{
	GOSTSR_U16	u16NbProg;
	SEARCH_PROG_INFO_S stProgInfo[SEARCH_PROG_NUM_MAX];

	GOSTSR_U8	u16NbCa;
	SEARCH_CA_INFO_S stCaInfo[SEARCH_CA_NUM_MAX];	/*有可能有多个CA系统，最大不差过10*/
} SEARCH_INFO_S;


typedef struct
{
	GOSTSR_U8 	u8PatVersion;
	GOSTSR_BOOL	bUpdateFlag;	/*PAT更新标志*/
	
	SEARCH_INFO_S stSearchInfo;
} SEARCH_UPDATE_INFO_S;

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /*  __cplusplus  */
