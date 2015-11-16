pkginclude_HEADERS = \
amilist.h \
amitypes.h \
boopsi.h \
classes.h \
hooks.h \
library.h \
mccheader.c \
premui.h \
preprotos.h \
rootclass.h \
tagitem.h \
zune.h \
zune_common.h

install-data-hook:
	if [ x$(DESTDIR) = x ]; then \
	   if [ -f $(srcdir)/mui.h ]; then \
	   $(INSTALL_DATA) $(srcdir)/mui.h $(pkgincludedir)/mui.h; fi; \
	   if [ -f $(srcdir)/muimaster_protos.h ]; then \
	   $(INSTALL_DATA) $(srcdir)/muimaster_protos.h \
	   $(pkgincludedir)/muimaster_protos.h; fi \
	fi
