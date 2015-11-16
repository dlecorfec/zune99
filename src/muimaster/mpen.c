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
#include <stdio.h>

#include <zune/zune_common.h>
#include <pen.h>
#include <image.h>

#include <areadata.h>
#include <renderinfo.h>

/* get a pixel suitable for GDK drawing, out of a PenSpec.
 * Call only at MUIM_Setup !
 */
LONG
MUI_ObtainPen (struct MUI_RenderInfo *mri,struct MUI_PenSpec *spec,ULONG flags)
{
    g_return_val_if_fail(spec != NULL, 0);
    g_return_val_if_fail(mri != NULL, 0);
    g_return_val_if_fail(mri->mri_Colormap != NULL, 0);
    g_return_val_if_fail(mri->mri_Visual != NULL, 0);

    zune_penspec_setup (mri, spec);

    switch (spec->ps_penType)
    {
	case PST_MUI:
	    g_return_val_if_fail(_between(0, spec->ps_mui, MPEN_COUNT - 1), -1);
	    return mri->mri_Pens[spec->ps_mui];
	    break;
	case PST_CMAP:
	    return spec->ps_cmap;
	    break;
	case PST_RGB:
	    if (!spec->ps_rgbColor.pixel)
		g_print("rgb pen %p (%s) may not be allocated, or black\n",
			spec, zune_gdkcolor_to_string(&spec->ps_rgbColor));
	    return spec->ps_rgbColor.pixel;
	    break;
    }
    return -1;
}

VOID
MUI_ReleasePen (struct MUI_RenderInfo *mri, LONG pen)
{
    /* a do nothing stub ... hopefully gdk_colormap_unref should free
     * everything for us on cleanup ...
     */
/* Oops, cannot unref the cmap. Thus need to do something else to free colors */
}

/* call at pen dispose time. It doesnt free the pen itself, only the
 * datas contained in pen (useful for static penspecs)
 */
void
zune_penspec_destroy_content (struct MUI_PenSpec *spec)
{
    g_return_if_fail(spec != NULL);

    if (spec->ps_penType == PST_RGB && spec->ps_rgbText)
    {
	g_free(spec->ps_rgbText);
	spec->ps_rgbText = NULL;
    }
}

/* call this at MUIM_Setup */
void
zune_penspec_setup (struct MUI_RenderInfo *mri, struct MUI_PenSpec *spec)
{
    g_return_if_fail(mri != NULL);
    g_return_if_fail(spec != NULL);
    g_return_if_fail(mri->mri_Colormap != NULL);
    g_return_if_fail(mri->mri_Visual != NULL);

    if (spec->ps_penType == PST_RGB)
    {
	gdk_colormap_alloc_color(mri->mri_Colormap, &spec->ps_rgbColor,
				 FALSE, TRUE);
/*  	g_print("zune_penspec_setup rgb %p (%s) = %lx\n", */
/*  	spec, zune_gdkcolor_to_string(&spec->ps_rgbColor), */
/*  	spec->ps_rgbColor.pixel); */
    }
}

/* call this at MUIM_Cleanup */
void
zune_penspec_cleanup (struct MUI_RenderInfo *mri, struct MUI_PenSpec *spec)
{
    g_return_if_fail(mri != NULL);
    g_return_if_fail(spec != NULL);
    g_return_if_fail(mri->mri_Colormap != NULL);
    g_return_if_fail(mri->mri_Visual != NULL);

    if (spec->ps_penType == PST_RGB)
    {
	gdk_colormap_free_colors(mri->mri_Colormap,
				 &spec->ps_rgbColor, 1);
	spec->ps_rgbColor.pixel = 0;
    }
}

/* dump a penspec */
STRPTR
zune_penspec_to_string(struct MUI_PenSpec *spec)
{
    static char buf[1000];

    g_return_val_if_fail(spec != NULL, NULL);
    switch (spec->ps_penType)
    {
	case PST_MUI:
	    g_return_val_if_fail(_between(0, spec->ps_mui, MPEN_COUNT - 1), NULL);
	    g_snprintf(buf, 1000, "m|%d", spec->ps_mui);
	    return buf;
	case PST_CMAP:
	    g_snprintf(buf, 1000, "c|%" ULONG_FMT, spec->ps_cmap);
	    return buf;
	case PST_RGB:
	    if (spec->ps_rgbText)
	    {
		g_snprintf(buf, 1000, "t|%s", spec->ps_rgbText);
	    }
	    else
	    {
		g_snprintf(buf, 1000, "r|%s",
			   zune_gdkcolor_to_string(&spec->ps_rgbColor));
	    }
	    return buf;
    }
    return NULL;
}

STRPTR
zune_gdkcolor_to_string(GdkColor *rgb)
{
    static char buf[16];

    g_return_val_if_fail(rgb != NULL, NULL);
    g_snprintf(buf, 16, "%04hx,%04hx,%04hx",
	       rgb->red, rgb->green, rgb->blue);
    return buf;
}

/* it's legal to pass a NULL string here */
/* match with zune_penspec_destroy_content() to release pen resources */
/* between these calls, use zune_penspec_setup/zune_penspec_cleanup */
void
zune_string_to_penspec(STRPTR str, struct MUI_PenSpec *spec)
{    
    ULONG tmp;

    g_return_if_fail(spec != NULL);

    if (!str)
	return;

    switch (str[0])
    {
	case 'm':
	    sscanf(str+2, "%" ULONG_FMT, &tmp);
	    g_return_if_fail(_between(0, spec->ps_mui, MPEN_COUNT - 1));
	    spec->ps_penType = PST_MUI;
	    spec->ps_mui = tmp;
	    return;
	case 'c':
	    sscanf(str+2, "%" ULONG_FMT, &tmp);
	    spec->ps_penType = PST_CMAP;
	    spec->ps_cmap = tmp;
	    return;
	case 'r':
	{
	    spec->ps_penType = PST_RGB;
	    zune_string_to_gdkcolor(str + 2, &spec->ps_rgbColor);
	    return;
	}
	case 't':
/*  	    g_print("zune_string_to_penspec: got text %s\n", str + 2); */
	    spec->ps_rgbText = g_strdup(str+2);
	    spec->ps_penType = PST_RGB;
	    gdk_color_parse(str+2, &spec->ps_rgbColor);
    }
    return;
}


void
zune_string_to_gdkcolor (STRPTR str, GdkColor *rgb)
{
    gushort rgbshort[3];

    g_return_if_fail(rgb != NULL);

    if (!str)
	return;
    
    sscanf(str, "%hx,%hx,%hx",
	   rgbshort, rgbshort+1, rgbshort+2);
    rgb->red = rgbshort[0];
    rgb->green = rgbshort[1];
    rgb->blue = rgbshort[2];
}


void
SetAPen (GdkGC *gc, gulong pen)
{
    GdkColor color;
    g_return_if_fail(gc != NULL);

    color.pixel = pen;
    gdk_gc_set_foreground(gc, &color);
}

void
SetBPen (GdkGC *gc, gulong pen)
{
    GdkColor color;
    g_return_if_fail(gc != NULL);

    color.pixel = pen;
    gdk_gc_set_background(gc, &color);
}

