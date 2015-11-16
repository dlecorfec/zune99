#ifndef __ZUNE_PEN_H__
#define __ZUNE_PEN_H__

#include <zune/amitypes.h>
#include <gdk/gdktypes.h>

struct MUI_RenderInfo;
struct MUI_PenSpec;

typedef enum {
    MPEN_SHINE = 0,
    MPEN_HALFSHINE,
    MPEN_BACKGROUND,
    MPEN_HALFSHADOW,
    MPEN_SHADOW,
    MPEN_TEXT,
    MPEN_FILL,
    MPEN_MARK,
    MPEN_COUNT,
} MPen;

typedef struct {
    MPen fg;
    MPen bg;
} MPenCouple;

typedef enum {
    PST_MUI,
    PST_CMAP,
    PST_RGB,
} PenSpecType;

struct MUI_PenSpec {
    union {
	char buf[32]; /* constraint PenSpec size */
	struct {
	    PenSpecType ps_Type;
	    union {
		MPen     mui;   /* MUI pen number */
		ULONG    cmap;  /* colormap entry */
		struct {
		    GdkColor rgb;
		    STRPTR   text;
		} c;
	    } v;
	} s;
    } u;
};

#define ps_penType u.s.ps_Type
#define ps_rgbColor u.s.v.c.rgb
#define ps_rgbText u.s.v.c.text
#define ps_mui u.s.v.mui
#define ps_cmap u.s.v.cmap

/* get the pixel for a given penspec */
LONG
MUI_ObtainPen (struct MUI_RenderInfo *mri,struct MUI_PenSpec *spec,ULONG flags);

VOID
MUI_ReleasePen (struct MUI_RenderInfo *mri,LONG pen);

/* set drawing color with the result from MUI_ObtainPen */
void
SetAPen (GdkGC *gc, gulong pen);

void
SetBPen (GdkGC *gc, gulong pen);

/* dump a penspec */
/* as usual, returns ptr to static storage */
STRPTR
zune_penspec_to_string(struct MUI_PenSpec *spec);

/* create a penspec */
/* it's legal to pass a NULL str param here */
void
zune_string_to_penspec(STRPTR str, struct MUI_PenSpec *spec);

/* dispose pen content, matching zune_string_to_penspec */
void zune_penspec_destroy_content (struct MUI_PenSpec *spec);

/* prepare penspec for use with a given mri (window) */
void
zune_penspec_setup (struct MUI_RenderInfo *mri, struct MUI_PenSpec *spec);

void
zune_penspec_cleanup (struct MUI_RenderInfo *mri, struct MUI_PenSpec *spec);

/* gdk color stream IO */

STRPTR
zune_gdkcolor_to_string(GdkColor *rgb);

void
zune_string_to_gdkcolor (STRPTR str, GdkColor *rgb);

#endif
