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
#include <builtin.h>

static ULONG rootDispatcher (Class *cl, Object *o, Msg msg);

static Class rootclass =
{
    { { NULL, NULL, 0, 0, ROOTCLASS }, rootDispatcher, NULL },
    0,		/* reserved */
    NULL,	/* No superclass */
    (ClassID)ROOTCLASS,  /* ClassID */

    0, 0,	/* No offset and size */

    0,		/* UserData */
    0,		/* SubClassCount */
    0,		/* ObjectCount */
    0,		/* Flags */
    NULL,       /* memchunk */
};


void
_zune_add_root_class (void)
{
/*  g_print("adding rootclass\n"); */
    if (!GetPublicClass(ROOTCLASS))
	AddClass(&rootclass);
}


static ULONG
rootDispatcher (Class *cl, Object *o, Msg msg)
{
    ULONG retval = 0;

    switch (msg->MethodID)
    {
    case OM_NEW:
	/* Called at beginning of constructor, allocate memory for object.
	 */
	cl = _OBJECT(o)->o_Class;

	retval = (ULONG)g_mem_chunk_alloc0(cl->cl_MemChunk);
	retval = (ULONG) BASEOBJECT(retval);
	OCLASS(retval) = cl;
	break;

    case OM_DISPOSE:
	/* Speicher freigeben. Aufrufer ist verantwortlich,
	   dass bereits alles andere freigegeben wurde ! */
	cl = _OBJECT(o)->o_Class;
/*  g_print("rootDispatcher: DISPOSE: obj=%p cl=%p %s, clmc=%p\n", o, cl, */
/*  	cl->cl_ID ? cl->cl_ID : "", cl->cl_MemChunk); */
	g_mem_chunk_free(cl->cl_MemChunk, _OBJECT(o));
	break;

#ifdef REALAROSSTUFF /* unused in Zune */
    case OM_ADDTAIL:
	/* Fuege <o> an Liste an. */
	AddTail (((struct opAddTail *)msg)->opat_List,
		 (struct Node *) _OBJECT(o));
	break;

    case OM_REMOVE:
	/* Entferne Object aus der Liste */
	Remove ((struct Node *) _OBJECT(o));
	break;
#endif
    default:
	/* Ignore */
	break;

    } /* switch */

    return (retval);
}

