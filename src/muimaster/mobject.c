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
#include <muio.h>
#include <macros.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <renderinfo.h>
#include <Text.h>
#include <Area.h>
#include <Rectangle.h>
#include <Image.h>
#include <Group.h>
#include <Cycle.h>

#define MAX_TAGS 50

/*
 * Create an object from a class.
 */
Object *
MUI_NewObjectA (ClassID classID, struct TagItem *tagList)
{
    Class *cl;

/*      g_print("MUI_NewObjectA %s\n", classID); */

    cl = MUI_GetClass(classID);
    if (cl)
	return (NewObjectA(cl, classID, tagList));
    else
	return NULL;
}

/*
 * Delete a MUI object.
 */
void
MUI_DisposeObject (Object *object)
{
    ULONG MethodID = OM_DISPOSE;

    if (!object)
	return;

    OCLASS (object)->cl_ObjectCount --;
    DoMethodA (object, (Msg)&MethodID);
}


/*
 * create an object from the builtin object collection.
 */
Object *
MUI_MakeObjectA(LONG type, ULONG *params)
{
    switch (type)
    {
	case MUIO_Label: /* STRPTR label, ULONG flags */
	{
	    TagItem tags[5];
	    int ntags = 0;

	    if (params[1] & MUIO_Label_SingleFrame)
	    {
		tags[ntags].ti_Tag = MUIA_Frame;
		tags[ntags].ti_Data = (IPTR)MUIV_Frame_ImageButton;
		ntags++;
	    }
	    else if (params[1] & MUIO_Label_DoubleFrame)
	    {
		tags[ntags].ti_Tag = MUIA_Frame;
		tags[ntags].ti_Data = (IPTR)MUIV_Frame_String;
		ntags++;
	    }

	    if (params[1] & MUIO_Label_LeftAligned)
	    {
		tags[ntags].ti_Tag = MUIA_Text_PreParse;
		tags[ntags].ti_Data = (IPTR)"\33l";
		ntags++;
	    }
	    else if (params[1] & MUIO_Label_Centered)
	    {
		tags[ntags].ti_Tag = MUIA_Text_PreParse;
		tags[ntags].ti_Data = (IPTR)"\33c";
		ntags++;
	    }
	    else
	    {
		tags[ntags].ti_Tag = MUIA_Text_PreParse;
		tags[ntags].ti_Data = (IPTR)"\33r";
		ntags++;
	    }

	    if (params[1] & MUIO_Label_FreeVert)
	    {
		tags[ntags].ti_Tag = MUIA_Text_SetVMax;
		tags[ntags].ti_Data = (IPTR)FALSE;
		ntags++;
	    }

	    tags[ntags].ti_Tag = TAG_DONE;
	    tags[ntags].ti_Data = 0;

	    return TextObject,
		MUIA_FramePhantomHoriz, TRUE,
		MUIA_Text_HiChar, params[1] & 0xff,
		MUIA_Text_Contents, params[0],
		TAG_MORE, tags,
		End;	
	    break;
	}
	case MUIO_Button: /* STRPTR label */
	{
	    return TextObject,
		    ButtonFrame,
		    MUIA_Font, MUIV_Font_Button,
		    MUIA_Text_HiCharIdx, '_',
		    MUIA_Text_Contents, params[0],
		    MUIA_Text_PreParse, "\33c",
		    MUIA_InputMode    , MUIV_InputMode_RelVerify,
		    MUIA_Background   , MUII_ButtonBack,
		End;
	}
	case MUIO_Checkmark: /* STRPTR label */
	{
	    return HGroup,
		Child, Label(params[0]),
		Child, ImageObject,
	            MUIA_Image_Spec, MUII_CheckMark,
	            MUIA_InputMode, MUIV_InputMode_Toggle,
	            MUIA_Frame, MUIV_Frame_None,
	            MUIA_ShowSelState, FALSE,
		End,
	       End;
	    break;
	}
	case MUIO_Cycle: /* STRPTR label, STRPTR *entries */
	    return HGroup,
		Child, LLabel1(params[0]),
		Child, CycleObject,
		    MUIA_Cycle_Entries, params[1],
		End,
		End;
	    break;
	case MUIO_Radio: /* STRPTR label, STRPTR *entries */
	    break;
	case MUIO_Slider: /* STRPTR label, LONG min, LONG max */
	    break;
	case MUIO_String: /* STRPTR label, LONG maxlen */
	    break;
	case MUIO_PopButton: /* STRPTR imagespec */
	    return ImageObject,
		MUIA_Image_Spec, params[0],
		MUIA_InputMode, MUIV_InputMode_RelVerify,
		MUIA_ShowSelState, FALSE,
		End;
	    break;
	case MUIO_HSpace: /* LONG space   */
	    return RectangleObject,
		MUIA_FixWidth, params[0],
		End;
	    break;
	case MUIO_VSpace: /* LONG space   */
	    return RectangleObject,
		MUIA_FixHeight, params[0],
		End;
	    break;
	case MUIO_HBar: /* LONG space   */
	    return RectangleObject,
		MUIA_Rectangle_HBar, TRUE,
		MUIA_FixHeight, params[0],
		End;
	    break;
	case MUIO_VBar: /* LONG space   */
	    return RectangleObject,
		MUIA_Rectangle_VBar, TRUE,
		MUIA_FixWidth, params[0],
		End;
	    break;
	case MUIO_MenustripNM: /* struct NewMenu *nm, ULONG flags */
	    break;
	case MUIO_Menuitem: /* STRPTR label, STRPTR shortcut, ULONG flags, ULONG data  */
	    break;
	case MUIO_BarTitle: /* STRPTR label */
	    return RectangleObject,
		MUIA_Rectangle_HBar, TRUE,
		MUIA_Rectangle_BarTitle, params[0],
		End;
	    break;
	case MUIO_NumericButton: /* STRPTR label, LONG min, LONG max, STRPTR format */
	    break;
	default:
    }
    return NULL;
}


/*
 * Redraw yourself.
 * Instead of drawing directly during OM_SET, you should simply call
 * MUI_Redraw(). MUI calculates all necessary coordinates
 * and clip regions (in case of virtual groups) and then sends
 * a MUIM_Draw method to your object.
 */
void
MUI_Redraw(Object *obj, ULONG flag)
{
    muiRenderInfo(obj)->mri_ClipRect.x = _left(obj);
    muiRenderInfo(obj)->mri_ClipRect.y = _top(obj);
    muiRenderInfo(obj)->mri_ClipRect.width = _width(obj);
    muiRenderInfo(obj)->mri_ClipRect.height = _height(obj);

    DoMethod(obj, MUIM_Draw, flag);
}


/*
 * Pop up a MUI requester.
 */
LONG
MUI_RequestA (APTR app, APTR win, LONGBITS flags,
	      char *title, char *gadgets, char *format, APTR params)
{
    return 0;
}

static LONG
MUI_RequestA_intern (APTR app, APTR win, LONGBITS flags,
		     char *title, char *gadgets, char *body)
{
    return 0;
}



/*
 * Reject previously requested input events.
 */
/*  VOID */
/*  MUI_RejectIDCMP(Object *obj, ULONG flags) */
/*  { */
/*  } */


/*
 * Request input events for your custom class.
 * Whenever an input event you requested arrives at your parent
 * windows message port, your object will receive a
 * MUIM_HandleInput method.
 */
/*  VOID */
/*  MUI_RequestIDCMP(Object *obj, ULONG flags) */
/*  { */
    /* Get window object
     * Get list of objects that requested idcmp
     * put object and idcmp in list
     * window will check that list for matching idcmp
     */
/*      Object *winobj = _win(obj); */

/*      if (winobj != NULL) */
/*      { */

/*      } */
/*  } */

Object *
SimpleChainedButton (STRPTR label)
{
    Object *obj;

    obj = SimpleButton(label);
    set(obj, MUIA_CycleChain, TRUE);
    return obj;
}

Object *
ChainedCheckmark (STRPTR label)
{
    Object *obj;

    obj = MUI_MakeObject(MUIO_Checkmark, _U(label));
    set(obj, MUIA_CycleChain, TRUE);
    return obj;
}

#if 1
/*
 * Varargs stub for MUI_NewObjectA().
 */
Object *
MUI_NewObject (ClassID classID, ULONG tag1, ...)
{
    TagItem tags[MAX_TAGS];
    int     i;
    va_list ap;
    
    tags[0].ti_Tag = tag1;
    if (tag1 != TAG_DONE)
    {
	va_start(ap, tag1);
	tags[0].ti_Data = va_arg(ap, ULONG);
	for (i = 1; i < MAX_TAGS; i++)
	{
	    tags[i].ti_Tag = va_arg(ap, Tag);
	    if (tags[i].ti_Tag == TAG_DONE)
		break;
	    tags[i].ti_Data = va_arg(ap, ULONG);
	}
	va_end(ap);
    }
    return MUI_NewObjectA (classID, tags);
} /* MUI_NewObject */
#endif

#ifndef __GNUC__
/*
 * Varargs stub for MUI_MakeObjectA.
 */
Object *
MUI_MakeObject(LONG type, ...)
{
    va_list args;
    APTR    object;

    va_start (args, type);
    object = MUI_MakeObjectA (type, (ULONG *)args);
    va_end (args);
    return object;
}
#endif

/*
 * Varargs stub
 */
LONG
MUI_Request (APTR app, APTR win, LONGBITS flags,
	      char *title, char *gadgets, char *format, ...)
{
    gchar *buffer;
    LONG    retval;
    va_list args;

    va_start (args, format);
    buffer = g_strdup_vprintf (format, args);
    va_end (args);
 
    retval = MUI_RequestA_intern (app, win, flags, title, gadgets, buffer);
    g_free(buffer);
    return retval;
}
