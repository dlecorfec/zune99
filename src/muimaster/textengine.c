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

#include <textengine.h>
#include <renderinfo.h>
#include <image.h>
#include <pen.h>
#include <ctype.h>

#ifdef WITH_IMLIB
#include <gdk_imlib.h>
#endif

/* A bit of explanation:
 * The most important thing, after the datastructure, is the bounds
 * calculation (especially chunk width).
 * It will determine where to draw. Drawing is then straightforward.
 * From the beginning:
 * the input string is parsed line by line. Each line is formed of chunks
 * of symbols having the same rendering style. As a special case,
 * HiChar is a single letter chunk with underline style.
 * Size calculation is done by calculating for each line the width and
 * height of each chunk, then line width is the sum of widths and line height
 * is the max height. Text width will be max line width, and text height
 * will be the sum of line heights. Remember group layout.
 * Chunk width is affected by soft style of text, and maybe other params.
 * Drawing is done line by line, chunk by chunk, incrementing each time
 * the offsets with chunk width or line height.
 *
 * Italic is looking ugly, no ?
 */

typedef struct ZTextLine {
    UBYTE  align;
    GList *chunks;
    WORD   lwidth;
    WORD   lheight;
} ZTextLine;

#define ZTL_LEFT    1
#define ZTL_CENTER  2
#define ZTL_RIGHT   3

#define DEFAULT_TEXT_ALIGN ZTL_LEFT

/*-------------*/

typedef struct ZTextChunk {
    GString              *str;
    struct MUI_ImageSpec *image;
#ifdef WITH_IMLIB
    GdkImlibImage        *im;
#endif
    LONG                  dripen;
    UBYTE                 style;
    UBYTE                 longer_underline;
    WORD                  deltabearing; /* width bearing - right bearing */
    WORD                  cwidth;
    WORD                  cheight;
    WORD                  lbearing;
    WORD                  lbear2;
} ZTextChunk;

#define ZTC_STYLE_BOLD   (1<<0)
#define ZTC_STYLE_ITALIC (1<<1)
#define ZTC_STYLE_UNDERLINE (1<<2)
#define ZTC_STYLE_NORMAL (1<<3)

#define ITALIC_RATIO 5

#define NEED_LONG_UNDERLINE 10000

/*-------------*/

struct line_bounds_datas {
    ZText *text;
    GdkFont *font;
};

struct chunk_bounds_datas {
    ZTextLine *line;
    struct line_bounds_datas *lbd;
};

/*-------------*/

struct line_draw_datas {
    ZText *text;
    Object *obj;
    WORD left;
    WORD right;
    WORD top;
    WORD yoffset; /* incremented by each line draw */
};

struct chunk_draw_datas {
    ZTextLine *line;
    struct line_draw_datas *ldd;
    WORD xoffset; /* incremented by each chunk draw */
};

/*-------------*/

struct line_pos_datas {
    ZText *text;
    Object *obj;
    WORD left;
    WORD right;
    WORD *realleft;
    WORD *realright;
};

/** Variables **********************/

static GMemChunk *textMemChunk = NULL;
static GMemChunk *lineMemChunk = NULL;
static GMemChunk *chunkMemChunk = NULL;

/** Declarations **********************/

static ZTextLine *zune_text_parse_line (gchar *s, ULONG *current_style,
					gint *argtype, guchar arg);

/************************/

static void
destroy_memchunk (void)
{
    if (textMemChunk)
	g_mem_chunk_destroy(textMemChunk);
    if (lineMemChunk)
	g_mem_chunk_destroy(lineMemChunk);
    if (chunkMemChunk)
	g_mem_chunk_destroy(chunkMemChunk);
}

/************************************************************/
/* New / Destroy */
/************************************************************/

ZText *
zune_text_new (STRPTR preparse, STRPTR content, gint argtype, guchar arg)
{
    ZText *text;
    gchar **lines;
    int i;

    if (!textMemChunk)
    {
	textMemChunk = g_mem_chunk_create(ZText, 20, G_ALLOC_AND_FREE);
	lineMemChunk = g_mem_chunk_create(ZTextLine, 30, G_ALLOC_AND_FREE);
	chunkMemChunk = g_mem_chunk_create(ZTextChunk, 40, G_ALLOC_AND_FREE);
	g_atexit(destroy_memchunk);
    }

    text = g_chunk_new0(ZText, textMemChunk);
    text->style = ZTC_STYLE_NORMAL;

    if (content == NULL)
	content = "";

    lines = g_strsplit(content, "\n", 0);
    if (lines[0])
    {
	STRPTR s;

	if (preparse)
	{
	    s = g_strconcat(preparse, lines[0], NULL);
	}
	else
	{
	    s = lines[0];
	}
	if (strlen(s) > 0)
	{
	    text->lines = g_list_prepend(
		text->lines,
		zune_text_parse_line(s, &text->style, &argtype, arg));
	}
	if (preparse)
	    g_free(s);
	for (i = 1; lines[i] != NULL; i++)
	{
	    if (strlen(lines[i]) > 0)
	    {
		text->lines = g_list_prepend(
		    text->lines,
		    zune_text_parse_line(lines[i], &text->style, &argtype, arg));
	    }
	}
    }
    g_strfreev(lines);

    text->lines = g_list_reverse(text->lines);
    return text;
}

static ZTextChunk *
zune_text_chunk_new (ULONG style, LONG pen)
{
    ZTextChunk *ztc = g_chunk_new0(ZTextChunk, chunkMemChunk);

/*  g_print("zune_text_chunk_new, style %ld\n", style);     */
    ztc->style = style;
    ztc->dripen = pen;
    ztc->str = g_string_new("");
    return ztc;
}

static void
zune_text_destroy_chunk (gpointer val, gpointer dummy)
{
    ZTextChunk *chunk = (ZTextChunk *)val;
    if (chunk->str)
	g_string_free(chunk->str, TRUE);
    if (chunk->image)
	zune_imspec_free(chunk->image);
    if (chunk->im)
	gdk_imlib_kill_image(chunk->im);
    g_chunk_free(chunk, chunkMemChunk);
}

static void
zune_text_destroy_chunks (ZTextLine *line)
{
    if (!line->chunks)
	return;
    g_list_foreach(line->chunks, zune_text_destroy_chunk, NULL);
    g_list_free(line->chunks);
}

static void
zune_text_destroy_line (gpointer val, gpointer dummy)
{
    ZTextLine *line = (ZTextLine *)val;
    zune_text_destroy_chunks (line);
    g_chunk_free(line, lineMemChunk);
}

static void
zune_text_destroy_lines (ZText *text)
{
    if (!text->lines)
	return;
    g_list_foreach(text->lines, zune_text_destroy_line, NULL);
    g_list_free(text->lines);
}


void
zune_text_destroy (ZText *text)
{
    if (!text)
	return;
    zune_text_destroy_lines (text);
    g_chunk_free(text, textMemChunk);
}

/************************************************************/
/* Parsing */
/************************************************************/

#ifdef DEBUG_LINES
/* replace ESC character with '[' for display purpose */
static STRPTR
clean_string(STRPTR s)
{
    STRPTR t, u;

    u = g_strdup(s);
    t = u;
    while (*t)
    {
	if (*t == '\33')
	    *t = '[';
	t++;
    }

    return u;
}
#endif

static gchar *
parse_escape_code (ZTextLine *ztl, ZTextChunk **ztc, ULONG *current_style,
		   LONG *pen, gchar *s)
{
    switch (*s)
    {
	case 'c': /* center */
	    ztl->align = ZTL_CENTER;
	    break;
	case 'r': /* right justify */
	    ztl->align = ZTL_RIGHT;
	    break;
	case 'l': /* left justify */
	    ztl->align = ZTL_LEFT;
	    break;
	case 'n': /* normal style */
	    if (*ztc && (*ztc)->style != 0)
		*ztc = NULL;
	    *current_style = ZTC_STYLE_NORMAL;
	    break;
	case 'u': /* underline */
	    if (*ztc && !((*ztc)->style & ZTC_STYLE_UNDERLINE))
		*ztc = NULL;
	    *current_style |= ZTC_STYLE_UNDERLINE;
	    break;
	case 'b': /* bold */
	    if (*ztc && !((*ztc)->style & ZTC_STYLE_BOLD))
		*ztc = NULL;
	    *current_style |= ZTC_STYLE_BOLD;
	    break;
	case 'i': /* italic */
	    if (*ztc && !((*ztc)->style & ZTC_STYLE_ITALIC))
		*ztc = NULL;
	    *current_style |= ZTC_STYLE_ITALIC;
	    break;
	case 'I': /* image spec */
	    /*  *(s+1) = '['  */
			
	    break;
	case '-': /* disable engine */
	    if (!*ztc)
	    {
		*ztc = zune_text_chunk_new (*current_style, 0);
		ztl->chunks = g_list_append(ztl->chunks, *ztc);
		*current_style = ZTC_STYLE_NORMAL;
	    }
	    g_string_append ((*ztc)->str, s);
	    s += strlen(s);
	    break;
	default: /* some other ESC code ? */
	    if (isdigit(*s)) /* pen */
	    {
		int p = *s - '0';
		if (*ztc && ((*ztc)->dripen != p))
		    *ztc = NULL;
		*pen = p;
	    }
    }
    return s;
}

/* Parse a text line, and create text chunks.
 * The input string has no trailing '\n'.
 * Whenever an escape code is encountered, the current chunk
 * is terminated, and the escape parameters are stacked until
 * a normal character is read. Then a new chunk begins with this char.
 *
 * This is probably a function to rewrite to deal with wide chars.
 */
static ZTextLine *
zune_text_parse_line (gchar *s, ULONG *current_style, gint *argtype, guchar arg)
{
    ZTextLine *ztl = g_chunk_new0(ZTextLine, lineMemChunk);
    ZTextChunk *ztc = NULL;
    LONG pen = -1;

    g_return_val_if_fail(s != NULL, NULL);

#ifdef DEBUG_LINES
    g_print("parse line <%s>\n", clean_string(s));
#endif

    while (*s)
    {
	switch (*s)
	{
	    case '\33': /* ESC code */
		s++;
		if (*s == 0)
		    return ztl;
		s = parse_escape_code (ztl, &ztc, current_style, &pen, s);
		break;
	    default:
		if (*argtype == ZTEXT_ARG_HICHAR && arg == *s)
		{
		    /* underline this char */
		    int styleback = *current_style;

		    if (ztc)
			*current_style = ztc->style | ZTC_STYLE_UNDERLINE;
		    else
			*current_style |= ZTC_STYLE_UNDERLINE;
		    *current_style &= ~ZTC_STYLE_NORMAL;
		    ztc = zune_text_chunk_new (*current_style, pen);
		    ztl->chunks = g_list_append(ztl->chunks, ztc);
		    g_string_append_c (ztc->str, *s);
		    ztc = NULL;
		    *current_style = styleback;
		    *argtype = ZTEXT_ARG_NONE;
		    break;
		}
		else if (*argtype == ZTEXT_ARG_HICHARIDX && arg == *s)
		{
		    /* underline next char */
		    s++;
		    if (*s)
		    {
			int styleback = *current_style;
			if (ztc)
			{
			    styleback = ztc->style;
			    *current_style = ztc->style | ZTC_STYLE_UNDERLINE;
			}
			else
			    *current_style |= ZTC_STYLE_UNDERLINE;
			*current_style &= ~ZTC_STYLE_NORMAL;
			ztc = zune_text_chunk_new (*current_style, pen);
			ztl->chunks = g_list_append(ztl->chunks, ztc);
			g_string_append_c (ztc->str, *s);
			ztc = NULL;
			*current_style = styleback;			
		    }
		    *argtype = ZTEXT_ARG_NONE;
		    break;
		}

		if (!ztc)
		{
		    ztc = zune_text_chunk_new (*current_style, pen);
		    ztl->chunks = g_list_append(ztl->chunks, ztc);
		}
		g_string_append_c (ztc->str, *s);

		if (ztc->style & ZTC_STYLE_BOLD)
		{
/* bold chunks have an additional width, so we must have a chunk
 * for each character to avoid eating character interspacing
 * when writing a bold word.
 */
/*  		    g_print("single bold chunk <%c>\n", *s); */
		    ztc = NULL;
		}
		break;
	} /* switch */

	if (*s != 0) /* don't get past EOL */
	    s++;
    } /* while */
    return ztl;
}


/************************************************************/
/* Bounds */
/************************************************************/

/* trailing spaces are not correctly handled, thus the hack which
 * adds a final dot for the calculation  .
 */
static void
calculate_chunk_bounds (gpointer ch, gpointer udata)
{
    struct chunk_bounds_datas *cbd = (struct chunk_bounds_datas *)udata;
    ZTextChunk *chunk = (ZTextChunk *)ch;
    gint lbearing;
    gint rbearing;
    gint width;

    chunk->cwidth = 0;
    chunk->cheight = 0;
    if (chunk->str)
    {
        GList *next;
	GList *prev;
        GList *me = g_list_find(cbd->line->chunks, chunk);
        g_return_if_fail(me != NULL);
        next = g_list_next(me);
	prev = g_list_previous(me);

	cbd->lbd->text->style = chunk->style;
/*  	g_print("chunk <%s>\n", chunk->str->str); */
	chunk->cheight += cbd->lbd->font->ascent + cbd->lbd->font->descent;

	gdk_text_extents (cbd->lbd->font,
			  chunk->str->str, chunk->str->len,
			  &lbearing, &rbearing, &width, NULL, NULL);

	chunk->lbear2 = lbearing;

	if (!next) /* last chunk of a line */
	{
	    chunk->cwidth += rbearing;
	    if (cbd->lbd->text->style & ZTC_STYLE_ITALIC)
		chunk->cwidth += chunk->cheight / ITALIC_RATIO;
	}
	else
	{
	    chunk->cwidth += width;
	}

	if (cbd->lbd->text->style & ZTC_STYLE_BOLD)
	    chunk->cwidth += 1;

	if (cbd->lbd->text->style & ZTC_STYLE_UNDERLINE)
	{
	    chunk->deltabearing = width - rbearing;
	    if (next)
	    {
		if (((ZTextChunk *)next->data)->style != ZTC_STYLE_NORMAL
		    && (((ZTextChunk *)next->data)->style == 0
			|| ((ZTextChunk *)next->data)->style & ZTC_STYLE_UNDERLINE))
		    chunk->longer_underline = TRUE;
	    }
	}


	if (!prev && lbearing > 0) /* get rid extra space on line beginning */
	{
	    chunk->cwidth -= lbearing;
	    chunk->lbearing = lbearing;
	}
    }
    else if (chunk->image)
    {
    }
    cbd->line->lheight = MAX(cbd->line->lheight, chunk->cheight);
    cbd->line->lwidth += chunk->cwidth;
}

static void
calculate_line_bounds (gpointer l, gpointer udata)
{
    struct chunk_bounds_datas data;
    struct line_bounds_datas *lbd = (struct line_bounds_datas *)udata;
    ZTextLine *line = (ZTextLine *)l;

    line->lwidth = 0;
    line->lheight = 0;
    data.line = line;
    data.lbd = lbd;
    g_list_foreach(line->chunks, calculate_chunk_bounds, &data);
    lbd->text->height += line->lheight;
    lbd->text->width = MAX(lbd->text->width, line->lwidth);
}

void
zune_text_get_bounds (ZText *text, GdkFont *font)
{
    struct line_bounds_datas data;

    g_return_if_fail(text != NULL);
    g_return_if_fail(font != NULL);

    text->width = 0;
    text->height = 0;
    text->align = DEFAULT_TEXT_ALIGN;
    text->style = ZTC_STYLE_NORMAL;
    text->dripen = 0;
    data.font = font;
    data.text = text;

    g_list_foreach(text->lines, calculate_line_bounds, &data);
}

/************************************************************/
/* Drawing */
/************************************************************/

#ifdef DEBUG_ITALIC_BUFFER
static void
debug_italic_buffer (guchar *t, guint width)
{
    int i;
    for (i = 0; i < width; i++)
    {
	if (t[0] == 0 && t[1] == 255 && t[2] == 0)
	    g_print(" ");
	else
	    g_print("*");
	t += 3;
    }
    g_print("\n");
}
#endif

#ifdef WITH_IMLIB

static void
shear_image (GdkImlibImage *im)
{
    int y;
    int i;
    guchar *linedata = g_malloc(3 * sizeof(guchar) * im->rgb_width);

    for (y = 0; y < im->rgb_height; y++)
    {
	guchar *t = linedata;
	/* shear to right from y/ITALIC_RATIO */
	/* init sheared line to transparent */
	for (i = 0; i < im->rgb_width; i++)
	{
	    *t++ = 0;
	    *t++ = 255;
	    *t++ = 0;
	}
	/* shear to buffer */
	memcpy(linedata + (y / ITALIC_RATIO) * 3,
	       im->rgb_data + 3 * im->rgb_width * (im->rgb_height - y - 1),
	       3 * (im->rgb_width - y / ITALIC_RATIO));
#ifdef DEBUG_ITALIC_BUFFER
	debug_italic_buffer(linedata, im->rgb_width);
#endif
	/* copy buffer back to image */
	memcpy(im->rgb_data + 3 * im->rgb_width * (im->rgb_height - y - 1),
	       linedata,
	       3 * im->rgb_width);
    }
    g_free(linedata);
}

static GdkImlibImage *
paste_italized (GdkDrawable *src, gint src_width, gint src_height,
		GdkDrawable *dest, gint dest_x, gint dest_y)
{
    GdkImlibImage *im;
    GdkImlibColor color;

    im = gdk_imlib_create_image_from_drawable(
	src, NULL, 0, 0,
	src_width, src_height);

    shear_image(im);
    gdk_imlib_changed_image(im);
    color.r = 0;
    color.g = 255;
    color.b = 0;
    gdk_imlib_set_image_shape(im, &color);
	    
    gdk_imlib_paste_image(im, dest, dest_x, dest_y,
			  src_width, src_height);
    return im;
}
#endif


static void
prepare_italic_pixmap (GdkDrawable **drawable, GdkGC **gc, Object *obj,
		       ZTextChunk *chunk)
{
	GdkColor shape;
	GdkColor oldc;
	GdkGCValues val;

	gdk_gc_get_values(*gc, &val);
	oldc = val.foreground;

	*drawable = gdk_pixmap_new(_window(obj),
				  chunk->cwidth + (chunk->cheight / ITALIC_RATIO),
				  chunk->cheight, -1);
	*gc = gdk_gc_new(_window(obj));
	/* TODO : check that gc fg is not green */
	shape.red = 0;
	shape.green = 0xffff;
	shape.blue = 0;
	gdk_colormap_alloc_color(
	    muiRenderInfo(obj)->mri_Colormap, &shape, FALSE, TRUE);
	gdk_gc_set_foreground(*gc, &shape);
	gdk_draw_rectangle(*drawable, *gc, TRUE, 0, 0,
			   chunk->cwidth + (chunk->cheight / ITALIC_RATIO),
			   chunk->cheight);
	gdk_gc_set_foreground(*gc, &oldc);
	gdk_colormap_free_colors(muiRenderInfo(obj)->mri_Colormap, &shape, 1);
}

/* Render only the first time, then keep the imlib image hanging around
 * for a quick paste until text destruction.
 */
static void
draw_italic (GdkDrawable *drawable, GdkGC *gc, struct chunk_draw_datas *cdd,
	     Object *obj, ZTextChunk *chunk, gint xback)
{
#ifdef WITH_IMLIB
    if (chunk->im)
	gdk_imlib_paste_image(chunk->im, _window(obj),
			      xback, cdd->ldd->top + cdd->ldd->yoffset,
			      chunk->cwidth + (chunk->cheight / ITALIC_RATIO),
			      chunk->cheight);
    else
#endif
    {
	chunk->im = paste_italized(
	    drawable,
	    chunk->cwidth + (chunk->cheight / ITALIC_RATIO),
	    chunk->cheight,
	    _window(obj),
	    xback, cdd->ldd->top + cdd->ldd->yoffset);
	gdk_pixmap_unref(drawable);
	gdk_gc_unref(gc);
    }
}


static void
draw_text_string (struct chunk_draw_datas *cdd, ZTextChunk *chunk)
{
    Object *obj = cdd->ldd->obj;
    GdkDrawable *drawable = _window(obj);
    GdkGC *gc = _rp(obj);
    gint xtext = cdd->ldd->left + cdd->xoffset - chunk->lbearing;
    gint ytext = cdd->ldd->top + cdd->ldd->yoffset + _font(obj)->ascent;
    gint xback = 0;
    gint yback = 0;

    cdd->ldd->text->style = chunk->style;
    if (chunk->dripen != -1)
    {
	cdd->ldd->text->dripen = chunk->dripen;
	SetAPen(gc, _dri(obj)->dri_Pens[cdd->ldd->text->dripen]);
    }

    if (cdd->ldd->text->style & ZTC_STYLE_ITALIC)
    {
	xback = xtext;
	xtext = 0;
	yback = ytext;
	ytext = _font(obj)->ascent;

	if (chunk->im)
	{
	    draw_italic (drawable, gc, cdd, obj, chunk, xback);
	    return;
	}
	prepare_italic_pixmap(&drawable, &gc, obj, chunk);
    }

    gdk_draw_text(drawable, _font(obj), gc,
		  xtext,
		  ytext,
		  chunk->str->str, chunk->str->len);

    if (cdd->ldd->text->style & ZTC_STYLE_UNDERLINE)
    {
	int endu = xtext + chunk->lbear2
	    + MIN(chunk->cwidth, chunk->cwidth - chunk->deltabearing) - 1;

	if (chunk->longer_underline)
	    endu = xtext + chunk->lbear2 + chunk->cwidth;

	gdk_draw_line(drawable, gc, xtext + chunk->lbear2,
		      ytext + MIN(_font(obj)->descent - 1, 1),
		      endu, ytext + MIN(_font(obj)->descent - 1, 1));
    }
    if (cdd->ldd->text->style & ZTC_STYLE_BOLD)
    {
	gdk_draw_text(drawable, _font(obj), gc, xtext + 1, ytext,
		      chunk->str->str, chunk->str->len);
    }

    if (cdd->ldd->text->style & ZTC_STYLE_ITALIC)
    {
	draw_italic (drawable, gc, cdd, obj, chunk, xback);
    }
}

/* 
 * first, you have to hide the real graphic environment, to draw
 * either on the real place, or in a new pixmap if doing italic.
 * Means hiding gc, drawable and origin behind new variables.
 * How is done italic ?
 * the normal text is rendered to a pixmap with
 * a special background color (which will be 'transparent').
 * The pixmap is converted to an imlib image. This image is sheared
 * (lines are scrolled right according to their y-coordinate)
 * Then image is pasted to the window.
 */
static void
draw_text_chunk (gpointer ch, gpointer d)
{
    struct chunk_draw_datas *cdd = (struct chunk_draw_datas *)d;
    ZTextChunk *chunk = (ZTextChunk *)ch;

    if (chunk->str)
    {
	draw_text_string(cdd, chunk);
    }
    else if (chunk->image)
    {
    }
    cdd->xoffset += chunk->cwidth;
}


static void
draw_text_line (gpointer l, gpointer d)
{
    struct line_draw_datas *ldd = (struct line_draw_datas *)d;
    ZTextLine *line = (ZTextLine *)l;
    struct chunk_draw_datas data;

    data.line = line;
    data.xoffset = 0;
    data.ldd = ldd;

    if ((ldd->right - ldd->left + 1) < line->lwidth)
	g_warning("draw_text_line: allocated space is smaller than needed space");

    if (line->align > 0)
	ldd->text->align = line->align;

    if (ldd->text->align == ZTL_CENTER)
        data.xoffset = ((ldd->right - ldd->left + 1) - line->lwidth) / 2;
    else if (ldd->text->align == ZTL_RIGHT)
        data.xoffset = (ldd->right - ldd->left + 1) - line->lwidth;

    g_list_foreach(line->chunks, draw_text_chunk, &data);    
    ldd->yoffset += line->lheight;
}

void
zune_text_draw (ZText *text, Object *obj, WORD left, WORD right, WORD top)
{
    struct line_draw_datas data;

    text->align = ZTL_RIGHT;
    text->style = ZTC_STYLE_NORMAL;
    text->dripen = 0;
    data.text = text;
    data.obj = obj;
    data.left = left;
    data.right = right;
    data.top = top;
    data.yoffset = 0;
    g_list_foreach(text->lines, draw_text_line, &data);
}

