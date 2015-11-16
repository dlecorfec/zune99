#ifndef __WINDOW_IMPL_H__
#define __WINDOW_IMPL_H__

#include <zune/amitypes.h>

/*
 * low level stuff for window handling
 */

struct MUI_RenderInfo;
struct MUI_WindowData;


BOOL
_zune_renderinfo_setup (struct MUI_RenderInfo *mri);

void
_zune_renderinfo_cleanup (struct MUI_RenderInfo *mri);

void
_zune_renderinfo_show (struct MUI_RenderInfo *mri);

void
_zune_renderinfo_hide (struct MUI_RenderInfo *mri);

/* low level open */
gboolean
_zune_window_open (struct MUI_WindowData *data);

/* low level open */
void
_zune_window_close (struct MUI_WindowData *data);

/* low level resize */
void
_zune_window_resize (struct MUI_WindowData *data);

/* get default event mask */
gint
_zune_window_get_default_events (void);

/* add event from mask to window */
void
_zune_window_add_events (struct MUI_WindowData *data, gint mask);

/* rem events from a window */
void
_zune_window_remove_events (struct MUI_WindowData *data, gint mask);

/* process an event got in this window */
gint
_zune_window_event(Object *win, GdkEvent *event);


void
_zune_focus_new (Object *obj);

void
_zune_focus_destroy (Object *obj);


#endif
