#ifndef __TEXTDATA_H__
#define __TEXTDATA_H__

#include <zune/classes.h>
#include <textengine.h>

struct MUI_TextData {
    ULONG  mtd_Flags;
    STRPTR contents;
    STRPTR preparse;
    gchar  hichar;
    ZText *ztext;
};
 
#define MTDF_SETMIN  (1<<0)
#define MTDF_SETMAX  (1<<1)
#define MTDF_SETVMAX (1<<2)
#define MTDF_HICHAR  (1<<3)
#define MTDF_HICHARIDX (1<<4)

#endif
