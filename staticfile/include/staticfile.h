/// @file staticfile.h
/// @brief 
/// @author tengym <tengym@gospell.com>
/// 0.01_0
/// @date 2016-08-17


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef __STATICFILE_H__
#define __STATICFILE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "TsErrorCheck_Common.h"

GOSTSR_S32 staticfile_read_tsfile(GOSTSR_U8 *filePath);

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /*  __cplusplus  */
