#ifndef __ZUNEKEY_H__
#define __ZUNEKEY_H__

#include <zune/classes.h>
#include <gdk/gdktypes.h>
#include <muikey.h>


BOOL zune_key_translate (Object *win, GdkEventKey *event, ULONG *muikey);

void zune_keyspec_parse (ZuneKeySpec *spec);


#endif
