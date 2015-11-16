#ifndef __ATYPES_H__
#define __ATYPES_H__

#include <glib.h>

/* Zune amiga-derived types.
 * In MUI, ULONG must be the same size as a pointer.
 */

typedef gpointer APTR;
typedef gulong ULONG;
typedef glong SLONG;

#define SLONG_FMT "ld"
#define ULONG_FMT "lu"

typedef gulong IPTR;

typedef SLONG LONG;
typedef ULONG LONGBITS;

typedef guint16 UWORD;
typedef gint16 SWORD;
typedef SWORD WORD;

typedef gint8 BYTE;
typedef guint8 UBYTE;
typedef gboolean BOOL;

typedef gchar *STRPTR;

#define VOID void

struct IBox {
    WORD Left;
    WORD Top;
    WORD Width;
    WORD Height;
};

/* portability stuff */
#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

#undef REG
#ifdef FAKE_REG_GNUC
#define REG(param, r) param
#else
#define REG(r) /* nothing */
#endif

#undef ASM
#define ASM

#undef SAVEDS
#define SAVEDS

/* always success */
#define OpenLibrary(name, vmin) ((void *)(name))

#define CloseLibrary(base) /* nothing */

#endif
