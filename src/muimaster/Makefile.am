#AUTOMAKE_OPTIONS = no-dependencies

OMIT_DEPENDENCIES = mui.h muimaster_protos.h

lib_LTLIBRARIES = libmuimaster.la

#libmuimaster_la_LDFLAGS = -version-info 0:0:0

libmuimaster_la_SOURCES = \
aboopsi.c \
aclass.c \
aobject.c \
arootclass.c \
asetget.c \
events_gdk.c \
file.c \
font.c \
image_gdk.c \
imspec.c \
mbuiltins.c \
mclass.c \
mclipping.c \
mdynamic.c \
mframes.c \
minit.c \
mkey.c \
mlayout.c \
mobject.c \
mpen.c \
mprefs.c \
mprefsio.c \
pixmap_imlib.c \
stringset.c \
textengine.c \
uproperties.c \
window_gdk.c \
zapplication.c \
zarea.c \
zconfigdata.c \
zdataspace.c \
zfamily.c \
zgroup.c \
zimage.c \
znotify.c \
znumeric.c \
zrectangle.c \
zslider.c \
ztext.c \
zwindow.c

INCLUDES = \
	-D_ZUNE_CORE \
	-DZUNEDATADIR=\"$(datadir)/zune\" \
	-DZUNELIBDIR=\"$(libdir)/zune\" \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/include/priv \
	 $(ZUNE_CFLAGS)

