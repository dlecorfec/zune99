#ifndef __APPLICATIONDATA_H__
#define __APPLICATIONDATA_H__

/*  #include <zune.h> */
#include <notifydata.h>

struct MUI_ApplicationData
{
    struct MUI_GlobalInfo app_GlobalInfo;
    APTR           app_WindowFamily; /* delegates window list */
    struct MinList app_IHList;
    struct MinList app_MethodQueue;
    struct MinList app_ReturnIDQueue;
    GMemChunk     *app_RIDMemChunk;
    STRPTR         app_Author;
    STRPTR         app_Base;
    STRPTR         app_Copyright;
    STRPTR         app_Description;
    STRPTR         app_HelpFile;
    STRPTR         app_Title;
    STRPTR         app_Version;
    ULONG          app_SleepCount;
    BOOL           app_ForceQuit;
    BOOL           app_Iconified;
    BOOL           app_SingleTask;
    GMainLoop     *app_MainLoop;
    guint          app_CfgSpyTimeout;
};

#endif
