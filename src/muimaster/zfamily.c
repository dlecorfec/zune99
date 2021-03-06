/* Zune -- a free Magic User Interface implementation
 * Copyright (C) 1999 David Le Corfec
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include <zunepriv.h>
#include <builtin.h>

#include <Notify.h>
#include <Family.h>
#include <familydata.h>

/*
 * Family class is the base class for objects that are able
 * to handle a list of children. This is e.g. the case for
 * MUIs Menustrip, Menu and Menuitem objects.
 * Group class and application class should also be a
 * subclass of Family class, but due to BOOPSI system
 * limitations, this is currently impossible.
 */

/*
Family.mui/MUIA_Family_Child [I..]        done (note : == MUIA_Group_Child)
Family.mui/MUIA_Family_List [..G]         done

Family.mui/MUIM_Family_AddHead            done
Family.mui/MUIM_Family_AddTail            done
Family.mui/MUIM_Family_Insert             done
Family.mui/MUIM_Family_Remove             done
Family.mui/MUIM_Family_Sort               done
Family.mui/MUIM_Family_Transfer           done
Notify.mui/MUIM_FindUData                 done
Notify.mui/MUIM_GetUData                  done
Notify.mui/MUIM_SetUData                  done
Notify.mui/MUIM_SetUDataOnce              done
*/

static const int __version = 1;
static const int __revision = 1;

/*  static void */
/*  debuglist(struct List *list) */
/*  { */
/*      g_print("list %p:\nlh_Head@%p = %p\nlh_Tail@%p = %p\nlh_TailPred@%p = %p\n", */
/*  	    list, &list->lh_Head, list->lh_Head, */
/*  	    &list->lh_Tail, list->lh_Tail, */
/*  	    &list->lh_TailPred, list->lh_TailPred); */
/*  } */

/*  static void */
/*  printlist (struct List *list) */
/*  { */
/*      struct Node *node; */

/*      debuglist(list); */

/*      for (node = list->lh_Head; node->ln_Succ; node = node->ln_Succ) */
/*      { */
/*  	g_print("%s (ln_Succ@%p = %p | ln_Pred@%p = %p)\n", "node->ln_Name", */
/*  		&node->ln_Succ, node->ln_Succ, &node->ln_Pred, node->ln_Pred); */
/*      } */
/*      g_print("\n"); */
/*  } */

/*
 * OM_NEW
 */
static ULONG
mNew(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct MUI_FamilyData *data;
    struct TagItem        *tags;
    struct TagItem        *tag;
    BOOL   bad_childs = FALSE;

    obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg);
    if (!obj)
	return FALSE;
    /*
     * Initial local instance data
     */
    data = INST_DATA(cl, obj);
    InitList(&(data->childs));

    /*
     * parse initial taglist
     */
    for (tags = msg->ops_AttrList; (tag = NextTagItem(&tags)); )
    {
	if (tag->ti_Tag == MUIA_Family_Child)
	{
	    if (tag->ti_Data) /* add child */
		DoMethod(obj, MUIM_Family_AddTail, tag->ti_Data);
	    else /* fail and dispose childs */
	    {
		bad_childs = TRUE;
	    }
	}
    }
	
    if (bad_childs)
    {
	CoerceMethod(cl, obj, OM_DISPOSE);
	return 0;
    }

    return (ULONG)obj;
}


/*
 * OM_DISPOSE
 */
static ULONG
mDispose(struct IClass *cl, Object *obj, Msg msg)
{
    struct MUI_FamilyData *data = INST_DATA(cl, obj);
    Object                *cstate = (Object *)data->childs.lh_Head;
    Object                *child;

    while ((child = NextObject(&cstate)))
    {
/*  g_print("Family_Dispose: dispose child %p\n", child); */
	MUI_DisposeObject(child);
    }

    return DoSuperMethodA(cl, obj, msg);
}


/*
 * OM_GET
 */
static ULONG
mGet(struct IClass *cl, Object *obj, struct opGet *msg)
{
    struct MUI_FamilyData *data = INST_DATA(cl, obj);
    ULONG *store = msg->opg_Storage;

    switch(msg->opg_AttrID)
    {
    case MUIA_Family_List:
	*store = (ULONG)&data->childs;
	return(TRUE);
    case MUIA_Version:
	*store = __version;
	return(TRUE);
    case MUIA_Revision:
	*store = __revision;
	return(TRUE);
    }

    return(DoSuperMethodA(cl, obj, (Msg) msg));
}


/*
 * MUIM_Family_AddHead : Add an object as first object to the family.
 */
static ULONG
mAddHead(struct IClass *cl, Object *obj, struct MUIP_Family_AddHead *msg)
{
    struct MUI_FamilyData *data = INST_DATA(cl, obj);

    if (msg->obj)
    {
	AddHead(&(data->childs), (struct Node *)_OBJECT(msg->obj));
/*  g_print("Family_AddHead: added node %p in family %p\n", _OBJECT(msg->obj), obj); */
/*      printlist(&data->childs); */
	return TRUE;
    }
    else
	return FALSE;
}


/*
 * MUIM_Family_AddTail : Add an object as last object to the family.
 */
static ULONG
mAddTail(struct IClass *cl, Object *obj, struct MUIP_Family_AddTail *msg)
{
    struct MUI_FamilyData *data = INST_DATA(cl, obj);

    if (msg->obj)
    {
	AddTail(&(data->childs), (struct Node *)_OBJECT(msg->obj));
/*  g_print("Family_AddTail: added node %p (%s) in family %p\n", _OBJECT(msg->obj), */
/*  	OCLASS(msg->obj)->cl_ID, obj); */
/*      printlist(&data->childs); */
	return TRUE;
    }
    else
	return FALSE;
}


/*
 * MUIM_Family_Insert : Add an object after another object to the family.
 */
static ULONG
mInsert(struct IClass *cl, Object *obj, struct MUIP_Family_Insert *msg)
{
    struct MUI_FamilyData *data = INST_DATA(cl, obj);

    if (msg->obj)
    {
	Insert(&(data->childs), (struct Node *)_OBJECT(msg->obj),
	       (struct Node *)_OBJECT(msg->pred));
/*  g_print("Family_Insert: added node %p in family %p\n", _OBJECT(msg->obj), obj); */
/*      printlist(&data->childs); */
	return TRUE;
    }
    else
	return FALSE;
}


/*
 * MUIM_Family_Remove : Remove an object from a family.
 */
static ULONG
mRemove(struct IClass *cl, Object *obj,
			   struct MUIP_Family_Remove *msg)
{
    struct MUI_FamilyData *data = INST_DATA(cl, obj);
    struct Node *node;

    if (msg->obj)
    {
/*      g_print("removing obj node %p\n", _OBJECT(msg->obj)); */

/*      printlist(&data->childs); */
	Remove((struct Node *)_OBJECT(msg->obj));
/*      g_print("apres remove:\n"); */
/*      printlist(&data->childs); */
	return TRUE;
    }
    else
	return FALSE;
}


/*
 * MUIM_Family_Sort : Sort the children of a family.
 */
static ULONG 
mSort(struct IClass *cl, Object *obj,
			 struct MUIP_Family_Sort *msg)
{
    struct MUI_FamilyData *data = INST_DATA(cl, obj);
    SLONG  i;

    InitList(&(data->childs));
    for (i = 0 ; msg->obj[i] ; i++)
    {
	AddTail(&(data->childs), (struct Node *)_OBJECT(msg->obj[i]));
    }
    return TRUE;
}


/*
 * MUIM_Family_Transfer : All the children of the family are removed and
 * added to another family in the same order.
 */
static ULONG
mTransfer(struct IClass *cl, Object *obj, struct MUIP_Family_Transfer *msg)
{
    struct MUI_FamilyData *data = INST_DATA(cl, obj);
    Object                *cstate = (Object *)data->childs.lh_Head;
    Object                *child;

    while ((child = NextObject(&cstate)))
    {
	DoMethod(obj, MUIM_Family_Remove, _U(child));
	DoMethod(msg->family, MUIM_Family_AddTail, _U(child));
    }
    return TRUE;
}


/*
 * MUIM_FindUData : tests if the MUIA_UserData of the object
 * contains the given <udata> and returns the object pointer in this case.
 */
static ULONG
mFindUData(struct IClass *cl, Object *obj, struct MUIP_FindUData *msg)
{
    struct MUI_FamilyData *data = INST_DATA(cl, obj);
    Object                *cstate = (Object *)data->childs.lh_Head;
    Object                *child;

    if (muiNotifyData(obj)->mnd_UserData == msg->udata)
	return (ULONG)obj;
    while ((child = NextObject(&cstate)))
       if (DoMethodA(child, (Msg)msg))
	   return (ULONG)child;

    return 0L;
}


/*
 * MUIM_GetUData : This method tests if the MUIA_UserData of the object
 * contains the given <udata> and gets <attr> to <storage> for itself
 * in this case.
 */
static ULONG
mGetUData(struct IClass *cl, Object *obj, struct MUIP_GetUData *msg)
{
    struct MUI_FamilyData *data = INST_DATA(cl, obj);
    Object                *cstate = (Object *)data->childs.lh_Head;
    Object                *child;

    if (muiNotifyData(obj)->mnd_UserData == msg->udata)
    {
	get(obj, msg->attr, msg->storage);
	return TRUE;
    }
    while ((child = NextObject(&cstate)))
       if (DoMethodA(child, (Msg)msg))
	   return TRUE;

    return FALSE;
}


/*
 * MUIM_SetUData : This method tests if the MUIA_UserData of the object
 * contains the given <udata> and sets <attr> to <val> for itself in this case.
 */
static ULONG 
mSetUData(struct IClass *cl, Object *obj, struct MUIP_SetUData *msg)
{
    struct MUI_FamilyData *data = INST_DATA(cl, obj);
    Object                *cstate = (Object *)data->childs.lh_Head;
    Object                *child;

    if (muiNotifyData(obj)->mnd_UserData == msg->udata)
	set(obj, msg->attr, msg->val);

    while ((child = NextObject(&cstate)))
       DoMethodA(child, (Msg)msg);

    return TRUE;
}


/*
 * MUIM_SetUDataOnce : This method tests if the MUIA_UserData of the object
 * contains the given <udata> and sets <attr> to <val> for itself in this case.
 */
static ULONG 
mSetUDataOnce(struct IClass *cl, Object *obj, struct MUIP_SetUDataOnce *msg)
{
    struct MUI_FamilyData *data = INST_DATA(cl, obj);
    Object                *cstate = (Object *)data->childs.lh_Head;
    Object                *child;

    if (muiNotifyData(obj)->mnd_UserData == msg->udata)
    {
	set(obj, msg->attr, msg->val);
	return TRUE;
    }
    while ((child = NextObject(&cstate)))
       if (DoMethodA(child, (Msg)msg))
	   return TRUE;

    return FALSE;
}


/*
 * The class dispatcher
 */
static ULONG
MUI_FamilyDispatcher (struct IClass *cl, Object *obj, Msg msg)
{
    /*
     * watch out for methods we do understand
     */
    switch (msg->MethodID)
    {
	/* Whenever an object shall be created using NewObject(), it will be
	** sent a OM_NEW method.
	*/
    case OM_NEW:
	return(mNew(cl, obj, (struct opSet *) msg));
    case OM_DISPOSE:
	return(mDispose(cl, obj, msg));
    case OM_GET:
	return(mGet(cl, obj, (struct opGet *)msg));
    case MUIM_Family_AddHead :
	return(mAddHead(cl, obj, (APTR)msg));
    case OM_ADDMEMBER:
    case MUIM_Family_AddTail :
	return(mAddTail(cl, obj, (APTR)msg));
    case MUIM_Family_Insert :
	return(mInsert(cl, obj, (APTR)msg));
    case OM_REMMEMBER:
    case MUIM_Family_Remove :
	return(mRemove(cl, obj, (APTR)msg));
    case MUIM_Family_Sort :
	return(mSort(cl, obj, (APTR)msg));
    case MUIM_Family_Transfer :
	return(mTransfer(cl, obj, (APTR)msg));
    case MUIM_FindUData :
	return(mFindUData(cl, obj, (APTR)msg));
    case MUIM_GetUData :
	return(mGetUData(cl, obj, (APTR)msg));
    case MUIM_SetUData :
	return(mSetUData(cl, obj, (APTR)msg));
    case MUIM_SetUDataOnce :
	return(mSetUDataOnce(cl, obj, (APTR)msg));
    }

    /*
     * we didn't understand the last method, so call our superclass
     */
    return(DoSuperMethodA(cl, obj, msg));
}


/*
 * Class descriptor.
 */
const struct __MUIBuiltinClass _MUI_Family_desc = {
    MUIC_Family,
    MUIC_Notify,
    sizeof(struct MUI_FamilyData),
    MUI_FamilyDispatcher
};
