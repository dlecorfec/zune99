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
#include <config.h>
#include <gdk/gdk.h>
#ifdef WITH_IMLIB
#include <gdk_imlib.h>
#endif
#include <zunepriv.h>
#include <renderinfo.h>
#include <window_impl.h>
#include <prefs.h>
#include <image.h>
#include <key.h>
#include <Application.h>
#include <Window.h>
#include <windowdata.h>
#include <muikey.h>
#include <ctype.h>

/* reference count event clients */
static gint event_masks_ref[32];

static void add_events_soft(struct MUI_WindowData *data, gint mask);
static void remove_events_soft(struct MUI_WindowData *data, gint mask);
static void handle_expose (Object *win, GdkEventExpose *ev);
static void handle_resize (Object *win, guint width, guint height);
static ULONG invoke_event_handler (struct MUI_EventHandlerNode *ehn,
				   GdkEvent *event, ULONG muikey);
static void handle_key(Object *win, GdkEventKey *event, gint mask);
static void handle_event(Object *win, GdkEvent *event, gint mask);

/****************************************************************************/
/** Public functions                                                       **/
/****************************************************************************/

BOOL
_zune_renderinfo_setup (struct MUI_RenderInfo *mri)
{
    gboolean success[MPEN_COUNT];
    int i;

    mri->mri_Pens = mri->mri_Pixels;
    mri->mri_DrawInfo = &mri->mri_DriHack;
    mri->mri_DrawInfo->dri_Pens = mri->mri_DrawInfo->dri_Pixels;

    mri->mri_ScreenWidth = gdk_screen_width();
    mri->mri_ScreenHeight = gdk_screen_height();

#ifdef WITH_IMLIB
    mri->mri_Visual = gdk_imlib_get_visual();
    mri->mri_Colormap = gdk_imlib_get_colormap();
#else
    mri->mri_Visual = gdk_visual_get_system();
    mri->mri_Colormap = gdk_colormap_get_system();
#endif
    
    g_return_val_if_fail(mri->mri_Colormap != NULL, FALSE);

    /* use prefs pens as a temp to alloc pixels, copy pixels to mri */
    gdk_colormap_alloc_colors(mri->mri_Colormap, __zprefs.muipens, MPEN_COUNT,
			      FALSE, TRUE, success);

    mri->mri_FocusPixel = MUI_ObtainPen (mri, &__zprefs.active_object_color, 0);
 
    for (i = 0; i < MPEN_COUNT; i++)
    {
	mri->mri_Pens[i] = __zprefs.muipens[i].pixel;
	mri->mri_DrawInfo->dri_Pens[i+2] = mri->mri_Pens[i];
    }
    mri->mri_DrawInfo->dri_Pens[TEXTPEN] = mri->mri_Pens[MPEN_TEXT];
    mri->mri_DrawInfo->dri_Pens[SHINEPEN] = mri->mri_Pens[MPEN_SHINE];
    mri->mri_DrawInfo->dri_Pens[SHADOWPEN] = mri->mri_Pens[MPEN_SHADOW];
    mri->mri_DrawInfo->dri_Pens[BARDETAILPEN] = mri->mri_Pens[MPEN_SHINE];
    mri->mri_DrawInfo->dri_Pens[BARBLOCKPEN] = mri->mri_Pens[MPEN_FILL];
    mri->mri_DrawInfo->dri_Pens[BARTRIMPEN] = mri->mri_Pens[MPEN_BACKGROUND];
    mri->mri_DrawInfo->dri_Pens[6] = mri->mri_Pens[MPEN_HALFSHINE];
    mri->mri_DrawInfo->dri_Pens[7] = mri->mri_Pens[MPEN_HALFSHADOW];
    mri->mri_DrawInfo->dri_Pens[8] = mri->mri_Pens[MPEN_SHINE];
    mri->mri_DrawInfo->dri_Pens[9] = mri->mri_Pens[MPEN_MARK];

    return TRUE;
}

void
_zune_renderinfo_cleanup (struct MUI_RenderInfo *mri)
{
    int i;

    for (i = 0; i < MPEN_COUNT; i++)
    {
	__zprefs.muipens[i].pixel = mri->mri_Pens[i];
	mri->mri_Pens[i] = 0;
    }
    mri->mri_Visual = NULL;
    if (mri->mri_Colormap)
    {
	gdk_colormap_free_colors(mri->mri_Colormap, __zprefs.muipens, MPEN_COUNT);
	/* Cannot unref colormap, would cause segfault on next setup
	 * when getting cmap from gdk or imlib. Perhaps creating our own ?
	 * Will need to do something to effectively free colors !
	 * Break MUI_ReleasePen API ? Anyway, I dont care in true color :)))
	 */
/*  	gdk_colormap_unref(mri->mri_Colormap); */
    }
    mri->mri_Colormap = NULL;
}

void
_zune_renderinfo_show (struct MUI_RenderInfo *mri)
{
    mri->mri_RastPort = gdk_gc_new(mri->mri_Window);
}

void
_zune_renderinfo_hide (struct MUI_RenderInfo *mri)
{
    gdk_gc_destroy(mri->mri_RastPort);
    mri->mri_RastPort = NULL;
}



gint
_zune_window_get_default_events (void)
{
    int i;
    for (i = 0; i < 32; i++)
	event_masks_ref[i] = 0;

    return GDK_EXPOSURE_MASK |
	GDK_KEY_PRESS_MASK |
	GDK_ENTER_NOTIFY_MASK |
	GDK_LEAVE_NOTIFY_MASK |
	GDK_FOCUS_CHANGE_MASK |
	GDK_STRUCTURE_MASK;
}

void
_zune_window_add_events (struct MUI_WindowData *data, gint mask)
{
    gint old_events = data->wd_Events;
/*  	g_print("old events= %d  mask = %d\n", data->wd_Events, mask); */

    add_events_soft(data, mask);
    if ((old_events != data->wd_Events) && (data->wd_Flags & MUIWF_OPENED))
    {
	gdk_window_set_events(data->wd_RenderInfo.mri_Window, 
			      data->wd_Events);
/*  	g_print("set events\n"); */
    }
}

void
_zune_window_remove_events (struct MUI_WindowData *data, gint mask)
{
    gint old_events = data->wd_Events;

    remove_events_soft(data, mask);
    if ((old_events != data->wd_Events) && (data->wd_Flags & MUIWF_OPENED))
	gdk_window_set_events(data->wd_RenderInfo.mri_Window, 
			      data->wd_Events);
}

/*
 * Can I open a toplevel window at a specific position ? Seems the
 * wm is annoying ...
 */
gboolean
_zune_window_open (struct MUI_WindowData *data)
{
    GdkWindow     *gdkwin;
    GdkWindowAttr  attributes;
    gint           attributes_mask = 0;
    GdkGeometry    geom;
    GdkWindowHints hints;

    g_return_val_if_fail(data->wd_RenderInfo.mri_Colormap != NULL, FALSE);

    attributes.window_type = GDK_WINDOW_TOPLEVEL;
    attributes.title = data->wd_Title;
    attributes.wmclass_name = "zune";
    attributes.wmclass_class = "Zune";
    attributes.width = data->wd_Width;
    attributes.height = data->wd_Height;
    if (data->wd_X)
    {
	attributes.x = data->wd_X;
	attributes.y = data->wd_Y;
	attributes_mask |= GDK_WA_X | GDK_WA_Y;
    }
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.visual = data->wd_RenderInfo.mri_Visual;
    attributes.colormap = data->wd_RenderInfo.mri_Colormap;
/*      attributes.event_mask = GDK_ALL_EVENTS_MASK ^ GDK_POINTER_MOTION_HINT_MASK; */
    
    attributes.event_mask = data->wd_Events;
    add_events_soft(data, data->wd_Events);
   
    attributes_mask |= (GDK_WA_VISUAL | GDK_WA_COLORMAP);
    attributes_mask |= (data->wd_Title ? GDK_WA_TITLE : 0);
    attributes_mask |= (attributes.wmclass_name ? GDK_WA_WMCLASS : 0);
   
    gdkwin = gdk_window_new (NULL, &attributes, attributes_mask);
    if (!gdkwin)
	return FALSE;

    gdk_window_set_user_data (gdkwin, data->wd_RenderInfo.mri_WindowObject);

    data->wd_RenderInfo.mri_Window = gdkwin;

    hints = GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE;
    geom.min_width = data->wd_MinMax.MinWidth;
    geom.min_height = data->wd_MinMax.MinHeight;
    geom.max_width = data->wd_MinMax.MaxWidth;
    geom.max_height = data->wd_MinMax.MaxHeight;
    if (data->wd_X)
    {
	hints |= GDK_HINT_POS;
    }
    gdk_window_set_geometry_hints(gdkwin, &geom, hints);

    gdk_window_show (gdkwin);
    return TRUE;
}

void
_zune_window_close (struct MUI_WindowData *data)
{
    gdk_window_get_position(data->wd_RenderInfo.mri_Window,
			    &data->wd_X, &data->wd_Y);
/*      g_print("get pos %d %d\n", data->wd_X, data->wd_Y); */
    gdk_window_destroy(data->wd_RenderInfo.mri_Window);
}

void
_zune_window_resize (struct MUI_WindowData *data)
{
    GdkGeometry    geom;
    GdkWindowHints hints;

    hints = GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE;
    geom.min_width = data->wd_MinMax.MinWidth;
    geom.min_height = data->wd_MinMax.MinHeight;
    geom.max_width = data->wd_MinMax.MaxWidth;
    geom.max_height = data->wd_MinMax.MaxHeight;

    gdk_window_set_geometry_hints(data->wd_RenderInfo.mri_Window, &geom, hints);

    gdk_window_resize(data->wd_RenderInfo.mri_Window,
		      data->wd_Width, data->wd_Height);
}

/**************/

void
_zune_focus_new (Object *obj)
{
    GdkWindowAttr  attributes;
    gint           attributes_mask = 0;
    WORD           w[4];
    WORD           h[4];
    WORD           x[4];
    WORD           y[4];
    int            i;
    GdkWindow    **focuswin = muiRenderInfo(obj)->mri_FocusWin;

    GdkColor color;
    color.pixel = muiRenderInfo(obj)->mri_FocusPixel;
/*  g_print("focus using pixel %lx\n", color.pixel); */
    i = 0;
    x[i] = _left(obj) - 1;
    y[i] = _top(obj) - 1;
    w[i] = _width(obj) + 2;
    h[i] = 1;
    i++;
    x[i] = _left(obj) - 1;
    y[i] = _top(obj) + _height(obj);
    w[i] = _width(obj) + 2;
    h[i] = 1;
    i++;
    x[i] = _left(obj) - 1;
    y[i] = _top(obj);
    w[i] = 1;
    h[i] = _height(obj);
    i++;
    x[i] = _left(obj) + _width(obj);
    y[i] = _top(obj);
    w[i] = 1;
    h[i] = _height(obj);

    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.visual = muiRenderInfo(obj)->mri_Visual;
    attributes.colormap = muiRenderInfo(obj)->mri_Colormap;  
    attributes.event_mask = muiWindowData(obj)->wd_Events;   
    attributes_mask |= GDK_WA_X | GDK_WA_Y;
    attributes_mask |= (GDK_WA_VISUAL | GDK_WA_COLORMAP); 

    for (i = 0; i < 4; i++)
    {
	attributes.width = w[i];
	attributes.height = h[i];
	attributes.x = x[i];
	attributes.y = y[i];

	focuswin[i] = NULL;
	focuswin[i] = gdk_window_new (_window(obj), &attributes, attributes_mask);
	if (!focuswin[i])
	    return;
	gdk_window_set_background(focuswin[i], &color);
	gdk_window_clear(focuswin[i]);
	gdk_window_set_user_data (focuswin[i], _win(obj));
	gdk_window_show (focuswin[i]);    
    }
}

void
_zune_focus_destroy (Object *obj)
{
    GdkWindow    **focuswin = muiRenderInfo(obj)->mri_FocusWin;
    int i;

    for (i = 0; i < 4; i++)
    {
	if (focuswin[i])
	    gdk_window_destroy(focuswin[i]);
	else
	    break;
	focuswin[i] = NULL;
    }
}

/**************/


gint
_zune_window_event(Object *win, GdkEvent *event)
{
    Object   *grab_object;

    g_return_val_if_fail (win != NULL, TRUE);
    g_return_val_if_fail (event != NULL, TRUE);

    /* If there is a grab in effect...
     */
    grab_object = win;

    /* Not all events get sent to the grabbing widget.
     * The delete, destroy, expose, focus change and resize
     *  events still get sent to the event widget because
     *  1) these events have no meaning for the grabbing widget
     *  and 2) redirecting these events to the grabbing widget
     *  could cause the display to be messed up.
     * 
     * Drag events are also not redirected, since it isn't
     *  clear what the semantics of that would be.
     */
    switch (event->type)
    {
	case GDK_NOTHING:
	    break;
      
	case GDK_DELETE:
	case GDK_DESTROY:
	    set(win, MUIA_Window_CloseRequest, TRUE);
	    break;
      
	case GDK_PROPERTY_NOTIFY:
	    g_print("atom name : %s\n", gdk_atom_name(((GdkEventProperty *)event)->atom));
	    break;
	case GDK_EXPOSE:
	    handle_expose(win, (GdkEventExpose *)event);
	    break;
	case GDK_NO_EXPOSE:
	case GDK_FOCUS_CHANGE:
	    break;
	case GDK_CONFIGURE:
	    handle_resize(win, ((GdkEventConfigure *)event)->width,
			  ((GdkEventConfigure *)event)->height);
	    break;
	case GDK_MAP:
	case GDK_UNMAP:
	case GDK_SELECTION_CLEAR:
	case GDK_SELECTION_REQUEST:
	case GDK_SELECTION_NOTIFY:
	    break;
	case GDK_CLIENT_EVENT:
	    break;
	case GDK_VISIBILITY_NOTIFY:
	    break;

	case GDK_BUTTON_PRESS:
	case GDK_2BUTTON_PRESS:
	case GDK_3BUTTON_PRESS:
	    handle_event(win, event, GDK_BUTTON_PRESS_MASK);
	    /* We treat button 4-5 specially, assume we have
	     * a MS-style scrollwheel mouse, and try to find
	     * a plausible widget to scroll. We also trap
	     * button 4-5 double and triple clicks here, since
	     * they will be generated if the user scrolls quickly.
	     */
	    break;

	case GDK_KEY_PRESS:
	    handle_key(win, (GdkEventKey *)event, GDK_KEY_PRESS_MASK);
	    break; 
	case GDK_KEY_RELEASE:
	    handle_key(win, (GdkEventKey *)event, GDK_KEY_RELEASE_MASK);
	    break;
	    /* key snoopers here */

	    /* else fall through */
	case GDK_MOTION_NOTIFY:
	    handle_event(win, event, GDK_POINTER_MOTION_MASK |
			 GDK_BUTTON_MOTION_MASK | GDK_BUTTON1_MOTION_MASK |
			 GDK_BUTTON2_MOTION_MASK | GDK_BUTTON3_MOTION_MASK);
	    break;
	case GDK_BUTTON_RELEASE:
	    handle_event(win, event, GDK_BUTTON_RELEASE_MASK);
	    break;
	case GDK_PROXIMITY_IN:
	    handle_event(win, event, GDK_PROXIMITY_IN_MASK);
	    break;
	case GDK_PROXIMITY_OUT:
	    handle_event(win, event, GDK_PROXIMITY_OUT_MASK);
	    break;
      

	    /*
	     * perhaps code to highlight active gadget here
	     */
	case GDK_ENTER_NOTIFY:
	    break;      
	case GDK_LEAVE_NOTIFY:
	    break;
      


	case GDK_DRAG_STATUS:
	case GDK_DROP_FINISHED:
	    break;
	case GDK_DRAG_ENTER:
	case GDK_DRAG_LEAVE:
	case GDK_DRAG_MOTION:
	case GDK_DROP_START:
	    break;
    }
  



    return FALSE;
}


/****************************************************************************/
/** Private functions                                                      **/
/****************************************************************************/

static void
add_events_soft(struct MUI_WindowData *data, gint mask)
{
    int i;
    for (i = 0; i < 32; i++)
	if (mask & (1 << i))
	    ++event_masks_ref[i];
    data->wd_Events |= mask;
}

static void
remove_events_soft(struct MUI_WindowData *data, gint mask)
{
    int i;
    for (i = 0; i < 32; i++)
	if (mask & (1 << i))
	{
	    --event_masks_ref[i];
	    if (event_masks_ref[i] == 0)
		data->wd_Events &= ~(1 << i);
	}
}

static void
handle_expose (Object *win, GdkEventExpose *ev)
{
    struct MUI_WindowData *data = muiWindowData(win);
    APTR clip;

/*      g_print("expose %d %d %d %d %d\n", ev->area.x, ev->area.y, */
/*  	ev->area.width, ev->area.height, ev->count); */
    data->wd_RenderInfo.mri_ClipRect.x = ev->area.x;
    data->wd_RenderInfo.mri_ClipRect.y = ev->area.y;
    data->wd_RenderInfo.mri_ClipRect.width = ev->area.width;
    data->wd_RenderInfo.mri_ClipRect.height = ev->area.height;
/*      zune_draw_image(&data->wd_RenderInfo, */
/*  		    __zprefs.images[MUII_WindowBack], */
/*  		    ev->area.x, ev->area.y, ev->area.width, ev->area.height, */
/*  		    0, 0, 0); */
    clip = MUI_AddClipping(&data->wd_RenderInfo, ev->area.x, ev->area.y,
			   ev->area.width, ev->area.height);
    DoMethod(data->wd_RootObject, MUIM_Draw, MADF_DRAWALL);
    MUI_RemoveClipping(&data->wd_RenderInfo, clip);
}


static void
handle_resize (Object *win, guint width, guint height)
{
    struct MUI_WindowData *data = muiWindowData(win);

    if ((width != data->wd_Width) ||
	(height != data->wd_Height))
    {
	data->wd_Width = width;
	data->wd_Height = height;
	DoMethod(data->wd_RootObject, MUIM_Hide);
	
	_width(data->wd_RootObject) = data->wd_Width
	    - (data->wd_innerLeft + data->wd_innerRight);
	_height(data->wd_RootObject) = data->wd_Height
	    - (data->wd_innerBottom + data->wd_innerTop);
	DoMethod(data->wd_RootObject, MUIM_Layout);
	DoMethod(data->wd_RootObject, MUIM_Show);
    }
}


static ULONG
invoke_event_handler (struct MUI_EventHandlerNode *ehn,
		      GdkEvent *event, ULONG muikey)
{
    ULONG res;

    if (ehn->ehn_Class)
	res = CoerceMethod(ehn->ehn_Class, ehn->ehn_Object,
			   MUIM_HandleEvent, GPOINTER_TO_UINT(event), muikey);
    else
	res = DoMethod(ehn->ehn_Object,
		       MUIM_HandleEvent, GPOINTER_TO_UINT(event), muikey);
    return res;
}

static void
handle_key(Object *win, GdkEventKey *event, gint mask)
{
    struct MUI_WindowData *data = muiWindowData(win);
    struct MinNode *mn;
    struct MUI_EventHandlerNode *ehn;
    ULONG res;
    ULONG muikey;

    /* Zune can handle the key itself */
    if (zune_key_translate(win, event, &muikey) == FALSE)
	return;

    /* try ActiveObject */
    if (data->wd_ActiveObject)
    {
	for (mn = data->wd_EHList.mlh_Head; mn->mln_Succ; mn = mn->mln_Succ)
	{
	    ehn = (struct MUI_EventHandlerNode *)mn;

	    if ((ehn->ehn_Object == (Object *)data->wd_ActiveObject->data) &&
		(ehn->ehn_Events & mask))
	    {
		res = invoke_event_handler(ehn, (GdkEvent *)event, muikey);
		if (res & MUI_EventHandlerRC_Eat)
		{
		    DoMethod(_app(ehn->ehn_Object), MUIM_Application_PushMethod,
			     GPOINTER_TO_UINT(ehn->ehn_Object), 3,
			     MUIM_HandleEvent, 0, MUIKEY_RELEASE);
		    return;
		}
	    }
	}
    }

    /* try DefaultObject */
    if ((data->wd_DefaultObject)
	&& !((data->wd_ActiveObject)
	     && (data->wd_DefaultObject == (Object *)data->wd_ActiveObject->data)))
    {	    
	for (mn = data->wd_EHList.mlh_Head; mn->mln_Succ; mn = mn->mln_Succ)
	{
	    ehn = (struct MUI_EventHandlerNode *)mn;

	    if ((ehn->ehn_Object == data->wd_DefaultObject) &&
		(ehn->ehn_Events & mask))
	    {
		res = invoke_event_handler(ehn, (GdkEvent *)event, muikey);
		if (res & MUI_EventHandlerRC_Eat)
		{
		    DoMethod(_app(ehn->ehn_Object), MUIM_Application_PushMethod,
			     GPOINTER_TO_UINT(ehn->ehn_Object), 3,
			     MUIM_HandleEvent, 0, MUIKEY_RELEASE);
		    return;
		}
	    }
	}
    }

    if ((event->string == NULL)
	|| (strlen(event->string) != 1))
	return;

    /* try Control Chars */
    for (mn = data->wd_CCList.mlh_Head; mn->mln_Succ; mn = mn->mln_Succ)
    {
	ehn = (struct MUI_EventHandlerNode *)mn;

	if ((tolower(ehn->ehn_Events) == tolower(event->string[0])))
	{
	    res = invoke_event_handler(ehn, (GdkEvent *)event, ehn->ehn_Flags);
	    if (res & MUI_EventHandlerRC_Eat)
	    {
		DoMethod(_app(ehn->ehn_Object), MUIM_Application_PushMethod,
			 GPOINTER_TO_UINT(ehn->ehn_Object), 3,
			 MUIM_HandleEvent, 0, MUIKEY_RELEASE);
		return;
	    }
	}
    }
    
}

/* handle non-key events
 * event : event to handle
 * mask : event mask to match corresponding to this event
 */
static void
handle_event(Object *win, GdkEvent *event, gint mask)
{
    struct MUI_WindowData *data = muiWindowData(win);
    struct MinNode *mn;
    struct MUI_EventHandlerNode *ehn;
    ULONG res;

    for (mn = data->wd_EHList.mlh_Head; mn->mln_Succ; mn = mn->mln_Succ)
    {
	ehn = (struct MUI_EventHandlerNode *)mn;

	if (ehn->ehn_Events & mask)
	{
	    res = invoke_event_handler(ehn, event, MUIKEY_NONE);
	    if (res & MUI_EventHandlerRC_Eat)
		break;
	}
    }
}

