#include <config.h>

#ifdef WITH_IMLIB

#include <zune/amitypes.h>
#include <gdk_imlib.h>
#include <gdk/gdktypes.h>
#include <imspec.h>
#include <string.h>

/*
 * load image and render it to its original size
 */
void
_zune_bitmap_load(struct MUI_ImageSpec *img, struct MUI_RenderInfo *mri)
{
    GdkImlibImage *im;
/*  g_print("zune_bitmap_load\n"); */
    im = gdk_imlib_load_image(__zune_imspec_get_path(img));
    if (!im)
    {
	g_warning("zune_bitmap_load : cannot load '%s'\n",
		  __zune_imspec_get_path(img));
	return;
    }
    gdk_imlib_render(im, im->rgb_width, im->rgb_height);
    zune_imspec_set_width(img, im->rgb_width);
    zune_imspec_set_height(img, im->rgb_height);
    img->u.bm.pixmap = gdk_imlib_move_image(im);
    gdk_pixmap_ref(img->u.bm.pixmap);
    gdk_imlib_free_pixmap(img->u.bm.pixmap);
    gdk_imlib_destroy_image(im);    
    img->u.bm.image = NULL;
}

void
_zune_bitmap_unload(struct MUI_ImageSpec *img, struct MUI_RenderInfo *mri)
{
/*  g_print("zune_bitmap_unload\n"); */
    if (img->u.bm.pixmap)
	gdk_pixmap_unref(img->u.bm.pixmap);
}

/*
 * load the imlib images
 */
void
_zune_brush_load(struct MUI_ImageSpec *img, struct MUI_RenderInfo *mri)
{
    GdkImlibImage *im;
    GdkImlibBorder border;
    STRPTR         path;
    int            path_len;

/*  g_print("zune_brush_load %p (%s)\n", img, zune_imspec_to_string(img)); */
    path = __zune_imspec_get_path(img);
    if (!path)
	return;
    path_len = strlen(path);

    im = gdk_imlib_load_image(path);
/*  g_print("imlib image %p\n", im); */
    if (!im)
    {
	g_warning("zune_brush_load : cannot load '%s'\n",
		  path);
	return;
    }
/*      g_print("bsh 0 loaded\n"); */
    zune_imspec_set_width(img, im->rgb_width);
    zune_imspec_set_height(img, im->rgb_height);
    img->u.ext.u.bsh.image[0] = im;
    border.left = border.right =
	border.top = border.bottom = img->u.ext.u.bsh.border;
    gdk_imlib_set_image_border(im, &border);

    if (path[path_len - 1] == '0')
    {
	BOOL stop = FALSE;
	int i = 1;

	while (!stop && i < BS_COUNT)
	{
	    path[path_len - 1] = '0' + i;
	    im = gdk_imlib_load_image(path);
	    if (!im)
		stop = TRUE;
	    else
	    {
/*  		g_print("bsh %d loaded\n", i); */
/*  g_print("imlib image %p\n", im); */
		img->u.ext.u.bsh.image[i] = im;
		border.left = border.right =
		    border.top = border.bottom = img->u.ext.u.bsh.border;
		gdk_imlib_set_image_border(im, &border);
	    }
	    i++;
	}

	path[path_len - 1] = '0';
    }
    return;
}

void
_zune_brush_unload (struct MUI_ImageSpec *img, struct MUI_RenderInfo *mri)
{
    int i = 0;
    GdkImlibImage *im;

/*  g_print("zune_brush_unload\n"); */
    while (img->u.ext.u.bsh.image[i] != NULL && i < BS_COUNT)
    {
	im = (GdkImlibImage *)img->u.ext.u.bsh.image[i];
/*  g_print("free img %d %p\n", i, im); */
	gdk_imlib_destroy_image(im);
	img->u.ext.u.bsh.image[i] = NULL;
	i++;
    }
}

/*
 * render the images to the current area size
 */
void
_zune_brush_render (struct MUI_ImageSpec *img, Object *obj)
{
    int i = 0;
    GdkImlibImage *im;
    struct MUI_BrushSpec *br;

/*      g_print("bsh render for imspec %p (%s), obj=%p\n", */
/*  	    img, zune_imspec_to_string(img), obj); */
    if (!obj)
	return;

    br = &img->u.ext.u.bsh;
    while (br->image[i] != NULL && i < BS_COUNT)
    {
/*  	g_print("im %d\n", i); */
	im = (GdkImlibImage *)br->image[i];
	if (!im)
	{
/*  	    g_print("null im %d\n", i); */
	    break;
	}
	

/*  g_print("scaling at %dx%d\n", br->swidth, br->sheight); */
	gdk_imlib_render(im, br->swidth, br->sheight);
	br->pixmap[i] = gdk_imlib_move_image(im);
	if (br->pixmap[i])
	{
/*    g_print("imlib rendered pixmap %p\n", br->pixmap[i]); */
	    gdk_pixmap_ref(br->pixmap[i]);
	    br->mask[i] = gdk_imlib_move_mask(im);
	    if (br->mask[i])
		gdk_pixmap_ref(br->mask[i]);
	    gdk_imlib_free_pixmap(br->pixmap[i]);
	}
	i++;
    }
}

void
_zune_brush_unrender (struct MUI_ImageSpec *img)
{
    int i = 0;

    while (img->u.ext.u.bsh.pixmap[i] != NULL && i < BS_COUNT)
    {
	gdk_pixmap_unref(img->u.ext.u.bsh.pixmap[i]);
	img->u.ext.u.bsh.pixmap[i] = NULL;
	if (img->u.ext.u.bsh.mask[i])
	{
	    gdk_pixmap_unref(img->u.ext.u.bsh.mask[i]);
	    img->u.ext.u.bsh.mask[i] = NULL;
	}
	i++;
    }
}

#endif /* WITH_IMLIB */
