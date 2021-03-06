#ifndef __MCC_H__
#define __MCC_H__

#include <zune/classes.h>

struct _GModule; /* dont want to include gmodule.h here */

struct MUI_CustomClass
{
    APTR mcc_UserData;                  /* use for whatever you want */

/* libraries unused in Zune */
    struct Library *mcc_UtilityBase;    /* MUI has opened these libraries */
    struct Library *mcc_DOSBase;        /* for you automatically. You can */
    struct Library *mcc_GfxBase;        /* use them or decide to open     */
    struct Library *mcc_IntuitionBase;  /* your libraries yourself.       */

    struct IClass *mcc_Super;           /* pointer to super class   */
    struct IClass *mcc_Class;           /* pointer to the new class */

/* private follows ... */
    struct _GModule *mcc_Module;        /* non-null if external class */
};


#endif
