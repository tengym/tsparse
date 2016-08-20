/// @file main.c
/// @brief 
/// @author tengym <tengym@gospell.com>
/// 0.01_0
/// @date 2016-08-18

#include "TsErrorCheck_Api.h"
#include "TsErrorCheck_Log.h"

#include "TsErrorCheck_OneLevel.h"
#include "TsErrorCheck_TwoLevel.h"
#include "TsErrorCheck_ThreeLevel.h"

#include "staticsearch.h"
#include "staticfile.h"
#include "ts_band.h"

int main(int argc, char *argv[])
{
	char filepath[256] = "149.ts";

    if(argc > 1)
    {
        strcpy(filepath, argv[1]);
    }

    tsband_init();

    TsErrorCheck_Api_Init();

#if STATIC_SEARCH_USE
    Static_Search_ProgSearch_Init(filepath);
#endif

    tsband_creatthread_showresult(500);

    staticfile_read_tsfile(filepath);
    tsband_set_startflag(0);

    while(1)
    {
        sleep(5);
        break;
    }

    return 0;
}
