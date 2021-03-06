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
#include <renderinfo.h>
#include <zunepriv.h>
#include <builtin.h>
#include <prefs.h>

#include <Notify.h>
#include <Family.h>
#include <familydata.h>
#include <Dataspace.h>
#include <Group.h>
#include <groupdata.h>
#include <Window.h>
#include <windowdata.h>
#include <gdk/gdk.h>

/*
 * Attributes and methods, all done !
*/

#define ROUND(x) ((int)(x + 0.5))

static const int __version = 1;
static const int __revision = 1;

/******************************************************************************/
/******************************************************************************/

struct MUIP_Group_ADDMEMBER {
    ULONG    MethodID;
    Object   *obj;
};

struct MUIP_Group_REMMEMBER {
    ULONG    MethodID;
    Object   *obj;
};

struct layout2d_elem {
    int min;
    int max;
    int dim;
    int weight;
};

static ULONG
Group_DispatchMsg(struct IClass *cl, Object *obj, Msg msg);

static void
change_active_page (struct IClass *cl, Object *obj, LONG page)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    LONG newpage;

    switch (page)
    {
	case MUIV_Group_ActivePage_First:
	    newpage = 0;
	    break;
        case MUIV_Group_ActivePage_Last:
	    newpage = data->num_childs - 1;
	    break;
        case MUIV_Group_ActivePage_Prev:
	    newpage = data->active_page - 1;
	    if (newpage == -1)
		newpage = data->num_childs - 1;
	    break;
        case MUIV_Group_ActivePage_Next:
        case MUIV_Group_ActivePage_Advance:
	    newpage = (data->active_page + 1) % data->num_childs;	    
	    break;
	default:
	    newpage = page;
	    break;
    }

    if (newpage != data->active_page)
    {
	data->active_page = newpage;
	MUI_Redraw(obj, MADF_DRAWOBJECT);
    }
}


/*
 * Constructor
 */
static ULONG
mNew(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct MUI_GroupData *data;
    struct TagItem *tags,*tag;
    BOOL   bad_childs = FALSE;

    obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg);
    if (!obj)
	return 0;

    /* Initial local instance data */
    data = INST_DATA(cl, obj);

    data->family = MUI_NewObjectA(MUIC_Family, NULL);
    if (!data->family)
	return 0;

    data->horiz_spacing = __zprefs.group_hspacing;
    data->vert_spacing = __zprefs.group_vspacing;
    data->columns = 1;
    data->rows = 1;

    /* parse initial taglist */
    for (tags = msg->ops_AttrList; (tag = NextTagItem(&tags)); )
    {
	switch (tag->ti_Tag)
	{
	case MUIA_Group_Child:
	    if (tag->ti_Data)
	    {
		DoMethod(data->family, MUIM_Family_AddTail, tag->ti_Data);
		data->num_childs++;
	    }
	    else
	    {
		bad_childs = TRUE;
	    }
	    break;
	case MUIA_Group_ActivePage:
	    change_active_page(cl, obj, (LONG)tag->ti_Data);
	    break;
	case MUIA_Group_Columns:
	    data->columns = MAX((ULONG)tag->ti_Data, 1);
	    data->rows = 0;
	    break;
	case MUIA_Group_Horiz:
	    _handle_bool_tag(data->flags, tag->ti_Data, GROUP_HORIZ);
	    break;
	case MUIA_Group_HorizSpacing:
	    data->horiz_spacing = tag->ti_Data;
	    break;
	case MUIA_Group_LayoutHook:
	    data->layout_hook = (struct Hook *)tag->ti_Data;
	    break;
	case MUIA_Group_PageMode:
	    _handle_bool_tag(data->flags, tag->ti_Data, GROUP_PAGEMODE);
	    break;
	case MUIA_Group_Rows:
	    data->rows = MAX((ULONG)tag->ti_Data, 1);
	    data->columns = 0;
	    break;
	case MUIA_Group_SameHeight:
	    _handle_bool_tag(data->flags, tag->ti_Data, GROUP_SAME_HEIGHT);
	    break;
	case MUIA_Group_SameSize:
	    _handle_bool_tag(data->flags, tag->ti_Data, GROUP_SAME_HEIGHT);
	    _handle_bool_tag(data->flags, tag->ti_Data, GROUP_SAME_WIDTH);
	    break;
	case MUIA_Group_SameWidth:
	    _handle_bool_tag(data->flags, tag->ti_Data, GROUP_SAME_WIDTH);
	    break;
	case MUIA_Group_Spacing:
	    data->horiz_spacing = tag->ti_Data;
	    data->vert_spacing = tag->ti_Data;
	    break;
	case MUIA_Group_VertSpacing:
	    data->vert_spacing = tag->ti_Data;
	    break;
	}
    }

    if (bad_childs)
    {
	CoerceMethod(cl, obj, OM_DISPOSE);
	return 0;
    }

    return (ULONG)obj;
}

/******************************************************************************/
/* DISPOSE                                                                    */
/******************************************************************************/

static ULONG
mDispose(struct IClass *cl, Object *obj, Msg msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);

    MUI_DisposeObject(data->family);
    return DoSuperMethodA(cl, obj, msg);
}

/*
 * OM_SET
 */
static ULONG
mSet(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct MUI_GroupData *data  = INST_DATA(cl, obj);
    struct TagItem       *tags  = msg->ops_AttrList;
    struct TagItem       *tag;
    BOOL forward = TRUE;

/* There are many ways to find out what tag items provided by set()
** we do know. The best way should be using NextTagItem() and simply
** browsing through the list.
*/
    while ((tag = NextTagItem(&tags)) != NULL)
    {
	switch (tag->ti_Tag)
	{
	    case MUIA_Group_Columns:
		data->columns = MAX((ULONG)tag->ti_Data, 1);
		data->rows = 0;
		DoMethod(_win(obj), MUIM_Window_RecalcDisplay);
		break;
	    case MUIA_Group_ActivePage:
		change_active_page(cl, obj, (LONG)tag->ti_Data);
		break;
	    case MUIA_Group_Forward:
		forward = FALSE;
		break;
	    case MUIA_Group_HorizSpacing:
		data->horiz_spacing = tag->ti_Data;
		break;
	    case MUIA_Group_Rows:
		data->rows = MAX((ULONG)tag->ti_Data, 1);
		data->columns = 0;
		DoMethod(_win(obj), MUIM_Window_RecalcDisplay);
		break;
	    case MUIA_Group_Spacing:
		data->horiz_spacing = tag->ti_Data;
		data->vert_spacing = tag->ti_Data;
		break;
	    case MUIA_Group_VertSpacing:
		data->vert_spacing = tag->ti_Data;
		break;
	}
    }

    /* seems to be the documented behaviour, however it should be slow! */
    if (forward)
	Group_DispatchMsg(cl, obj, (Msg)msg);

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

/******************************************************************************/
/* GET                                                                        */
/******************************************************************************/

static ULONG
mGet(struct IClass *cl, Object *obj, struct opGet *msg)
{
/* small macro to simplify return value storage */
#define STORE *(msg->opg_Storage)

    struct MUI_GroupData *data = INST_DATA(cl, obj);

    switch(msg->opg_AttrID)
    {
    case MUIA_Version:
	STORE = __version;
	return(TRUE);
    case MUIA_Revision:
	STORE = __revision;
	return(TRUE);

    case MUIA_Group_ActivePage:
	STORE = data->active_page;
	return(TRUE);
    case MUIA_Group_ChildList:
	STORE = xget(data->family, MUIA_Family_List);
	return(TRUE);
    case MUIA_Group_HorizSpacing:
	STORE = data->horiz_spacing;
	return(TRUE);
    case MUIA_Group_VertSpacing:
	STORE = data->vert_spacing;
	return(TRUE);
    }

    /* seems to be the documented behaviour, however it should be slow! */
    Group_DispatchMsg(cl, obj, (Msg)msg);

    /* our handler didn't understand the attribute, we simply pass
    ** it to our superclass now
    */
    return(DoSuperMethodA(cl, obj, (Msg) msg));
#undef STORE
}


/*
 * OM_ADDMEMBER
 */
static ULONG
mAddMember(struct IClass *cl, Object *obj,
		struct MUIP_Group_ADDMEMBER *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    ULONG show = MUIM_Show;

    DoMethodA(data->family, (Msg)msg);
    data->num_childs++;
    /* if we are in an application tree, propagate pointers */
    if (muiNotifyData(obj)->mnd_GlobalInfo)
	DoMethod(msg->obj, MUIM_ConnectParent, _U(obj));

    if (_flags(obj) & MADF_SETUP)
	DoMethod(msg->obj, MUIM_Setup, _U(muiRenderInfo(obj)));
    if (_flags(obj) & MADF_CANDRAW)
	DoMethodA(msg->obj, (Msg)&show);

    return TRUE;
}


static ULONG
mRemMember(struct IClass *cl, Object *obj,
		struct MUIP_Group_REMMEMBER *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    ULONG hide = MUIM_Hide;
    ULONG cleanup = MUIM_Cleanup;
    ULONG disconnect = MUIM_DisconnectParent;

    if (_flags(obj) & MADF_CANDRAW)
	DoMethodA(msg->obj, (Msg)&hide);
    if (_flags(obj) & MADF_SETUP)
	DoMethodA(msg->obj, (Msg)&cleanup);

    if (muiNotifyData(obj)->mnd_GlobalInfo)
	DoMethodA(msg->obj, (Msg)&disconnect);

    data->num_childs--;
    DoMethodA(data->family, (Msg)msg);

    return TRUE;
}


static void
group_connect_childs (struct MUI_GroupData *data, Object *obj)
{
    Object                *cstate;
    Object                *child;
    struct MinList        *ChildList;

    get(data->family, MUIA_Family_List, (ULONG *)&(ChildList));
    cstate = (Object *)ChildList->mlh_Head;
    while ((child = NextObject(&cstate)))
    {
	if (muiAreaData(child)->mad_Background == NULL)
	{
	    muiAreaData(child)->mad_Background =
		zune_imspec_copy(muiAreaData(obj)->mad_Background);
	}
	DoMethod(child, MUIM_ConnectParent, _U(obj));
    }
}

/*
 * called by parent object between OM_NEW and MUIM_Setup,
 * init RenderInfo and GlobalInfo
 */
static ULONG
mConnectParent(struct IClass *cl, Object *obj,
		    struct MUIP_ConnectParent *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    Object                *parent = msg->parent;

    g_assert(parent != NULL);
    g_assert(muiAreaData(parent)->mad_RenderInfo != NULL);
    g_assert(muiNotifyData(parent)->mnd_GlobalInfo != NULL);

    _parent(obj) = parent;
    muiAreaData(obj)->mad_RenderInfo = muiAreaData(parent)->mad_RenderInfo;
    muiNotifyData(obj)->mnd_GlobalInfo = muiNotifyData(parent)->mnd_GlobalInfo;

    group_connect_childs(data, obj);
    return TRUE;
}


/*
 * called by parent window object between OM_NEW and MUIM_Setup,
 * init RenderInfo and GlobalInfo
 * no muiAreaData(parent) here !!!
 */
static ULONG
mConnectParentWindow(struct IClass *cl, Object *obj,
		    struct MUIP_ConnectParentWindow *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);

    muiAreaData(obj)->mad_RenderInfo = msg->mri;
    muiNotifyData(obj)->mnd_GlobalInfo = muiNotifyData(msg->win)->mnd_GlobalInfo;
    _parent(obj) = NULL;

    group_connect_childs(data, obj);
    return TRUE;
}

/*
 * called by parent object
 */
static ULONG
mDisconnectParent(struct IClass *cl, Object *obj,
		       struct MUIP_DisconnectParent *msg)
{
    static ULONG          method = MUIM_DisconnectParent;
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    Object               *cstate;
    Object               *child;
    struct MinList       *ChildList;

    get(data->family, MUIA_Family_List, (ULONG *)&(ChildList));
    cstate = (Object *)ChildList->mlh_Head;
    while ((child = NextObject(&cstate)))
	DoMethodA(child, (Msg)&method);

    muiNotifyData(obj)->mnd_GlobalInfo = NULL;
    muiAreaData(obj)->mad_RenderInfo = NULL;
    _parent(obj) = NULL;
    return TRUE;
}


/*
 * Put group in exchange state
 */
static ULONG
mInitChange(struct IClass *cl, Object *obj,
		 struct MUIP_Group_InitChange *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);

    data->flags |= GROUP_CHANGING;
    return TRUE;
}


/*
 * Will recalculate display after dynamic adding/removing
 */
static ULONG
mExitChange(struct IClass *cl, Object *obj,
		 struct MUIP_Group_ExitChange *msg)
{
    static ULONG method = MUIM_Window_RecalcDisplay;
    struct MUI_GroupData *data = INST_DATA(cl, obj);

    if (data->flags & GROUP_CHANGING)
    {
	data->flags &= ~GROUP_CHANGING;

/* FIXME: this needs optimization !!! */
	/* as a last resort only */
	DoMethodA(_win(obj), (Msg)&method);
    }

    return TRUE;
}


/*
 * Sort the family
 */
static ULONG
mSort(struct IClass *cl, Object *obj, struct MUIP_Group_Sort *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);

    /* modify message */
    msg->MethodID = MUIM_Family_Sort;

    DoMethodA(data->family, (APTR)msg);

    /* restore original message */
    msg->MethodID = MUIM_Group_Sort;
    return TRUE;
}


/*
 * Propagate a method to group childs.
 */
static ULONG
Group_DispatchMsg(struct IClass *cl, Object *obj, Msg msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    Object               *cstate;
    Object               *child;
    struct MinList       *ChildList;

    get(data->family, MUIA_Family_List, (ULONG *)&(ChildList));
    cstate = (Object *)ChildList->mlh_Head;
    while ((child = NextObject(&cstate)))
	DoMethodA(child, (Msg)msg);
    return TRUE;
}


/*
 * MUIM_Setup
 */
static ULONG
mSetup(struct IClass *cl, Object *obj, Msg msg)
{
    static ULONG          method = MUIM_Cleanup;
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    Object               *cstate;
    Object               *cstate_copy;
    Object               *child;
    Object               *childFailed;
    struct MinList       *ChildList;

    if (!DoSuperMethodA(cl, obj, (Msg)msg))
	return FALSE;

    get(data->family, MUIA_Family_List, (ULONG *)&(ChildList));
    cstate = cstate_copy = (Object *)ChildList->mlh_Head;
    while ((child = NextObject(&cstate)))
    {
	if (! (_flags(child) & MADF_SHOWME))
	    continue;

	if (!DoMethodA(child, (Msg)msg))
	{
	    /* Send MUIM_Cleanup to all objects that received MUIM_Setup.
	     */
	    childFailed = child;
	    cstate = cstate_copy;
	    while ((child = NextObject(&cstate)) && (child != childFailed))
	    {
		if (! (_flags(child) & MADF_SHOWME))
		    continue;
		DoMethodA(child, (Msg)&method);
	    }
	    return FALSE;
	}
    }
    return TRUE;
}


/*
 * MUIM_Cleanup
 */
static ULONG
mCleanup(struct IClass *cl, Object *obj, Msg msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    Object               *cstate;
    Object               *child;
    struct MinList       *ChildList;

    get(data->family, MUIA_Family_List, (ULONG *)&(ChildList));
    cstate = (Object *)ChildList->mlh_Head;
    while ((child = NextObject(&cstate)))
    {
	if (! (_flags(child) & MADF_SHOWME))
	    continue;
	DoMethodA(child, (Msg)msg);
    }
    return DoSuperMethodA(cl, obj, (Msg)msg);
}


/*
 * Draw group frame
 */
static ULONG
mDraw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    Object                *cstate;
    Object                *child;
    struct MinList        *ChildList;
    GdkRectangle           group_rect, child_rect;
    int                    page = -1;

    DoSuperMethodA(cl, obj, (Msg)msg);
    if (!(msg->flags & MADF_DRAWOBJECT))
	return TRUE;

    /*
     * update is set when changing active page of a page group
     * need to redraw background ourself
     */
/*      if (msg->flags & MADF_DRAWUPDATE) */
/*      { */
/*  	DoMethod(obj, MUIM_DrawBackground, _mleft(obj), _mtop(obj), */
/*  		 _mwidth(obj), _mheight(obj), _left(obj), _top(obj), 0); */
/*      } */


    group_rect = muiRenderInfo(obj)->mri_ClipRect;
    get(data->family, MUIA_Family_List, (ULONG *)&(ChildList));
    cstate = (Object *)ChildList->mlh_Head;
    while ((child = NextObject(&cstate))) {
	if (! (_flags(child) & MADF_SHOWME))
	    continue;
	++page;
	if ((data->flags & GROUP_PAGEMODE) && (page != data->active_page))
	{
	    continue;
	}
	
	msg->flags |= MADF_DRAWOBJECT; /* yup, do not forget */

	child_rect.x = _left(child);
	child_rect.y = _top(child);
	child_rect.width = _width(child);
	child_rect.height = _height(child);
/*  	    g_print("intersect: a=(%d, %d, %d, %d) b=(%d, %d, %d, %d)\n", */
/*  		    group_rect.x, group_rect.y, */
/*  		    group_rect.width, group_rect.height, */
/*  		    child_rect.x, child_rect.y, */
/*  		    child_rect.width, child_rect.height); */
	if (gdk_rectangle_intersect(&group_rect, &child_rect,
				    &muiRenderInfo(obj)->mri_ClipRect))
	    DoMethodA(child, (Msg)msg);

	muiRenderInfo(obj)->mri_ClipRect = group_rect;
/*  	    g_print("set back clip to (%d, %d, %d, %d)\n", */
/*  		    group_rect.x, group_rect.y, group_rect.width, group_rect.height); */
    }
    return TRUE;
}


#define END_MINMAX() \
    tmp.MaxHeight = MAX(tmp.MaxHeight, tmp.MinHeight); \
    tmp.MaxWidth = MAX(tmp.MaxWidth, tmp.MinWidth); \
    msg->MinMaxInfo->MinWidth += tmp.MinWidth; \
    msg->MinMaxInfo->MinHeight += tmp.MinHeight; \
    msg->MinMaxInfo->MaxWidth += tmp.MaxWidth; \
    msg->MinMaxInfo->MaxWidth = MIN(msg->MinMaxInfo->MaxWidth, MUI_MAXMAX); \
    msg->MinMaxInfo->MaxHeight += tmp.MaxHeight; \
    msg->MinMaxInfo->MaxHeight = MIN(msg->MinMaxInfo->MaxHeight, MUI_MAXMAX); \
    msg->MinMaxInfo->DefWidth += tmp.MinWidth; \
    msg->MinMaxInfo->DefHeight += tmp.MinHeight;

/*
 * MinMax calculation function. When this is called,
 * the children of your group have already been asked
 * about their min/max dimension so you can use their
 * dimensions to calculate yours.
 *
 * Notes:
 * - Init minwidth and maxwidth with size needed for total child spacing.
 * - 1st pass to find maximum minimum width, to set minwidth of each child
 * if they should have the same width (for a row of buttons ...)
 * - Adjust minwidth w/o making object bigger than their max size.
 */
static void
group_minmax_horiz(struct IClass *cl, Object *obj,
		   struct MinList *children, struct MUIP_AskMinMax *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    Object *cstate;
    Object *child;
    struct MUI_MinMax tmp;
    WORD maxminwidth = 0;

    tmp.MinHeight = 0;
    tmp.MaxHeight = MUI_MAXMAX;
    tmp.MinWidth = tmp.MaxWidth = (data->num_childs - 1) * data->horiz_spacing;

    if (data->flags & GROUP_SAME_WIDTH) {
	cstate = (Object *)children->mlh_Head;
	while ((child = NextObject(&cstate))) {
	    if (! (_flags(child) & MADF_SHOWME))
		continue;
	    maxminwidth = MAX(maxminwidth, _minwidth(child));
	}
    }
    cstate = (Object *)children->mlh_Head;
    while ((child = NextObject(&cstate))) {
	if (! (_flags(child) & MADF_SHOWME))
	    continue;
	if (data->flags & GROUP_SAME_WIDTH)
	    _minwidth(child) = MIN(maxminwidth, _maxwidth(child));
	tmp.MinWidth += _minwidth(child);
	tmp.MaxWidth += _maxwidth(child);
	tmp.MaxWidth = MIN(tmp.MaxWidth, MUI_MAXMAX);
	tmp.MinHeight = MAX(tmp.MinHeight, _minheight(child));
	tmp.MaxHeight = MIN(tmp.MaxHeight, _maxheight(child));
    }
    END_MINMAX();
}

/* minmax calculation for vertical groups (see group_minmax_horiz)
*/
static void
group_minmax_vert(struct IClass *cl, Object *obj,
		  struct MinList *children, struct MUIP_AskMinMax *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    Object *cstate;
    Object *child;
    struct MUI_MinMax tmp;
    WORD maxminheight = 0;

    tmp.MinWidth = 0;
    tmp.MaxWidth = MUI_MAXMAX;
    tmp.MinHeight = tmp.MaxHeight =  (data->num_childs - 1) * data->vert_spacing;

    if (data->flags & GROUP_SAME_HEIGHT) {
	cstate = (Object *)children->mlh_Head;
	while ((child = NextObject(&cstate))) {
	    if (! (_flags(child) & MADF_SHOWME))
		continue;
	    maxminheight = MAX(maxminheight, _minheight(child));
	}
    }
    cstate = (Object *)children->mlh_Head;
    while ((child = NextObject(&cstate))) {
	if (! (_flags(child) & MADF_SHOWME))
	    continue;
	if (data->flags & GROUP_SAME_HEIGHT)
	    _minheight(child) = MIN(maxminheight, _maxheight(child));
	tmp.MinHeight += _minheight(child);
	tmp.MaxHeight += _maxheight(child);
	tmp.MaxHeight = MIN(tmp.MaxHeight, MUI_MAXMAX);
	tmp.MinWidth = MAX(tmp.MinWidth, _minwidth(child));
	tmp.MaxWidth = MIN(tmp.MaxWidth, _maxwidth(child));
    }
    END_MINMAX();
}


static void
minmax_2d_rows_pass (struct MUI_GroupData *data, struct MinList *children,
		     struct MUI_MinMax *req, const WORD maxmin_height)
{
    int i, j;
    Object *cstate;
    Object *child;

    /* do not rewind after the while, to process line by line */
    cstate = (Object *)children->mlh_Head;
    /* for each row */
    for (i = 0; i < data->rows; i++) {
	/* calculate min and max height of this row */
	int min_h = 0, max_h = MUI_MAXMAX;
	j = 0;
	while ((child = NextObject(&cstate))) {
	    if (! (_flags(child) & MADF_SHOWME))
		continue;
	    if (data->flags & GROUP_SAME_HEIGHT)
		_minheight(child) = MIN(maxmin_height, _maxheight(child));
	    min_h = MAX(min_h, _minheight(child));
	    max_h = MIN(max_h, _maxheight(child));
	    ++j;
	    if ((j % data->columns) == 0)
		break;
	}
	max_h = MAX(max_h, min_h);
/*  	g_print("row %d : min_h=%d max_h=%d\n", i, min_h, max_h); */
	req->MinHeight += min_h;
	req->MaxHeight += max_h;
    }
}


static void
minmax_2d_columns_pass (struct MUI_GroupData *data, struct MinList *children,
			struct MUI_MinMax *req, const WORD maxmin_width)
{
    int i, j;
    Object *cstate;
    Object *child;

    for (i = 0; i < data->columns; i++) {
	/* calculate min and max width of this column */
	int min_w = 0, max_w = MUI_MAXMAX;
	j = 0;
	/* process all childs to get childs on a column */
	cstate = (Object *)children->mlh_Head;
	while ((child = NextObject(&cstate))) {
	    if (! (_flags(child) & MADF_SHOWME))
		continue;
	    ++j;
	    if (((j - 1) % data->columns) != i)
		continue;
	    if (data->flags & GROUP_SAME_WIDTH)
		_minwidth(child) = MIN(maxmin_width, _maxwidth(child));
	    min_w = MAX(min_w, _minwidth(child));
	    max_w = MIN(max_w, _maxwidth(child));
	}
	max_w = MAX(max_w, min_w);
/*  	g_print("col %d : min_w=%d max_w=%d\n", i, min_w, max_w); */
	req->MinWidth += min_w;
	req->MaxWidth += max_w;
    }
}

static void
group_minmax_2d(struct IClass *cl, Object *obj,
		struct MinList *children, struct MUIP_AskMinMax *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    Object *cstate;
    Object *child;
    struct MUI_MinMax tmp;
    WORD maxmin_width;
    WORD maxmin_height;

    if (!data->columns)
    {
	g_return_if_fail((data->num_childs % data->rows) == 0);
	data->columns = data->num_childs / data->rows;
/*  	g_print("child=%ld rows=%ld new cols=%ld\n", */
/*  		data->num_childs, data->rows, data->columns); */
    }
    else
    {
	g_return_if_fail((data->num_childs % data->columns) == 0);
	data->rows = data->num_childs / data->columns;
/*  	g_print("new rows=%ld\n", data->rows); */
    }
    tmp.MinHeight = tmp.MaxHeight = (data->rows - 1) * data->vert_spacing;
    tmp.MinWidth = tmp.MaxWidth = (data->columns - 1) * data->horiz_spacing;
    /* get minimum dims if same dims for all childs are needed */
    maxmin_width = 0;
    maxmin_height = 0;

    if ((data->flags & GROUP_SAME_WIDTH) || (data->flags & GROUP_SAME_HEIGHT)) {
	cstate = (Object *)children->mlh_Head;
	while ((child = NextObject(&cstate))) {
	    if (! (_flags(child) & MADF_SHOWME))
		continue;
	    maxmin_width = MAX(maxmin_width, _minwidth(child));
	    maxmin_height = MAX(maxmin_height, _minheight(child));
	}
/*  	g_print("2d group: mminw=%d mminh=%d\n", maxmin_width, maxmin_height); */
    }
    minmax_2d_rows_pass (data, children, &tmp, maxmin_height);
    minmax_2d_columns_pass (data, children, &tmp, maxmin_width);
    END_MINMAX();
}


static void
group_minmax_pagemode(struct IClass *cl, Object *obj,
		      struct MinList *children, struct MUIP_AskMinMax *msg)
{
    Object *cstate;
    Object *child;
    struct MUI_MinMax tmp = { 0, 0, MUI_MAXMAX, MUI_MAXMAX, 0, 0 };
    
    cstate = (Object *)children->mlh_Head;
    while ((child = NextObject(&cstate)))
    {
	if (! (_flags(child) & MADF_SHOWME))
	    continue;

	tmp.MinHeight = MAX(tmp.MinHeight, _minheight(child));
	tmp.MinWidth = MAX(tmp.MinWidth, _minwidth(child));
	tmp.MaxHeight = MAX(tmp.MaxHeight, _maxheight(child));
	tmp.MaxWidth = MAX(tmp.MaxWidth, _maxwidth(child));
    }
    END_MINMAX();
}

/*
 * MUIM_AskMinMax : ask childs about min/max sizes, then
 * either call a hook, or the builtin method, to calculate our minmax
 */
static ULONG
mAskMinMax(struct IClass *cl, Object *obj,
		struct MUIP_AskMinMax *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    struct MUI_LayoutMsg  lm;
    struct MUIP_AskMinMax childMsg;
    struct MUI_MinMax childMinMax;
    Object *cstate;
    Object *child;

    /*
     * let our superclass first fill in its size with frame, inner spc etc ...
     */
    DoSuperMethodA(cl, obj, (Msg)msg);

    /*
     * Ask children
     */
    childMsg.MethodID = msg->MethodID;
    childMsg.MinMaxInfo = &childMinMax;
    lm.lm_Type = MUILM_MINMAX;
    get(data->family, MUIA_Family_List, (ULONG *)&(lm.lm_Children));

    cstate = (Object *)lm.lm_Children->mlh_Head;

    while ((child = NextObject(&cstate)))
    {
	if (! (_flags(child) & MADF_SHOWME))
	    continue;
	/*  Ask child  */
	DoMethodA(child, (Msg)&childMsg);

	__area_finish_minmax(child, childMsg.MinMaxInfo);

/*  g_print("child minmax (%p/%p): Min=%dx%d Max=%dx%d Def=%dx%d\n", */
/*  	child, obj, */
/*  	_minwidth(child), _minheight(child), */
/*  	_maxwidth(child), _maxheight(child), */
/*  	_defwidth(child), _defheight(child)); */
    }

    /*
     * Use childs infos to calculate group size
     */
    if (data->flags & GROUP_PAGEMODE)
    {
	group_minmax_pagemode(cl, obj, lm.lm_Children, msg);
    }
    else if (data->layout_hook)
    {
	(*(data->layout_hook->h_Entry))(data->layout_hook, obj, &lm);

	if (lm.lm_MinMax.MaxHeight < lm.lm_MinMax.MinHeight)
	    lm.lm_MinMax.MaxHeight = lm.lm_MinMax.MinHeight;
	if (lm.lm_MinMax.MaxWidth < lm.lm_MinMax.MinWidth)
	    lm.lm_MinMax.MaxWidth = lm.lm_MinMax.MinWidth;

	msg->MinMaxInfo->MinWidth += lm.lm_MinMax.MinWidth;
	msg->MinMaxInfo->MinHeight += lm.lm_MinMax.MinHeight;
	msg->MinMaxInfo->MaxWidth += lm.lm_MinMax.MaxWidth;
	if (msg->MinMaxInfo->MaxWidth > MUI_MAXMAX)
	    msg->MinMaxInfo->MaxWidth = MUI_MAXMAX;
	msg->MinMaxInfo->MaxHeight += lm.lm_MinMax.MaxHeight;
	if (msg->MinMaxInfo->MaxHeight > MUI_MAXMAX)
	    msg->MinMaxInfo->MaxHeight = MUI_MAXMAX;
	msg->MinMaxInfo->DefWidth += lm.lm_MinMax.DefWidth;
	msg->MinMaxInfo->DefHeight += lm.lm_MinMax.DefHeight;
    }
    else
    {
	if ((data->rows == 1) && (data->columns == 1))
	{
	    if (data->flags & GROUP_HORIZ)
		group_minmax_horiz(cl, obj, lm.lm_Children, msg);
	    else
		group_minmax_vert(cl, obj, lm.lm_Children, msg);
	}
	else
	{
	    group_minmax_2d(cl, obj, lm.lm_Children, msg);
	}
    }

g_print("Group_AskMinMax (%p): Min=%dx%d Max=%dx%d Def=%dx%d\n",
	obj,
	msg->MinMaxInfo->MinWidth, msg->MinMaxInfo->MinHeight,
	msg->MinMaxInfo->MaxWidth, msg->MinMaxInfo->MaxHeight,
	msg->MinMaxInfo->DefWidth, msg->MinMaxInfo->DefHeight);
    return 0;
}


    /*
    ** Layout function. Here, we have to call MUI_Layout() for each
    ** our children. MUI wants us to place them in a rectangle
    ** defined by (0,0,lm->lm_Layout.Width-1,lm->lm_Layout.Height-1)
    ** You are free to put the children anywhere in this rectangle.
    **
    ** Return TRUE if everything went ok, FALSE on error.
    ** Note: Errors during layout are not easy to handle for MUI.
    **       Better avoid them!
    */
static void
group_layout_horiz(struct IClass *cl, Object *obj, struct MinList *children)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    Object *cstate;
    Object *child;
    LONG left = 0;
    LONG top = 0;
    LONG width;
    LONG height;
    LONG bonus = 0;
    LONG totalBonus = _mwidth(obj) - (data->num_childs - 1) * data->horiz_spacing;

    data->horiz_weight_sum = 0;
    /*
     * pass 1 : consider fixed size objects, and calc weight
     */
    cstate = (Object *)children->mlh_Head;
    while ((child = NextObject(&cstate)))
    {
	if (! (_flags(child) & MADF_SHOWME))
	    continue;
	totalBonus -= _minwidth(child);

	if (_minwidth(child) != _maxwidth(child))
	    data->horiz_weight_sum += _hweight(child);
    }
    if (data->horiz_weight_sum == 0) /* fixed width childs */
    {
	left = totalBonus / 2;
	data->horiz_weight_sum = 1;
    }
    /* max size ?  (too much bonus) */
    cstate = (Object *)children->mlh_Head;
    while ((child = NextObject(&cstate)))
    {
	_flags(child) &= ~MADF_MAXSIZE;
	if (! (_flags(child) & MADF_SHOWME))
	    continue;
	if (_minwidth(child) == _maxwidth(child))
	    continue;
	if ((totalBonus * _hweight(child) / (double)data->horiz_weight_sum) >
	    (_maxwidth(child) - _minwidth(child)))
	{
	    _flags(child) |= MADF_MAXSIZE;
	    totalBonus -= _maxwidth(child) - _minwidth(child);
	    data->horiz_weight_sum -= _hweight(child);
	}
    }
    /*
     * pass 2 : distribute space. Minimum space is available,
     * bonus space is distributed according to weights
     */
    cstate = (Object *)children->mlh_Head;
    while ((child = NextObject(&cstate)))
    {
	gboolean has_variable_width;

	if (! (_flags(child) & MADF_SHOWME))
	    continue;
	has_variable_width = (_minwidth(child) != _maxwidth(child)) &&
	    !(_flags(child) & MADF_MAXSIZE);
	/* center child if group height is bigger than maxheight */
	height = MIN(_maxheight(child), _mheight(obj));
	height = MAX(height, _minheight(child));
	top = (_mheight(obj) - height) / 2;
	width = (_flags(child) & MADF_MAXSIZE) ?
	    _maxwidth(child) : _minwidth(child);
	if (has_variable_width)
	{
	    bonus = ROUND(totalBonus * _hweight(child)
			  / (double)data->horiz_weight_sum);
	    bonus = MIN(bonus, _maxwidth(child) - width);
	    width += bonus;
	}
	width = CLAMP(width, _minwidth(child), _maxwidth(child));
	if (!MUI_Layout(child, left, top, width, height, 0))
	    return;
	left += data->horiz_spacing + width;
	if (has_variable_width)
	{
	    data->horiz_weight_sum -= _hweight(child);
	    totalBonus -= bonus;
	}	
    }
}


static void
group_layout_vert(struct IClass *cl, Object *obj, struct MinList *children)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    Object *cstate;
    Object *child;
    LONG left = 0;
    LONG top = 0;
    LONG width;
    LONG height;
    LONG bonus = 0;
    LONG totalBonus = _mheight(obj) - (data->num_childs - 1) * data->vert_spacing;

    data->vert_weight_sum = 0;
    /*
     * pass 1 : consider fixed size objects, and calc weight
     */
    cstate = (Object *)children->mlh_Head;
    while ((child = NextObject(&cstate)))
    {
	if (! (_flags(child) & MADF_SHOWME))
	    continue;
	totalBonus -= _minheight(child);

	if (_minheight(child) != _maxheight(child))
	    data->vert_weight_sum += _vweight(child);
    }
    if (data->vert_weight_sum == 0) /* fixed height childs */
    {
	top = totalBonus / 2;
	data->vert_weight_sum = 1;
    }
    /* max size ?  (too much bonus) */
    cstate = (Object *)children->mlh_Head;
    while ((child = NextObject(&cstate)))
    {
	_flags(child) &= ~MADF_MAXSIZE;
	if (! (_flags(child) & MADF_SHOWME))
	    continue;
	if (_minheight(child) == _maxheight(child))
	    continue;
	if ((totalBonus * _vweight(child) / (double)data->vert_weight_sum) >
	    (_maxheight(child) - _minheight(child)))
	{
	    _flags(child) |= MADF_MAXSIZE;
	    totalBonus -= _maxheight(child) - _minheight(child);
	    data->vert_weight_sum -= _vweight(child);
	}
    }
    /*
     * pass 2 : distribute space. Minimum space is available,
     * bonus space is distributed according to weights
     */
    cstate = (Object *)children->mlh_Head;
    while ((child = NextObject(&cstate)))
    {
	gboolean has_variable_height;

	if (! (_flags(child) & MADF_SHOWME))
	    continue;
	has_variable_height = (_minheight(child) != _maxheight(child)) &&
	    !(_flags(child) & MADF_MAXSIZE);
	width = MIN(_maxwidth(child), _mwidth(obj));
	width = MAX(width, _minwidth(child));
	left = (_mwidth(obj) - width) / 2;
	height = (_flags(child) & MADF_MAXSIZE) ?
	    _maxheight(child) : _minheight(child);
	if (has_variable_height)
	{
	    bonus = ROUND(totalBonus * _vweight(child)
			  / (double)data->vert_weight_sum);
	    bonus = MIN(bonus, _maxheight(child) - height);
	    height += bonus;
	}

	height = CLAMP(height, _minheight(child), _maxheight(child));
	if (!MUI_Layout(child, left, top, width, height, 0))
	    return;
	top += data->vert_spacing + height;
	if (has_variable_height)
	{
	    data->vert_weight_sum -= _vweight(child);
	    totalBonus -= bonus;
	}
    }
}

static void
layout_2d_row_precalc (struct MUI_GroupData *data,
		       struct layout2d_elem *row_infos,
		       struct MinList *children,
		       LONG *totBonusHe, LONG *top_start)
{
    Object *cstate;
    Object *child;
    int i, j;

    cstate = (Object *)children->mlh_Head;
    /* for each row */
    for (i = 0; i < data->rows; i++)
    {
	/* min and max heights */
	row_infos[i].min = 0;
	row_infos[i].max = MUI_MAXMAX;

	j = 0;
	while ((child = NextObject(&cstate)))
	{
	    if (! (_flags(child) & MADF_SHOWME))
		continue;
	    row_infos[i].min = MAX(row_infos[i].min, _minheight(child));
	    row_infos[i].max = MIN(row_infos[i].max, _maxheight(child));
	    row_infos[i].weight += _vweight(child);
	    ++j;
	    if ((j % data->columns) == 0)
		break;
	}
	row_infos[i].max = MAX(row_infos[i].max, row_infos[i].min);
	/* process results for this row */
	*totBonusHe -= row_infos[i].min;
	if (row_infos[i].min != row_infos[i].max)
	    data->vert_weight_sum += row_infos[i].weight;
/*  	g_print("l1 row %d : %d %d %d\n", i, */
/*  		row_infos[i].min, row_infos[i].max, row_infos[i].weight); */
    }
    if (data->vert_weight_sum == 0)
    {
	*top_start = *totBonusHe / 2;
	data->vert_weight_sum = 1;
    }
}

static void
layout_2d_col_precalc (struct MUI_GroupData *data,
		       struct layout2d_elem *col_infos,
		       struct MinList *children,
		       LONG *totBonusWi, LONG *left_start)
{
    Object *cstate;
    Object *child;
    int i, j;

    /* for each col */
    for (i = 0; i < data->columns; i++)
    {
	/* min and max widths */
	col_infos[i].min = 0;
	col_infos[i].max = MUI_MAXMAX;

	j = 0;
	cstate = (Object *)children->mlh_Head;
	while ((child = NextObject(&cstate)))
	{
	    if (! (_flags(child) & MADF_SHOWME))
		continue;
	    ++j;
	    if (((j - 1) % data->columns) != i)
		continue;
	    col_infos[i].min = MAX(col_infos[i].min, _minwidth(child));
	    col_infos[i].max = MIN(col_infos[i].max, _maxwidth(child));
	    col_infos[i].weight += _hweight(child);
	}
	col_infos[i].max = MAX(col_infos[i].max, col_infos[i].min);
	/* process results for this col */
	*totBonusWi -= col_infos[i].min;
	if (col_infos[i].min != col_infos[i].max)
	    data->horiz_weight_sum += col_infos[i].weight;
/*  	g_print("l1 col %d : %d %d %d\n", i, */
/*  		col_infos[i].min, col_infos[i].max, col_infos[i].weight); */
    }
    if (data->horiz_weight_sum == 0)
    {
	*left_start = *totBonusWi / 2;
	data->horiz_weight_sum = 1;
    }
}

void
static layout_2d_calc_rowcol_dims (struct MUI_GroupData *data,
				   struct layout2d_elem *row_infos,
				   struct layout2d_elem *col_infos,
				   LONG totBonusHe, LONG totBonusWi)
{
    int i;
    LONG bonusHe = 0;
    LONG bonusWi = 0;

    /* calc row heights */
    for (i = 0; i < data->rows; i++)
    {
	row_infos[i].dim = row_infos[i].min;
	if (row_infos[i].min != row_infos[i].max)
	{
	    bonusHe = ROUND(totBonusHe * row_infos[i].weight
			    / (double)data->vert_weight_sum);
	    row_infos[i].dim += bonusHe;
	    row_infos[i].dim = CLAMP(row_infos[i].dim,
				     row_infos[i].min, row_infos[i].max);
	    data->vert_weight_sum -= row_infos[i].weight;
	    totBonusHe -= bonusHe;
	}
/*  	g_print("l2 row %d : %d\n", i, row_infos[i].dim); */
    }

    /* calc columns widths */
    for (i = 0; i < data->columns; i++)
    {
	col_infos[i].dim = col_infos[i].min;
	if (col_infos[i].min != col_infos[i].max)
	{
	    bonusWi = ROUND(totBonusWi * col_infos[i].weight
			    / (double)data->horiz_weight_sum);
	    col_infos[i].dim += bonusWi;
	    col_infos[i].dim = CLAMP(col_infos[i].dim,
				     col_infos[i].min, col_infos[i].max);
	    data->horiz_weight_sum -= col_infos[i].weight;
	    totBonusWi -= bonusWi;
	}
    }
}

static void
layout_2d_distribute_space (struct MUI_GroupData *data,
			    struct layout2d_elem *row_infos,
			    struct layout2d_elem *col_infos,
			    struct MinList *children,
			    LONG left_start, LONG top_start)
{
    Object *cstate;
    Object *child;
    LONG left;
    LONG top;
    LONG col_width;
    LONG row_height;
    int i, j;

    /*
     * pass 2 : distribute space
     */
    cstate = (Object *)children->mlh_Head;
    top = top_start;
    /* for each row */
    for (i = 0; i < data->rows; i++)
    {
	/* left start for child layout in this row */
	left = left_start;

	/* max height for childs in this row */
	row_height = row_infos[i].dim;
	j = 0;
	/* for each column */
	while ((child = NextObject(&cstate)))
	{
	    LONG cleft;
	    LONG ctop;
	    LONG cwidth;
	    LONG cheight;

	    if (! (_flags(child) & MADF_SHOWME))
		continue;
	    /* max width for childs in this column */
	    col_width = col_infos[j].dim;

	    /* center child if col width is bigger than child maxwidth */
	    cwidth = MIN(_maxwidth(child), col_width);
	    cwidth = MAX(cwidth, _minwidth(child));
	    cleft = left + (col_width - cwidth) / 2;

	    /* center child if row height is bigger than child maxheight */
	    cheight = MIN(_maxheight(child), row_height);
	    cheight = MAX(cheight, _minheight(child));
	    ctop = top + (row_height - cheight) / 2;

/*  	    g_print("layout %d %d  %d %d\n", cleft, ctop, cwidth, cheight); */

	    if (!MUI_Layout(child, cleft, ctop, cwidth, cheight, 0))
		return;

	    left += data->horiz_spacing + col_width;

	    ++j;
	    if ((j % data->columns) == 0)
		break;
	}

	top += data->vert_spacing + row_height;
    }
}

/*
 * all childs in the same row have the same maximum height
 * all childs in the same column have the same maximum height
 * if a child maximum size is smaller than the biggest minimum size,
 * the chid will be centered in the remaining space.
 *
 * for each row, determine its height allocation
 * weight ? the vertical weight of a row, if no fixed-height child
 *  in the row, is the sum of all vertical weights of childs
 * all row members will have the same height
 *
 * for each column, determine its width allocation
 * all column members will have the same width
 */
static void
group_layout_2d(struct IClass *cl, Object *obj, struct MinList *children)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    LONG left_start = 0;
    LONG top_start = 0;
    LONG totBonusHe = _mheight(obj) - (data->rows - 1) * data->vert_spacing;
    LONG totBonusWi = _mwidth(obj) - (data->columns - 1) * data->horiz_spacing;
    struct layout2d_elem *row_infos;
    struct layout2d_elem *col_infos;

    if (data->rows == 0)
    {
	g_printerr("zune: group_layout_2d: the number of childs "
		   "is not a multiple of the number of columns.\n");
	return;
    }

    if (data->columns == 0)
    {
	g_printerr("zune: group_layout_2d: the number of childs "
		   "is not a multiple of the number of rows.\n");
	return;
    }

    g_return_if_fail((data->num_childs % data->rows) == 0);
    g_return_if_fail((data->num_childs % data->columns) == 0);

    /* it's ugly to store these values, but is there another solution ? */
    row_infos = g_malloc0(data->rows * sizeof(struct layout2d_elem));
    col_infos = g_malloc0(data->columns * sizeof(struct layout2d_elem));

    data->horiz_weight_sum = 0;
    data->vert_weight_sum = 0;

    layout_2d_row_precalc(data, row_infos, children,
			  &totBonusHe, &top_start);
    layout_2d_col_precalc(data, col_infos, children,
			  &totBonusWi, &left_start);

    layout_2d_calc_rowcol_dims (data, row_infos, col_infos,
				totBonusHe, totBonusWi);
    layout_2d_distribute_space (data, row_infos, col_infos,
				children, left_start, top_start);
    g_free(row_infos);
    g_free(col_infos);
}


static void
group_layout_pagemode (struct IClass *cl, Object *obj, struct MinList *children)
{
    Object *cstate;
    Object *child;
    int w, h;

    cstate = (Object *)children->mlh_Head;
    while ((child = NextObject(&cstate)))
    {
	w = MIN(_mwidth(obj), _maxwidth(child));
	h = MIN(_mheight(obj), _maxheight(child));

	MUI_Layout(child, (_mwidth(obj) - w) / 2, (_mheight(obj) - h) / 2,
		   w, h, 0);
    }
}

/*
 * Either use a given layout hook, or the builtin method.
 */
static ULONG
mLayout(struct IClass *cl, Object *obj, struct MUIP_Layout *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    struct MUI_LayoutMsg lm;

    get(data->family, MUIA_Family_List, (ULONG *)&(lm.lm_Children));
    if (data->flags & GROUP_PAGEMODE)
    {
	group_layout_pagemode(cl, obj, lm.lm_Children);
    }
    else if (data->layout_hook)
    {
 
	lm.lm_Type = MUILM_LAYOUT;
	lm.lm_Layout.Width = _mwidth(obj);
	lm.lm_Layout.Height = _mheight(obj);

	(*(data->layout_hook->h_Entry))(data->layout_hook, obj, &lm);
    }
    else
    {
	if ((data->rows == 1) && (data->columns == 1))
	{
	    if (data->flags & GROUP_HORIZ)
		group_layout_horiz(cl, obj, lm.lm_Children);
	    else
		group_layout_vert(cl, obj, lm.lm_Children);
	}
	else
	    group_layout_2d(cl, obj, lm.lm_Children);
    }
    return 0;
}


/*
 * MUIM_Export : to export an objects "contents" to a dataspace object.
 */
static ULONG
mExport(struct IClass *cl, Object *obj, struct MUIP_Export *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    STRPTR id;

    if ((id = muiNotifyData(obj)->mnd_ObjectID))
    {
	DoMethod(msg->dataspace, MUIM_Dataspace_AddInt,
		 _U(id), _U("activePage"), data->active_page);
    }
    return DoSuperMethodA(cl, obj, (Msg)msg);
}


/*
 * MUIM_Import : to import an objects "contents" from a dataspace object.
 */
static ULONG
mImport(struct IClass *cl, Object *obj, struct MUIP_Import *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);
    STRPTR id;

    if ((id = muiNotifyData(obj)->mnd_ObjectID))
    {
	DoMethod(msg->dataspace, MUIM_Dataspace_FindInt,
		 _U(id), _U("activePage"), _U(&data->active_page));
    }
    return DoSuperMethodA(cl, obj, (Msg)msg);
}


/*
 * MUIM_FindUData : tests if the MUIA_UserData of the object
 * contains the given <udata> and returns the object pointer in this case.
 */
static ULONG
mFindUData(struct IClass *cl, Object *obj, struct MUIP_FindUData *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);

    if (muiNotifyData(obj)->mnd_UserData == msg->udata)
	return (ULONG)obj;

    return DoMethodA(data->family, (Msg)msg);
}


/*
 * MUIM_GetUData : This method tests if the MUIA_UserData of the object
 * contains the given <udata> and gets <attr> to <storage> for itself
 * in this case.
 */
static ULONG
mGetUData(struct IClass *cl, Object *obj, struct MUIP_GetUData *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);

    if (muiNotifyData(obj)->mnd_UserData == msg->udata)
    {
	get(obj, msg->attr, msg->storage);
	return TRUE;
    }

    return DoMethodA(data->family, (Msg)msg);
}


/*
 * MUIM_SetUData : This method tests if the MUIA_UserData of the object
 * contains the given <udata> and sets <attr> to <val> for itself in this case.
 */
static ULONG 
mSetUData(struct IClass *cl, Object *obj, struct MUIP_SetUData *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);

    if (muiNotifyData(obj)->mnd_UserData == msg->udata)
	set(obj, msg->attr, msg->val);

    DoMethodA(data->family, (Msg)msg);
    return TRUE;
}


/*
 * MUIM_SetUDataOnce : This method tests if the MUIA_UserData of the object
 * contains the given <udata> and sets <attr> to <val> for itself in this case.
 * Stop after the first udata found.
 */
static ULONG 
mSetUDataOnce(struct IClass *cl, Object *obj, struct MUIP_SetUData *msg)
{
    struct MUI_GroupData *data = INST_DATA(cl, obj);

    if (muiNotifyData(obj)->mnd_UserData == msg->udata)
    {
	set(obj, msg->attr, msg->val);
	return TRUE;
    }
    return DoMethodA(data->family, (Msg)msg);
}


static ULONG
Group_Dispatcher (struct IClass *cl, Object *obj, Msg msg)
{
    switch (msg->MethodID)
    {
    case OM_NEW:
	return(mNew(cl, obj, (struct opSet *) msg));
    case OM_DISPOSE:
	return(mDispose(cl, obj, msg));
    case OM_SET:
	return(mSet(cl, obj, (struct opSet *)msg));
    case OM_GET:
	return(mGet(cl, obj, (struct opGet *)msg));
    case OM_ADDMEMBER:
	return(mAddMember(cl, obj, (APTR)msg));
    case OM_REMMEMBER:
	return(mRemMember(cl, obj, (APTR)msg));
    case MUIM_AskMinMax :
	return(mAskMinMax(cl, obj, (APTR)msg));
    case MUIM_Group_ExitChange :
	return(mExitChange(cl, obj, (APTR)msg));
    case MUIM_Group_InitChange :
	return(mInitChange(cl, obj, (APTR)msg));
    case MUIM_Group_Sort :
	return(mSort(cl, obj, (APTR)msg));
    case MUIM_ConnectParent :
	return(mConnectParent(cl, obj, (APTR)msg));
    case MUIM_DisconnectParent :
	return(mDisconnectParent(cl, obj, (APTR)msg));
    case MUIM_ConnectParentWindow :
	return(mConnectParentWindow(cl, obj, (APTR)msg));
    case MUIM_Layout :
	return(mLayout(cl, obj, (APTR)msg));
    case MUIM_Setup :
	return(mSetup(cl, obj, (APTR)msg));
    case MUIM_Cleanup :
	return(mCleanup(cl, obj, (APTR)msg));
    case MUIM_Draw :
	return(mDraw(cl, obj, (APTR)msg));

    case MUIM_Export :
	return(mExport(cl, obj, (APTR)msg));
    case MUIM_Import :
	return(mImport(cl, obj, (APTR)msg));
    case MUIM_FindUData :
	return(mFindUData(cl, obj, (APTR)msg));
    case MUIM_GetUData :
	return(mGetUData(cl, obj, (APTR)msg));
    case MUIM_SetUData :
	return(mSetUData(cl, obj, (APTR)msg));
    case MUIM_SetUDataOnce :
	return(mSetUDataOnce(cl, obj, (APTR)msg));
    }

    DoSuperMethodA(cl, obj, msg);
    /* sometimes you want to call a superclass method,
     * but not dispatching to child. 
     * But what to do with list methods in a listview ?
     */
    Group_DispatchMsg(cl, obj, (APTR)msg);
    return TRUE;
}


/*
 * Class descriptor.
 */
const struct __MUIBuiltinClass _MUI_Group_desc = { 
    MUIC_Group, 
    MUIC_Area, 
    sizeof(struct MUI_GroupData), 
    Group_Dispatcher 
};
