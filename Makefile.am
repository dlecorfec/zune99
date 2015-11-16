
SUBDIRS = include src examples doc

bin_SCRIPTS = zune-config

pkgdata_DATA = \
global.prefs

EXTRA_DIST =	\
	HACKING		\
	zune.spec.in	\
	zune.m4         \
	global.prefs

dist-hook:
	cp zune.spec $(distdir)

m4datadir = $(datadir)/aclocal
m4data_DATA = zune.m4

.PHONY: release snapshot

release:
	find . -name .deps | xargs rm -rf
	$(MAKE) distcheck

snapshot:
	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`

