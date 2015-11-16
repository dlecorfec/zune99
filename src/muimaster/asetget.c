#include <zune/boopsi.h>
#include <stdarg.h>


ULONG 
GetAttr (ULONG attrID, Object *object, ULONG *storagePtr)
{
    struct opGet get;

    get.MethodID    = OM_GET;
    get.opg_AttrID  = attrID;
    get.opg_Storage = storagePtr;

    return (DoMethodA (object, (Msg)&get));
} /* GetAttr */


ULONG
SetAttrsA (Object *object, struct TagItem * tagList)
{
    struct opSet ops;

    ops.MethodID     = OM_SET;
    ops.ops_AttrList = tagList;

    return (DoMethodA (object, (Msg)&ops));
} /* SetAttrsA */


#ifndef __GNUC__
ULONG
SetAttrs (Object *object, ULONG tag1, ...)
{
    va_list args;
    ULONG   retval;

    va_start (args, tag1);

    retval = SetAttrsA (object, (struct TagItem *)&tag1);

    va_end (args);

    return (retval);
} /* SetAttrs */
#endif /* ! __GNUC__ */
