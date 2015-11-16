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
#include <zune/boopsi.h>
#include <gdk/gdk.h>
#include <applicationdata.h>
#include <event_impl.h>
#include <window_impl.h>

/*
 * event handling based on gtkmain.c
 */


static Object *
zune_get_event_object (GdkEvent *event)
{
  Object *widget;

  widget = NULL;
  if (event && event->any.window)
    gdk_window_get_user_data (event->any.window, (void **) &widget);
  
  return widget;
}

void 
__zune_main_do_event (GdkEvent *event)
{
    GdkEvent *next_event;
    Object   *event_object;

    /* If there are any events pending then get the next one.
     */
    next_event = gdk_event_peek ();
  
    /* Try to compress enter/leave notify events. These event
     *  pairs occur when the mouse is dragged quickly across
     *  a window with many buttons (or through a menu). Instead
     *  of highlighting and de-highlighting each widget that
     *  is crossed it is better to simply de-highlight the widget
     *  which contained the mouse initially and highlight the
     *  widget which ends up containing the mouse.
     */
    if (next_event)
	if (((event->type == GDK_ENTER_NOTIFY) ||
	     (event->type == GDK_LEAVE_NOTIFY)) &&
	    ((next_event->type == GDK_ENTER_NOTIFY) ||
	     (next_event->type == GDK_LEAVE_NOTIFY)) &&
	    (next_event->type != event->type) &&
	    (next_event->any.window == event->any.window))
	{
	    /* Throw both the peeked copy and the queued copy away 
	     */
	    gdk_event_free (next_event);
	    next_event = gdk_event_get ();
	    gdk_event_free (next_event);
	
	    return;
	}

    if (next_event)
	gdk_event_free (next_event);

    /* Find the widget which got the event. We store the widget
     *  in the user_data field of GdkWindow's.
     *  Ignore the event if we don't have a widget for it, except
     *  for GDK_PROPERTY_NOTIFY events which are handled specialy.
     *  Though this happens rarely, bogus events can occour
     *  for e.g. destroyed GdkWindows. 
     */
    event_object = zune_get_event_object (event);
    if (!event_object)
    {
	/* To handle selection INCR transactions, we select
	 * PropertyNotify events on the requestor window and create
	 * a corresponding (fake) GdkWindow so that events get
	 * here. There won't be a widget though, so we have to handle
	 * them specially
	 */
	/* nothing for zune yet */
	return;
    }
  
    /* Push the event onto a stack of current events for
     * gtk_current_event_get().
     */
  

    /*
     * Let window object process event
     */
    _zune_window_event(event_object, event);
}


/*
 * Wait and process a single event.
 */
void
__application_wait_event (struct MUI_ApplicationData *data,
			gboolean may_block)
{
    if (g_main_is_running (data->app_MainLoop))
    {
/*  	g_main_run (data->app_MainLoop); */
	g_main_iteration(may_block);
    }
}

