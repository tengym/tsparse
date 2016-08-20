/// @file staticfile.c
/// @brief 
/// @author tengym <tengym@gospell.com>
/// 0.01_0
/// @date 2016-08-18

#include "TsErrorCheck_Api.h"
#include "staticfile.h"

#define TWOLEVEL_BYTEPOS_MAX 0xffffffff
#define SYNC_BYTE 0x47
#define TS_LENGTH_188 188
#define TS_LENGTH_204 204

static GOSTSR_U64 gPacketNumber = 0;


static GOSTSR_S32 staticfile_parse_packettype(GOSTSR_U32 *pu32PacketLen, GOSTSR_U8 *filePath)
{
	FILE *sFp = GOSTSR_NULL;
	GOSTSR_U64 fileLen = 0;
	GOSTSR_U8 chr = '\0';
	GOSTSR_BOOL checkFlag = GOSTSR_FALSE;
	GOSTSR_U64 offset = 0;
	GOSTSR_U64 tempOffset = 0;
	GOSTSR_U32 retLen = 0;
	GOSTSR_U32 packageLen = 0;
	GOSTSR_U8 *tsData = GOSTSR_NULL;

	if (NULL == filePath)
	{
		return GOSTSR_FAILURE;
	}

	sFp = fopen(filePath, "rb");
	if (NULL == sFp)
	{
		return GOSTSR_FAILURE;
	}

	fseek(sFp, 0, SEEK_END);
	fileLen = ftell(sFp);
	fseek(sFp, 0, SEEK_SET);
	do
	{
		chr = fgetc(sFp);
		offset++;

		if ((chr == EOF) && (offset >= fileLen))
		{
			break;
		}

		if (chr == SYNC_BYTE)
		{
			if (tempOffset == 0)
			{
				tempOffset = offset;
			}
			else
			{
				if (((offset - tempOffset) == TS_LENGTH_188) || ((offset - tempOffset) == TS_LENGTH_204))
				{
					offset = tempOffset;
					checkFlag = GOSTSR_TRUE;
				}
				else
				{
					tempOffset = offset;
				}
			}
		}

	}while(!checkFlag);

	if (chr == EOF)
	{
		fclose(sFp);
		sFp = GOSTSR_NULL;
		return GOSTSR_FAILURE;
	}

	offset = offset - 1;
	fseek(sFp, offset, SEEK_SET);

	/*choose package size 188 0r 204*/
	tsData = (GOSTSR_U8 *)malloc(TS_LENGTH_204 + 1);
	if (GOSTSR_NULL == tsData)
	{
		fclose(sFp);
		sFp = GOSTSR_NULL;
		return GOSTSR_FAILURE;
	}
	memset(tsData, 0x00, TS_LENGTH_204 + 1);
	retLen = fread(tsData, 1, TS_LENGTH_204 + 1, sFp);
	if (retLen != TS_LENGTH_204 + 1)
	{
		fclose(sFp);
		sFp = GOSTSR_NULL;
		if (GOSTSR_NULL != tsData)
		{
			free(tsData);
			tsData = GOSTSR_NULL;
		}
		return GOSTSR_FAILURE;
	}

	if((tsData[0] == SYNC_BYTE) && (tsData[188] == SYNC_BYTE))
	{
		/*package size 188*/
		packageLen = TS_LENGTH_188;
	}
	else if((tsData[0] == SYNC_BYTE) && (tsData[188] != SYNC_BYTE) && \
		(tsData[204] == SYNC_BYTE))
	{
		/*package size 204*/
		packageLen = TS_LENGTH_204;
	}
	else
	{
		fclose(sFp);
		sFp = GOSTSR_NULL;
		if (GOSTSR_NULL != tsData)
		{
			free(tsData);
			tsData = GOSTSR_NULL;
		}
		return GOSTSR_FAILURE;
	}

	if (GOSTSR_NULL != tsData)
	{
		free(tsData);
		tsData = GOSTSR_NULL;
	}
	
	*pu32PacketLen = packageLen;
	
	return GOSTSR_SUCCESS;
}

GOSTSR_S32 staticfile_read_tsfile(GOSTSR_U8 *filePath)
{
	FILE *sFp = GOSTSR_NULL;
	GOSTSR_U32 fileLen = 0;
	GOSTSR_U32 u32BytePos = 0;
	GOSTSR_U32 i = 0;
    GOSTSR_U32 offsetPos = 0;
	GOSTSR_U32 packageLen = 0;
	GOSTSR_U8 offsetPos_Carry = 0;
	GOSTSR_U8 chr = '\0';
	GOSTSR_U8 *tsData = GOSTSR_NULL;	
	GOSTSR_BOOL endState = GOSTSR_TRUE;
	GOSTSR_U32 curSyncOffset = 0;
	GOSTSR_U32 outSyncOffset = 0;
	GOSTSR_U32 tmpSyncOffset = 0;
	GOSTSR_U32 tmpSyncOffsetbak = 0;
	GOSTSR_BOOL bCarryFlag = GOSTSR_FALSE;

	if (GOSTSR_NULL == filePath)
	{
		printf("  return failure %d \n", __LINE__);
		return GOSTSR_FAILURE;
	}

	if(GOSTSR_FAILURE == staticfile_parse_packettype(&packageLen,filePath))
	{
		printf("  return failure %d \n", __LINE__);
		return GOSTSR_FAILURE;
	}
	printf("packageLen = %d\n",packageLen);
	sFp = fopen(filePath, "rb");
	if (GOSTSR_NULL == sFp)
	{
		printf("  return failure %d \n", __LINE__);
		return GOSTSR_FAILURE;
	}
	
	tsData = (GOSTSR_U8 *)malloc(packageLen);
	if (GOSTSR_NULL == tsData)
	{
		fclose(sFp);
		sFp = GOSTSR_NULL;
		printf("  return failure %d \n", __LINE__);
		return GOSTSR_FAILURE;
	}

	fseek(sFp, 0, SEEK_END);
    	fileLen = ftell(sFp);
	fseek(sFp, 0, SEEK_SET);
	printf("______ TS File size = %dM_____\n\n",fileLen / 1024 /1024);

	while(endState)
	{
		offsetPos = ftell(sFp);
		do
		{
			fseek(sFp, offsetPos, SEEK_SET);
			chr = fgetc(sFp);
			offsetPos++;
			if(offsetPos >= TWOLEVEL_BYTEPOS_MAX)
			{
				offsetPos_Carry++;
				offsetPos = 0;	
				bCarryFlag = GOSTSR_TRUE;
			}
			
			if (offsetPos_Carry*TWOLEVEL_BYTEPOS_MAX+offsetPos >= (fileLen - 1))
			{
				endState = GOSTSR_FALSE;
			}

		}while(chr != SYNC_BYTE);

		if (chr == SYNC_BYTE)
		{
			offsetPos = offsetPos - 1;
			curSyncOffset = offsetPos;
			fseek(sFp, offsetPos, SEEK_SET);

			for(i = 0;i < 5;i++)
			{
				if(fread(tsData, 1, packageLen, sFp) < packageLen)
				{
					endState = GOSTSR_FALSE;
					break;//文件结尾处理
				}
								
				if(tsData[0] != SYNC_BYTE)
				{
					break;
				}
			}

			if(i != 5)
			{
				fseek(sFp, curSyncOffset + 1, SEEK_SET);
				continue;
			}
			else
			{
				if((curSyncOffset > outSyncOffset) && outSyncOffset != 0)
				{
					tmpSyncOffset = outSyncOffset;
					if((curSyncOffset - outSyncOffset) > packageLen)
					{
						while(tmpSyncOffset <= curSyncOffset)
						{
							do
							{
								fseek(sFp, tmpSyncOffset, SEEK_SET);
								chr = fgetc(sFp);
								tmpSyncOffset++;

								if(tmpSyncOffset > curSyncOffset)
									{break;}
							}while(chr != SYNC_BYTE);

							if(chr == SYNC_BYTE)
							{
								tmpSyncOffset -= 1;
								if(tmpSyncOffset + packageLen < curSyncOffset)
								{
									fseek(sFp, tmpSyncOffset + packageLen, SEEK_SET);
									chr = fgetc(sFp);
									if(chr == SYNC_BYTE)
									{
										tmpSyncOffsetbak = tmpSyncOffset;
										tmpSyncOffset += packageLen;
									}
									else
									{
										tmpSyncOffset++;
									}
								}
								else
								{
									break;
								}
							}
						}
					}
				}
			
				fseek(sFp, curSyncOffset, SEEK_SET);
				do
				{
					offsetPos = ftell(sFp);
					if(fread(tsData, 1, packageLen, sFp) < packageLen)
					{
						endState = GOSTSR_FALSE;
						break;
					}
					if(tsData[0] == SYNC_BYTE)
					{
						u32BytePos = offsetPos;
			
						//FileDeal_AnalysisPacket(bCarryFlag,u32BytePos,tsData, packageLen);
                        TsErrorCheck_Api_AnalysisPacket(0, bCarryFlag, u32BytePos, tsData, packageLen);
						if(bCarryFlag)/*将进位标志置为初始化状态*/
						{
							bCarryFlag = GOSTSR_FALSE;
						}
						gPacketNumber++;
						if(gPacketNumber >= TWOLEVEL_BYTEPOS_MAX)
						{
							gPacketNumber = 0;
						}
					}
					else
					{
						outSyncOffset = offsetPos;
						fseek(sFp, outSyncOffset, SEEK_SET);
						break;
					}
				}while(1);
			}
		}
	}

LOOP_FILE_END:
	free(tsData);
	tsData = GOSTSR_NULL;

	if (GOSTSR_SUCCESS != fclose(sFp))
	{
		return GOSTSR_FAILURE;
	}
	sFp = GOSTSR_NULL;

	return GOSTSR_SUCCESS;
}
