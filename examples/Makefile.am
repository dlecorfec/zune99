OMIT_DEPENDENCIES = mui.h muimaster_protos.h

noinst_PROGRAMS = \
ttt_demo \
lay2 \
lay3 \
slider \
input1

EXTRA_DIST = \
tictactoe_mcc.h \
notif1.c \
notif2.c \
fami1.c \
lay1.c \
pages.c \
dynamic.c \
images.c

DEPS = \
	$(top_builddir)/src/muimaster/libmuimaster.la \
	$(top_builddir)/src/amilist/libamilist.la \
	$(top_builddir)/src/tagitem/libtagitem.la

LDADDS = \
$(top_builddir)/src/muimaster/libmuimaster.la \
$(top_builddir)/src/amilist/libamilist.la \
$(top_builddir)/src/tagitem/libtagitem.la \
@ZUNE_LIBS@ \
-lm

DEPENDENCIES = $(DEPS)
LDADD = $(LDADDS)
INCLUDES = -I$(top_srcdir)/include $(ZUNE_CFLAGS)



MCCLDFLAGS = -avoid-version -module
MCCLIBADD = $(LDADDS)

libTicTacToe_mcc_la_SOURCES = tictactoe.c
libTicTacToe_mcc_la_LDFLAGS = $(MCCLDFLAGS)
libTicTacToe_mcc_la_LIBADD  = $(MCCLIBADD)


lib_LTLIBRARIES = \
libTicTacToe_mcc.la


