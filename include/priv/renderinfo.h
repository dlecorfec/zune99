#ifndef __RENDERINFO_H__
#define __RENDERINFO_H__

#include <zune/classes.h>
#include <pen.h>
#include <gdk/gdktypes.h>

enum _dripens {
    TEXTPEN = 0,
    SHINEPEN,
    SHADOWPEN,
    BARDETAILPEN,
    BARBLOCKPEN,
    BARTRIMPEN,
};

/* nothing to do with the original structure .. */
struct DrawInfo
{
    gulong          *dri_Pens; /* lookup table for mri_Pens */
    gulong           dri_Pixels[MPEN_COUNT+2]; /* storage for pens */    
};

/* Information on display environment */
/* located in Window instance, but all Areas have a reference to it */
struct MUI_RenderInfo
{
    Object          *mri_WindowObject;  /* valid between MUIM_Setup/MUIM_Cleanup */
/*  	struct Screen   *_mri_Screen; */
    struct DrawInfo *mri_DrawInfo;
/*  	UWORD           *_mri_Pens; */
/*  	struct Window   *_mri_Window; */
/*  	struct RastPort *_mri_RastPort; */
    ULONG            mri_Flags;         /* valid between MUIM_Setup/MUIM_Cleanup */
    GdkWindow       *mri_Window;        /* valid between MUIM_Show/MUIM_Hide */
    gulong          *mri_Pens;
    GdkGC           *mri_GC;
    GdkColormap     *mri_Colormap;
    GdkVisual       *mri_Visual;
    gint             mri_ScreenWidth;
    gint             mri_ScreenHeight;
/* private follows ... */
    gulong           mri_Pixels[MPEN_COUNT]; /* storage for pens */
    GdkPixmap       *mri_PatternStipple;
    GdkRectangle     mri_ClipRect;
    GdkWindow       *mri_FocusWin[4];
    ULONG            mri_FocusPixel;
    struct DrawInfo  mri_DriHack;
};

#define mri_RastPort mri_GC

#endif
