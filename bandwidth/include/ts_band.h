/// @file ts_band.h
/// @brief 
/// @author tengym <tengym@gospell.com>
/// 0.01_0
/// @date 2016-08-17


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef __TS_BAND_H__
#define __TS_BAND_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "TsErrorCheck_Common.h"

#define TSBAND_CHANNEL_MAX  2
#define TSBAND_PROG_PES_MAX 8
#define TSBAND_PID_MAX      256

/*所有package统计结构*/
typedef struct
{
    unsigned int    total_bandwidth;
    long long       total_packets;

    /*null package*/
    long long       null_packets;
    unsigned int    null_bandwidth;
    float           null_percent;

    /*payload package*/
    long long       payload_packets;
    unsigned int    payload_bandwidth;
    float           payload_percent;

    /*psisi package*/
    long long       psisi_packets;
    unsigned int    psisi_bandwidth;
    float           psisi_percent;
}TSBAND_PACKAGE_INFO;

/*音视频带宽和占空比统计结构*/
typedef struct
{
    unsigned int pid;
    long long packets;

    unsigned int bandwidth;
    float percent;
}TSBAND_VIDEO_AUDIO_INFO;

typedef struct
{
    unsigned int pmt_pid;

    /*service*/
    unsigned int serviceID;
    long long service_packets;
    unsigned int service_bandwidth;
    float service_percent;
    
    /*video*/
    unsigned int nb_video;
    TSBAND_VIDEO_AUDIO_INFO video_info[TSBAND_PROG_PES_MAX];

    /*audio*/
    unsigned int nb_audio;
    TSBAND_VIDEO_AUDIO_INFO audio_info[TSBAND_PROG_PES_MAX];

}PES_BANDWIDTH_INFO;

typedef struct
{
    int                  nb_prog;
    PES_BANDWIDTH_INFO   stprog[SEARCH_PROG_NUM_MAX];
}TSBAND_PES_INFO;

/*所有pid统计结构*/
typedef struct
{
    char            used;

    long long       packets;
    unsigned int    pid;
    unsigned int    bandwidth;
    float           percent;
}PID_BANDWIDTH_INFO;

typedef struct
{
    int                  nb_pid;
    PID_BANDWIDTH_INFO   pid_info[TSBAND_PID_MAX];
}TSBAND_PID_INFO;

int tsband_set_startflag(int startflag);
int tsband_set_proginfo(int chanID, SEARCH_INFO_S *psearch_info);
int tsband_set_bitrate(int chanID, unsigned int bitrate);

int tsband_statistics_totalpacket(int chanID);
int tsband_statistics_pidpacket(int chanID, const void *pData);
int tsband_statistics_pespacket(int chanID, const void *pData);
int tsband_statistics_packagepacket(int chanID, const void *pData);

int tsband_get_packageinfo(int chanID, TSBAND_PACKAGE_INFO *package_info);
int tsband_get_pesinfo(int chanID, TSBAND_PES_INFO *pes_info);
int tsband_get_pidinfo(int chanID, TSBAND_PID_INFO *pid_info);

int tsband_creatthread_showresult(int time);

int tsband_init();
int tsband_deinit();

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /*  __cplusplus  */
