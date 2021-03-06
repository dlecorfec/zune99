#ifndef __TEXTENGINE_H__
#define __TEXTENGINE_H__

#include <zunepriv.h>

/* This is the clone of the MUI text engine, used by the Text class
 * and all class needing to draw text strings.
 * 
 */

struct ZText {
    WORD width; /* store calculated bounds - read only ! */
    WORD height;
/* private */
    GList *lines;
    UBYTE align;
    ULONG style;
    LONG  dripen;
};

typedef struct ZText ZText;

/* zune_text_new argtypes */
enum {
    ZTEXT_ARG_NONE,
    ZTEXT_ARG_HICHAR,    /* following arg is the HiChar */
    ZTEXT_ARG_HICHARIDX, /* following arg is the HiCharIdx */
};

ZText *
zune_text_new (STRPTR preparse, STRPTR content, gint argtype, guchar arg);

void
zune_text_destroy (ZText *text);

void
zune_text_get_bounds (ZText *text, GdkFont *font);

void
/*  zune_text_draw (ZText *text, Object *obj, WORD left, WORD top); */
zune_text_draw (ZText *text, Object *obj, WORD left, WORD right, WORD top);

void
zune_text_get_real_pos (ZText *text, Object *obj, WORD *left, WORD *right);

#endif
