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

#include <Area.h>
#include <Numeric.h>
#include <Slider.h>
#include <sliderdata.h>
#include <Group.h>
#include <Window.h>
#include <renderinfo.h>
#include <macros.h>

/* This class is said to be derived from MUI_Numeric.
 * However, it is in fact derived from MUI_Group. It doesn't change anything,
 * because MUI_Numeric methods and attributes are passed to the knob child
 * which is derived from MUI_Numeric.
 */


static ULONG 
mNew(struct IClass *cl, Object * obj, struct opSet *msg)
{
    struct MUI_SliderData *data;
    struct MUI_SliderData  dat;
    struct TagItem *tags, *tag;

    for (tags = msg->ops_AttrList; (tag = NextTagItem(&tags));)
    {
	switch (tag->ti_Tag)
	{
	    case MUIA_Group_Horiz:
	    case MUIA_Slider_Horiz:
		_handle_bool_tag(dat.flags, tag->ti_Data, SLIDER_HORIZ);
		break;
	    case MUIA_Slider_Quiet:
		_handle_bool_tag(dat.flags, tag->ti_Data, SLIDER_QUIET);
		break;
	}
    }

    if (!(obj = (Object *) DoSuperNew(cl, obj,
				      MUIA_Group_Horiz, dat.flags & SLIDER_HORIZ,
				      MUIA_Background, MUII_SliderBack,
				      MUIA_Frame, MUIV_Frame_Slider,
				      (ULONG)dat.knob = NumericObject,
				          MUIA_Background, MUII_SliderKnob,
				          MUIA_Frame, MUIV_Frame_Knob,
				      End,
				      TAG_MORE, msg->ops_AttrList,
				      TAG_DONE)))
	return (0);

    if (!dat.knob)
    {
	CoerceMethod(cl, obj, OM_DISPOSE);
	return 0;
    }

    data = INST_DATA(cl, obj);
    *data = dat;

    data->ehn.ehn_Events = GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK;
    data->ehn.ehn_Priority = 0;
    data->ehn.ehn_Flags = 0;
    data->ehn.ehn_Object = obj;
    data->ehn.ehn_Class = cl;

    return ((ULONG) obj);
}

static ULONG
mDispose(struct IClass *cl, Object *obj, Msg msg)
{
/*      struct MUI_SliderData *data = INST_DATA(cl, obj); */
/*      Object *cont; */

/*  g_print("slider dispose\n"); */
/*      if (data->knob) */
/*      { */
/*      } */
/*      return DoSuperMethodA(cl, obj, msg); */
}

static ULONG
mSetup(struct IClass *cl, Object *obj, struct MUIP_Setup *msg)
{
    struct MUI_SliderData *data = INST_DATA(cl, obj);

    if (!DoSuperMethodA(cl,obj,(Msg)msg))
	return FALSE;

    DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->ehn);

    return TRUE;
}

static ULONG
mCleanup(struct IClass *cl, Object *obj, struct MUIP_Cleanup *msg)
{
    struct MUI_SliderData *data = INST_DATA(cl, obj);

    DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->ehn);
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

static ULONG
mAskMinMax(struct IClass *cl, Object *obj, struct MUIP_AskMinMax *msg)
{
    struct MUI_SliderData *data = INST_DATA(cl, obj);

    DoSuperMethodA(cl, obj, (Msg)msg);

    if (data->flags & SLIDER_HORIZ)
    {
    }

}

static ULONG
mDraw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
    struct MUI_SliderData *data = INST_DATA(cl, obj);

    DoSuperMethodA(cl,obj,(Msg)msg);
    if (!(msg->flags & MADF_DRAWOBJECT))
	return(0);

}

static ULONG
event_button_press(struct IClass *cl, Object *obj, GdkEventButton *evb)
{
}

static ULONG
event_button_release(struct IClass *cl, Object *obj, GdkEventButton *evb)
{
}

static ULONG
event_motion(struct IClass *cl, Object *obj, GdkEventMotion *evm)
{
}

static ULONG
mHandleEvent(struct IClass *cl, Object *obj, struct MUIP_HandleEvent *msg)
{
    struct MUI_SliderData *data = INST_DATA(cl, obj);

    if (msg->imsg)
    {
	switch (msg->imsg->type)
	{
	    case GDK_BUTTON_PRESS:
		return event_button_press(cl, obj, (GdkEventButton *)msg->imsg);
	    case GDK_BUTTON_RELEASE:
		return event_button_release(cl, obj, (GdkEventButton *)msg->imsg);
	    case GDK_MOTION_NOTIFY:
		return event_motion(cl, obj, (GdkEventMotion *)msg->imsg);
	    default:
		return 0;
       }
    }
    return MUI_EventHandlerRC_Eat;
}


static ULONG
Slider_Dispatcher (struct IClass *cl, Object *obj, Msg msg)
{
    switch (msg->MethodID)
    {
	case OM_NEW:
	    return(mNew(cl, obj, (struct opSet *) msg));
	case OM_DISPOSE:
	    return(mDispose(cl, obj, msg));
	case MUIM_Setup :
	    return(mSetup(cl, obj, (APTR)msg));
	case MUIM_Cleanup :
	    return(mCleanup(cl, obj, (APTR)msg));
	case MUIM_AskMinMax :
	    return(mAskMinMax(cl, obj, (APTR)msg));
	case MUIM_Draw :
	    return(mDraw(cl, obj, (APTR)msg));
	case MUIM_HandleEvent:
	    return(mHandleEvent(cl, obj, (APTR)msg));
    }
    return (DoSuperMethodA(cl, obj, msg));
}


/*
 * Class descriptor.
 */
const struct __MUIBuiltinClass _MUI_Slider_desc = { 
    MUIC_Slider, 
    MUIC_Group, 
    sizeof(struct MUI_SliderData), 
    Slider_Dispatcher 
};
