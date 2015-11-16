#include <zune/boopsi.h>
#include <zune/hooks.h>
#include <stdarg.h>

/*
 * Most code is from the AROS project, www.aros.org
 * Slightly adapted for Zune.
 */

#ifdef G_CAN_INLINE
G_INLINE_FUNC ULONG
CallHookPkt (struct Hook * hook, Object *object, APTR paramPacket)
{
    g_return_val_if_fail(hook != NULL, 0);
    g_return_val_if_fail(hook->h_Entry != NULL, 0);
    g_return_val_if_fail(object != NULL, 0);

    return ((*(hook->h_Entry)) (hook, object, paramPacket));
}
#else
static ULONG
CallHookPkt (struct Hook * hook, Object *object, APTR paramPacket)
{
    g_return_val_if_fail(hook != NULL, 0);
    g_return_val_if_fail(hook->h_Entry != NULL, 0);
    g_return_val_if_fail(object != NULL, 0);

    return ((*(hook->h_Entry)) (hook, object, paramPacket));
}
#endif

ULONG
DoMethodA (Object * obj, Msg message)
{
    g_return_val_if_fail(obj && message, 0);

    return (CallHookPkt ((struct Hook *)OCLASS(obj), obj, message));
} /* DoMethodA */


ULONG DoSuperMethodA (Class *cl, Object *obj, Msg message)
{
    g_return_val_if_fail(cl && obj && message, 0);
    return (CallHookPkt ((struct Hook *)cl->cl_Super, obj, message));
} /* DoSuperMethodA */


ULONG CoerceMethodA (Class *cl, Object *obj, Msg message)
{
    g_return_val_if_fail(cl != NULL, 0);
    g_return_val_if_fail(obj != NULL, 0);
    return (CallHookPkt ((struct Hook *)cl, obj, message));
}


void GetAttrsA (Object * object, struct TagItem * tags)
{
    struct TagItem * ti, * tstate;

    for (ti = tstate = tags; ti; ti = NextTagItem (&tstate))
    {
	GetAttr (ti->ti_Tag, object, (ULONG *) ti->ti_Data);
    }
} /* GetAttrsA */



ULONG xget(Object *obj, ULONG attr)
{
    ULONG ret;

    GetAttr(attr, obj, &ret);
    return(ret);
}


Object *NextObject(Object **ostate)
{
    APTR ret;

    g_return_val_if_fail(ostate, NULL);

    ret = GetSucc((struct Node *)*ostate) ?
	BASEOBJECT(*ostate) : NULL;

    if (ret)
    {
	*ostate = (Object *)GetSucc((struct Node *)*ostate);
    }
    return ret;
}

#ifndef __GNUC__

ULONG DoMethod (Object * obj, ULONG MethodID, ...)
{
    ULONG   retval;

    g_return_val_if_fail(obj && MethodID, 0);
    retval = (CallHookPkt ((struct Hook *)OCLASS(obj), obj, (Msg)&MethodID));
    return (retval);
} /* DoMethod */

void GetAttrs (Object * obj, ...)
{
    va_list	     args;

    va_start (args, obj);
    GetAttrsA (obj, (struct TagItem *)args);
    va_end (args);
} /* GetAttrs */

ULONG CoerceMethod (Class * cl, Object * obj, ULONG MethodID, ...)
{
    ULONG   retval;

    g_return_val_if_fail(cl != NULL, 0);
    g_return_val_if_fail(obj != NULL, 0);
    retval = CallHookPkt ((struct Hook *)cl, obj, (Msg)&MethodID);
    return (retval);
}


/*
 * Commodity function using varargs.
 * Called in constructors, it calls constructor of the super class,
 * thus ensuring linking of constructors up to the root class.
 * Usually, dont forget TAG_MORE, msg->ops_AttrList, at the end !
 */
ULONG
DoSuperNew(struct IClass *cl, Object *obj, ULONG tag1, ...)
{
    ULONG args[2];
    g_return_val_if_fail(cl != NULL, 0);
    g_return_val_if_fail(obj != NULL, 0);

    args[0] = OM_NEW;
    args[1] = (ULONG)&tag1;
    return(CallHookPkt ((struct Hook *)cl->cl_Super, obj, (Msg)args));
}


ULONG DoSuperMethod (Class * cl, Object * obj, ULONG MethodID, ...)
{
    ULONG   retval;

    g_return_val_if_fail(cl && obj && MethodID, 0);
    retval = CallHookPkt ((struct Hook *)cl->cl_Super, obj, (Msg)&MethodID);
    return (retval);
} /* DoSuperMethod */


#endif /* ! __GNUC__ */
