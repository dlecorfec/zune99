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
#include <gdk/gdk.h>
#include <areadata.h>
#include <renderinfo.h>

static GSList *clip_list = NULL;
static GMemChunk *rectChunk = NULL;

static void
__zune_clipping_destroy (void)
{
    g_mem_chunk_destroy(rectChunk);
}

void
__zune_clipping_init (void)
{
    rectChunk = g_mem_chunk_create(GdkRectangle, 10, G_ALLOC_AND_FREE);
    g_atexit(__zune_clipping_destroy);
}


static GdkRectangle *
rectangle_new (WORD left, WORD top, WORD width, WORD height)
{
    GdkRectangle *rect = g_chunk_new0(GdkRectangle, rectChunk);
    rect->x = left;
    rect->y = top;
    rect->width = width;
    rect->height = height;
/*      g_print("rect %p\n", rect); */
    return rect;
}

static void
rectangle_destroy (GdkRectangle *rect)
{
/*  g_print("g_chunk_free %p %p\n", rect, rectChunk); */
    g_chunk_free(rect, rectChunk);
}


APTR
MUI_AddClipping (struct MUI_RenderInfo *mri, WORD left, WORD top,
		 WORD width, WORD height)
{
    GdkRectangle *rect;

    g_return_val_if_fail(mri != NULL, NULL);

    rect = rectangle_new(left, top, width, height);

    clip_list = g_slist_prepend(clip_list, rect);
/*  g_print("clip list = %p\n", clip_list); */
    gdk_gc_set_clip_rectangle(mri->mri_RastPort, rect);
    return clip_list;
}


/*
 * Remove current clip, then set previous clip or remove any clipping
 */
VOID
MUI_RemoveClipping (struct MUI_RenderInfo *mri, APTR handle)
{
    GSList *node = (GSList *)handle;

    g_return_if_fail(mri != NULL);
    g_return_if_fail(handle != NULL);

/*  g_print("remove clip %p, num %d\n", handle, g_slist_length(clip_list)); */

    clip_list = g_slist_remove_link(clip_list, node);

    if (clip_list)
	gdk_gc_set_clip_rectangle(mri->mri_RastPort, (GdkRectangle *)clip_list->data);
    else
	gdk_gc_set_clip_rectangle(mri->mri_RastPort, NULL);

    rectangle_destroy((GdkRectangle *)node->data);
    g_slist_free_1(node);
}
