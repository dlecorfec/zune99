#ifndef __ZUNE_FONT_H__
#define __ZUNE_FONT_H__

#include <gdk/gdktypes.h>
#include <zune/amitypes.h>

GdkFont *zune_font_get (LONG preset);
void zune_font_replace (GdkFont **font, STRPTR fontname);



#endif
