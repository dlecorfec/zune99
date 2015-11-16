#include <zunepriv.h>
#include <prefs.h>
#include <font.h>

GdkFont *
zune_font_get (LONG preset)
{
/*  g_print("zune_font_get %ld\n", preset); */
    if ((preset <= MUIV_Font_Inherit) && (preset >= MUIV_Font_NegCount))
    {
	g_return_val_if_fail(preset <= 0, NULL);
	return __zprefs.fonts[-preset];
    }
    return (GdkFont *)preset;
}

void
zune_font_replace (GdkFont **font, STRPTR fontname)
{
/*  g_print("replacing preset %ld with font %s\n", preset, fontname); */

    if (*font)
	gdk_font_unref(*font);
    if (fontname != NULL && strlen(fontname) > 0)
    {
	*font = gdk_font_load(fontname);
	if (*font)
	{
/*  	    g_print("font loaded\n"); */
	    return;
	}
/*  	g_print("cannot load font\n"); */
    }
    if (__zprefs.fonts[-MUIV_Font_Normal])
    {
	if (fontname != NULL && strlen(fontname) > 0)
	    g_warning("Cannot load font '%s', fallback to window font\n",
		      fontname);
	*font = gdk_font_ref(__zprefs.fonts[-MUIV_Font_Normal]);
    }
    else
    {
	g_warning("Cannot load font '%s', trying 'fixed'\n", fontname);
	*font = gdk_font_load("fixed");
    }
}

