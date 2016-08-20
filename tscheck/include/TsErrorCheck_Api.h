#ifndef _TSERRORCHECK_Api_H
#define _TSERRORCHECK_Api_H

#include "TsErrorCheck_Common.h"

#define API_PACKETBUFF_NUM		10

/*Api的全局变量结构体*/
typedef struct
{
	GOSTSR_BOOL bPacketType;
	GOSTSR_U32	u32PacketLen;
		
	GOSTSR_U32 u32Api_PktCount;	/*包个数统计*/
	GOSTSR_U32 u32Api_BytePos;	/*字节位置统计*/
	
	GOSTSR_BOOL bisMatched;	/*匹配标识，类似搜台*/	
	GOSTSR_BOOL bSyncisOk;	/*是否已同步*/
	GOSTSR_BOOL bSyncisLoss;	/*同步之后是否发生失步*/
	GOSTSR_U8 u8SyncNum;		/*同步包的个数*/

	GOSTSR_BOOL bSearchFlag;
	GOSTSR_U16 u16SearchCount;		/*用于search prog*/
	
	SEARCH_UPDATE_INFO_S stSearchUpdateInfo;
	
}TSERROR_API_PARAM_S;

typedef enum
{
	PACKET_ESTYPE_VIDEO = 0x00,
	PACKET_ESTYPE_AUDIO,
	PACKET_ESTYPE_PRIVATE,
	
	PACKET_ESTYPE_MAX
}PACKET_STREAM_TYPE_E;

extern GOSTSR_S32 TsErrorCheck_Api_Init();
extern GOSTSR_S32 TsErrorCheck_Api_DeInit();
extern GOSTSR_S32 TsErrorCheck_Api_ReInit(GOSTSR_U8 chanID);

extern GOSTSR_S32 TsErrorCheck_Api_AnalysisPacket(GOSTSR_U8 chanID,GOSTSR_BOOL bCarryFlag,GOSTSR_U32 u32BytePos,GOSTSR_U8 *pData, GOSTSR_U32 u32DataLen);
extern GOSTSR_S32 TsErrorCheck_Api_TsMonitor(GOSTSR_U8 ChanID,GOSTSR_U8 *pData, GOSTSR_U32 u32DataLen);

#endif
