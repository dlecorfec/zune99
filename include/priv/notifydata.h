#ifndef __NOTIFYDATA_H__
#define __NOTIFYDATA_H__

#include <zune/amitypes.h>
#include <zune/classes.h>

struct MUI_GlobalInfo
{
    ULONG priv0;
    Object *mgi_ApplicationObject;

    /* ... private data follows ... */
};

/*
 same size as publicly documented
struct MUI_NotifyData
{
	struct MUI_GlobalInfo *mnd_GlobalInfo;
	ULONG                  mnd_UserData;
	ULONG                  mnd_ObjectID; 
	ULONG priv1;
	ULONG priv2;
	ULONG priv3;
	ULONG priv4;
};
*/

struct MUI_NotifyData
{
    struct MUI_GlobalInfo *mnd_GlobalInfo;
    ULONG                  mnd_UserData;
    STRPTR                 mnd_ObjectID;
/* private starts here */
    struct MinList        *mnd_NotifyList; 
    Object                *mnd_ParentObject;
    STRPTR                 mnd_HelpNode;
    LONG                   mnd_HelpLine_dummy;
};

#endif
