#ifndef __WINDOWDATA_H__
#define __WINDOWDATA_H__

#include <notifydata.h>
#include <renderinfo.h>
#include <minmax.h>
#include <gdk/gdktypes.h>

struct MUI_WindowData
{
    struct MUI_RenderInfo wd_RenderInfo;
    struct MUI_MinMax     wd_MinMax;
    struct IBox    wd_AltDim;       /* zoomed dimensions */
    GList         *wd_CycleChain;   /* objects activated with tab */
    struct MinList wd_EHList;       /* event handlers */
    struct MinList wd_CCList;       /* control chars */
    ULONG          wd_Events;       /* events received */
    ULONG          wd_CrtFlags;     /* window creation flags, see below */
    GList         *wd_ActiveObject; /* list node of active obj */
    APTR           wd_DefaultObject;
    ULONG          wd_ID;
    STRPTR         wd_Title;
    LONG           wd_Height;       /* Current dimensions */
    LONG           wd_Width;
    gint           wd_X;
    gint           wd_Y;
    LONG           wd_ReqHeight;    /* given by programmer */
    LONG           wd_ReqWidth;
    APTR           wd_RootObject;   /* unique child */
    ULONG          wd_Flags;        /* various status flags */
    UWORD          wd_innerLeft;
    UWORD          wd_innerRight;
    UWORD          wd_innerTop;
    UWORD          wd_innerBottom;
};

/* wd_CrtFlags */
#define MUIWCF_CLOSEGADGET (1<<0) /* has close gadget */
#define MUIWCF_SIZEGADGET (1<<1)  /* has size gadget */
#define MUIWCF_BACKDROP (1<<2)    /* is backdrop window */
#define MUIWCF_BORDERLESS (1<<3)  /* has no borders */
#define MUIWCF_DEPTHGADGET (1<<4) /* has depth gadget */
#define MUIWCF_DRAGBAR (1<<5)     /* is draggable */
#define MUIWCF_SIZERIGHT (1<<6)   /* size gadget is in right border */

/* wd_Flags */
#define MUIWF_OPENED    (1<<0) /* window currently opened */
#define MUIWF_ICONIFIED (1<<1) /* window currently iconified */
#define MUIWF_ACTIVE    (1<<2) /* window currently active */
#define MUIWF_CLOSEREQUESTED (1<<3) /* when user hits close gadget */

struct __dummyXFC3__
{
	struct MUI_NotifyData mnd;
	struct MUI_WindowData mwd;
};

#define muiWindowData(obj)   (&(((struct __dummyXFC3__ *)(obj))->mwd))

#endif



