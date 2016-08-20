/// @file ts_band.c
/// @brief 
/// @author tengym <tengym@gospell.com>
/// 0.01_0
/// @date 2016-08-17

#include "ts_band.h"

/*NULL Packet PID*/
#define NULL_PID_BAND   0x1FFF

/*PSI PID*/
#define PAT_PID_BAND	0x0000
#define CAT_PID_BAND    0x0001
#define NIT_PID_BAND    0x0010

/*SI PID*/
#define TSDT_PID_BAND       0x0002
#define BAT_SDT_PID_BAND    0x0011
#define EIT_PID_BAND        0x0012
#define RST_PID_BAND        0x0013
#define TDT_TOT_PID_BAND    0x0014
#define DIT_PID_BAND        0x001E
#define SIT_PID_BAND        0x001F

static int g_band_startflag = 0;
static pthread_t tsband_showresult_handle = 0;

static TSBAND_PACKAGE_INFO gst_band_packageinfo[TSBAND_CHANNEL_MAX];
static TSBAND_PES_INFO gst_band_pesinfo[TSBAND_CHANNEL_MAX];
static TSBAND_PID_INFO gst_band_pidinfo[TSBAND_CHANNEL_MAX];

static int ts_band_get_tspid(const void *pData, unsigned int *pid)
{
    if((NULL == pid) || (NULL == pData))
    {
        return -1;
    }
    unsigned char *curData = (unsigned char *)pData;

    *pid = (curData[1] & 0x1f) * 256 + curData[2];

    return 0;
}

int tsband_set_startflag(int startflag)
{
    g_band_startflag = startflag;
    return 0;
}

/*获取节目信息*/
int tsband_service_set_proginfo(int chanID, SEARCH_INFO_S *psearch_info)
{
    if((chanID >= TSBAND_CHANNEL_MAX) || (psearch_info == GOSTSR_NULL))
    {
        return -1;
    }

    int i = 0, j = 0;
    int index_video = 0, index_audio = 0;

    for(i = 0; i < psearch_info->u16NbProg;i++)
    {
       gst_band_pesinfo[chanID].stprog[i].pmt_pid = psearch_info->stProgInfo[i].PmtPid; 
       gst_band_pesinfo[chanID].stprog[i].serviceID = psearch_info->stProgInfo[i].stserviceinfo.serviceID; 

       index_video = 0;
       for(j = 0; j < psearch_info->stProgInfo[i].stserviceinfo.u8Nbvideo; j++)
       {
           gst_band_pesinfo[chanID].stprog[i].video_info[index_video].pid  = psearch_info->stProgInfo[i].stserviceinfo.video_pid[j]; 
           index_video++;
       }
       gst_band_pesinfo[chanID].stprog[i].nb_video = index_video; 

       index_audio = 0;
       for(j = 0; j < psearch_info->stProgInfo[i].stserviceinfo.u8Nbaudio; j++)
       {
           gst_band_pesinfo[chanID].stprog[i].audio_info[index_audio].pid  = psearch_info->stProgInfo[i].stserviceinfo.audio_pid[j]; 
           index_audio++;
       }
       gst_band_pesinfo[chanID].stprog[i].nb_audio = index_audio; 
    }
    gst_band_pesinfo[chanID].nb_prog = i;
    
    printf("ts_band, Nb_prog = %d--video_num:%d---audio_num:%d\n", i, index_video, index_audio);

    return 0;
}

/*设置码流，用于带宽统计*/
int tsband_set_bitrate(int chanID, unsigned int bitrate)
{
    if((bitrate == 0)||(chanID >= TSBAND_CHANNEL_MAX))
    {
        return -1;
    }
    gst_band_packageinfo[chanID].total_bandwidth = bitrate;
    return 0;
}

/*统计所有ts包*/
int tsband_statistics_totalpacket(int chanID)
{
    if((chanID >= TSBAND_CHANNEL_MAX))
    {
        return -1;
    }
    gst_band_packageinfo[chanID].total_packets++;
    
    return 0;
}

/*统计所有的pid包*/
int tsband_statistics_pidpacket(int chanID, const void *pData)
{
    if((chanID >= TSBAND_CHANNEL_MAX) || (NULL == pData))
    {
        return -1;
    }
    int i = 0;
    unsigned int pid = 0xffff;

    if(0 != ts_band_get_tspid(pData, &pid))
    {
        return -1;
    }

    for(i = 0; i < gst_band_pidinfo[chanID].nb_pid; i++)
    {
        if(pid == gst_band_pidinfo[chanID].pid_info[i].pid)
        {
            gst_band_pidinfo[chanID].pid_info[i].packets++;
            return 0;
        }
    }
    
    for(i = 0; i < TSBAND_PID_MAX; i++)
    {
        if(0 == gst_band_pidinfo[chanID].pid_info[i].used)
        {
            break;
        }
    }

    if(TSBAND_PID_MAX == i)
    {
        printf("NO Space to Store PID Packet\n");
        return -1;
    }

    gst_band_pidinfo[chanID].pid_info[i].pid = pid;
    gst_band_pidinfo[chanID].pid_info[i].packets++;
    gst_band_pidinfo[chanID].pid_info[i].used = 1;
    gst_band_pidinfo[chanID].nb_pid++;

    return 0;
}

/*统计音视频包*/
int tsband_statistics_pespacket(int chanID, const void *pData)
{
    if((chanID >= TSBAND_CHANNEL_MAX) || (NULL == pData))
    {
        return -1;
    }

    int i = 0, j = 0;
    unsigned int pid = 0xffff;

    if(0 != ts_band_get_tspid(pData, &pid))
    {
        return -1;
    }

    for(i = 0; i < gst_band_pesinfo[chanID].nb_prog;i++)
    {
       for(j = 0; j < gst_band_pesinfo[chanID].stprog[i].nb_video; j++)
       {
           if(pid == gst_band_pesinfo[chanID].stprog[i].video_info[j].pid)
           {
               gst_band_pesinfo[chanID].stprog[i].video_info[j].packets++;
               gst_band_pesinfo[chanID].stprog[i].service_packets++; 
               return 0;
           }
       }
       for(j = 0; j < gst_band_pesinfo[chanID].stprog[i].nb_audio; j++)
       {
           if(pid == gst_band_pesinfo[chanID].stprog[i].audio_info[j].pid)
           {
               gst_band_pesinfo[chanID].stprog[i].audio_info[j].packets++;
               gst_band_pesinfo[chanID].stprog[i].service_packets++; 
               return 0;
           }
       }
    }
    return 0;
}

/*统计空包、psisi包、有效数据*/
int tsband_statistics_packagepacket(int chanID, const void *pData)
{
    if((chanID >= TSBAND_CHANNEL_MAX) || (NULL == pData))
    {
        return -1;
    }
    int i = 0;
    unsigned int pid = 0xffff;

    if(0 != ts_band_get_tspid(pData, &pid))
    {
        return -1;
    }
    /*null packet*/
    if(NULL_PID_BAND == pid)
    {
        gst_band_packageinfo[chanID].null_packets++;
        return 0;
    }

    /*psi packet*/
    if((PAT_PID_BAND == pid) || (CAT_PID_BAND == pid) || (NIT_PID_BAND == pid))
    {
        gst_band_packageinfo[chanID].psisi_packets++;
        return 0;
    }

    /*si packet*/
    else if((BAT_SDT_PID_BAND  == pid) || (EIT_PID_BAND == pid) || (RST_PID_BAND == pid) || (TDT_TOT_PID_BAND == pid) \
            || (TSDT_PID_BAND == pid) || (DIT_PID_BAND == pid) || (SIT_PID_BAND == pid))
    {
        gst_band_packageinfo[chanID].psisi_packets++;
        return 0;
    }

    /*pmt packet*/
    for(i = 0; i < gst_band_pesinfo[chanID].nb_prog; i++)
    {
       if(pid == gst_band_pesinfo[chanID].stprog[i].pmt_pid) 
       {
            gst_band_packageinfo[chanID].psisi_packets++;
            return 0;
       }
    }

    return 0;
}

/*获取空包、有效负载，以及psisi信息*/
int tsband_get_packageinfo(int chanID, TSBAND_PACKAGE_INFO *package_info)
{
    if((NULL == package_info) || (chanID >= TSBAND_CHANNEL_MAX))
    {
        return -1;
    }

    int i = 0;

    unsigned int bitrate = gst_band_packageinfo[chanID].total_bandwidth;
    long long totalpacket = gst_band_packageinfo[chanID].total_packets;
    if((totalpacket == 0))
    {
        printf("total packet is 0\n");
        return -1;
    }

    gst_band_packageinfo[chanID].payload_packets = gst_band_packageinfo[chanID].total_packets - gst_band_packageinfo[chanID].null_packets \
                                                   - gst_band_packageinfo[chanID].psisi_packets;

    /*null packet*/
    gst_band_packageinfo[chanID].null_bandwidth = (float)gst_band_packageinfo[chanID].null_packets / totalpacket * bitrate;
    gst_band_packageinfo[chanID].null_percent = (float)gst_band_packageinfo[chanID].null_packets / totalpacket;
    
    /*payload packet*/
    gst_band_packageinfo[chanID].payload_bandwidth = (float)gst_band_packageinfo[chanID].payload_packets / totalpacket * bitrate;
    gst_band_packageinfo[chanID].payload_percent = (float)gst_band_packageinfo[chanID].payload_packets / totalpacket;

    /*psisi packet*/
    gst_band_packageinfo[chanID].psisi_bandwidth = (float)gst_band_packageinfo[chanID].psisi_packets / totalpacket * bitrate;
    gst_band_packageinfo[chanID].psisi_percent = (float)gst_band_packageinfo[chanID].psisi_packets / totalpacket;

    memset(package_info, 0, sizeof(TSBAND_PACKAGE_INFO));
    memcpy(package_info, &gst_band_packageinfo[chanID], sizeof(TSBAND_PACKAGE_INFO));

    return 0;
}

/*获取音视频带宽，占空比等信息*/
int tsband_get_pesinfo(int chanID, TSBAND_PES_INFO *pes_info)
{
    if((NULL == pes_info) || (chanID >= TSBAND_CHANNEL_MAX))
    {
        printf("NULL\n");
        return -1;
    }

    int i = 0, j = 0;
    long long video_packet = 0;
    long long audio_packet = 0;
    long long service_packet = 0;

    unsigned int bitrate = gst_band_packageinfo[chanID].total_bandwidth;
    long long totalpacket = gst_band_packageinfo[chanID].total_packets;
    if((totalpacket == 0))
    {
        printf("total packet is 0\n");
        return -1;
    }

    for(i = 0; i < gst_band_pesinfo[chanID].nb_prog; i++)
    {
        /*serviceID*/
        service_packet = gst_band_pesinfo[chanID].stprog[i].service_packets;
        gst_band_pesinfo[chanID].stprog[i].service_bandwidth = (float)service_packet/totalpacket*bitrate;
        gst_band_pesinfo[chanID].stprog[i].service_percent = (float)service_packet/totalpacket;

        /*video*/
        for(j = 0; j < gst_band_pesinfo[chanID].stprog[i].nb_video; j++)
        {
            video_packet = gst_band_pesinfo[chanID].stprog[i].video_info[j].packets;

            gst_band_pesinfo[chanID].stprog[i].video_info[j].bandwidth = (float)video_packet/totalpacket*bitrate;
            gst_band_pesinfo[chanID].stprog[i].video_info[j].percent = (float)video_packet/totalpacket;
        }

        /*audio*/
        for(j = 0; j < gst_band_pesinfo[chanID].stprog[i].nb_audio; j++)
        {
            audio_packet = gst_band_pesinfo[chanID].stprog[i].audio_info[j].packets;

            gst_band_pesinfo[chanID].stprog[i].audio_info[j].bandwidth = (float)audio_packet/totalpacket*bitrate;
            gst_band_pesinfo[chanID].stprog[i].audio_info[j].percent = (float)audio_packet/totalpacket;
        }
    }

    memset(pes_info, 0, sizeof(TSBAND_PES_INFO));
    memcpy(pes_info, &gst_band_pesinfo[chanID], sizeof(TSBAND_PES_INFO));

    return 0;
}

int tsband_get_pidinfo(int chanID, TSBAND_PID_INFO *pid_info)
{
    if((NULL == pid_info) || (chanID >= TSBAND_CHANNEL_MAX))
    {
        return -1;
    }

    int i = 0;
    int j = 0;
    long long tmp_packet = 0;

    unsigned int bitrate = gst_band_packageinfo[chanID].total_bandwidth;
    long long totalpacket = gst_band_packageinfo[chanID].total_packets;
    if((totalpacket == 0))
    {
        printf("total packet is 0\n");
        return -1;
    }
    for(i = 0; i < gst_band_pidinfo[chanID].nb_pid; i++)
    {
        tmp_packet = gst_band_pidinfo[chanID].pid_info[i].packets;
        gst_band_pidinfo[chanID].pid_info[i].bandwidth = (float)tmp_packet / totalpacket * bitrate;
        gst_band_pidinfo[chanID].pid_info[i].percent = (float)tmp_packet / totalpacket;
    }

    PID_BANDWIDTH_INFO tmp_pid_info;
    memset(&tmp_pid_info, 0, sizeof(PID_BANDWIDTH_INFO));
    /*pid升序排序*/
    for(i = 0; i < gst_band_pidinfo[chanID].nb_pid - 1; i++)
    {
        for(j = i; j < gst_band_pidinfo[chanID].nb_pid; j++)
        {
            if(gst_band_pidinfo[chanID].pid_info[i].pid > gst_band_pidinfo[chanID].pid_info[j].pid)
            {
                memcpy(&tmp_pid_info, &gst_band_pidinfo[chanID].pid_info[i], sizeof(PID_BANDWIDTH_INFO));
                memcpy(&gst_band_pidinfo[chanID].pid_info[i], &gst_band_pidinfo[chanID].pid_info[j], sizeof(PID_BANDWIDTH_INFO));
                memcpy(&gst_band_pidinfo[chanID].pid_info[j], &tmp_pid_info, sizeof(PID_BANDWIDTH_INFO));
            }
        }
    }

    memset(pid_info, 0, sizeof(TSBAND_PID_INFO));
    memcpy(pid_info, &gst_band_pidinfo[chanID], sizeof(TSBAND_PID_INFO));

    return 0;
}

static int ts_band_show_pidinfo(int chanID)
{
    if(chanID >= TSBAND_CHANNEL_MAX)
    {
        return -1;
    }

    int i = 0;
    int ret = 0;


    TSBAND_PID_INFO pid_info;
    memset(&pid_info, 0, sizeof(TSBAND_PID_INFO));
    ret = tsband_get_pidinfo(chanID, &pid_info);
    if(0 != ret)
    {
        return -1;
    }
    
    printf("\n=================================PID========================================\n");
    for(i = 0; i < pid_info.nb_pid; i++)
    {
        printf("pid: %#6x---bandwidth = %.2fkbps----percent = %.3f%%---packets:%lld\n",pid_info.pid_info[i].pid, (float)pid_info.pid_info[i].bandwidth/1000, (float)pid_info.pid_info[i].percent*100, pid_info.pid_info[i].packets);
    }

    printf("\n=================================PID========================================\n");

    return 0;
}

static int ts_band_show_pesinfo(int chanID)
{
    int i = 0, j = 0;
    int ret = -1;

    TSBAND_PES_INFO pes_info;
    memset(&pes_info, 0, sizeof(TSBAND_PES_INFO));
    ret = tsband_get_pesinfo(chanID, &pes_info);
    if(0 != ret)
    {
        return -1;
    }

    for(i = 0; i < pes_info.nb_prog; i++)
    {
        printf("serviceID: %d---bandwidth = %.2fkbps----percent = %.3f%%\n",pes_info.stprog[i].serviceID, (float)pes_info.stprog[i].service_bandwidth/1000, (float)pes_info.stprog[i].service_percent*100);

        for(j = 0; j < pes_info.stprog[i].nb_video; j++)
        {
            printf("video[%d]: %#x---bandwidth = %.2fkbps----percent = %.3f%%\n", j, pes_info.stprog[i].video_info[j].pid, (float)pes_info.stprog[i].video_info[j].bandwidth/1000, (float)pes_info.stprog[i].video_info[j].percent*100);
        }

        for(j = 0; j < pes_info.stprog[i].nb_audio; j++)
        {
            printf("audio[%d]: %#x---bandwidth = %.2fkbps----percent = %.3f%%\n", j, pes_info.stprog[i].audio_info[j].pid, (float)pes_info.stprog[i].audio_info[j].bandwidth/1000, (float)pes_info.stprog[i].audio_info[j].percent*100);
        }
        printf("\n");
    }
    return 0;
}

static int ts_band_show_packageinfo(int chanID)
{
    int i = 0;
    int ret = -1;

    TSBAND_PACKAGE_INFO package_info;
    ret = tsband_get_packageinfo(chanID, &package_info);
    if(0 != ret)
    {
        return -1;
    }

    printf("   total_bandeidth = %.4fMbps\n\n", (float)package_info.total_bandwidth/1000/1000);
    printf("   null_packet:---bandwidth = %.2fkbps----percent = %.3f%%\n", (float)package_info.null_bandwidth/1000, (float)package_info.null_percent*100);
    printf("payload_packet:---bandwidth = %.2fkbps----percent = %.3f%%\n", (float)package_info.payload_bandwidth/1000, (float)package_info.payload_percent*100);
    printf("  psisi_packet:---bandwidth = %.2fkbps----percent = %.3f%%\n\n", (float)package_info.psisi_bandwidth/1000, (float)package_info.psisi_percent*100);

    return 0;
}

static void *ts_band_showresult_task(void *argv)
{
    int chanID = 0;
    int count = 0;

    while(g_band_startflag)
    {
        if(gst_band_packageinfo[chanID].total_bandwidth == 0)
        {
            continue;
        }
        printf("=================================count:%d========================================\n", count++);

        ts_band_show_packageinfo(chanID);
        ts_band_show_pesinfo(chanID);

        printf("================================================================================\n\n");

        usleep(500 * 1000);
    }

    printf("=================================count:%d========================================\n", count++);

    ts_band_show_packageinfo(chanID);
    ts_band_show_pesinfo(chanID);

    printf("================================================================================\n\n");

    printf("\npid num: %d\n", gst_band_pidinfo[chanID].nb_pid);
    ts_band_show_pidinfo(chanID);

    return NULL;
}

/*time unit: ms*/
int tsband_creatthread_showresult(int time)
{
    g_band_startflag = 1;
	pthread_create(&tsband_showresult_handle ,NULL, ts_band_showresult_task, NULL);
    return 0;
}

int tsband_init()
{
    memset(gst_band_packageinfo, 0, sizeof(gst_band_packageinfo));
    memset(gst_band_pesinfo, 0, sizeof(gst_band_pesinfo));
    memset(gst_band_pidinfo, 0, sizeof(gst_band_pidinfo));

    g_band_startflag = 0;

    return 0;
}

int tsband_deinit()
{
    memset(gst_band_packageinfo, 0, sizeof(gst_band_packageinfo));
    memset(gst_band_pesinfo, 0, sizeof(gst_band_pesinfo));
    memset(gst_band_pidinfo, 0, sizeof(gst_band_pidinfo));

    g_band_startflag = 0;

    return 0;
}
