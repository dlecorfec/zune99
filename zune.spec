# Note that this is NOT a relocatable package
%define ver      0.1.14
%define rel      1
%define prefix   /usr

Summary: A free MUI (Magic User Interface) clone
Name: zune
Version: %ver
Release: %rel
Copyright: LGPL
Group: X11/Libraries
Source: ftp://ftp.notyetsorry.org/pub/zune/zune-%{ver}.tar.gz
BuildRoot: /var/tmp/zune-%{PACKAGE_VERSION}-root
Packager: Reez <dlc@freesurf.fr>
URL: http://www.notyetsorry.org
Requires: imlib

Docdir: %{prefix}/doc

%description
Zune is a clone of the most popular GUI toolkit on Amiga, MUI.
It has nice features like builtin themability and powerful layout.

%package devel
Summary: Zune headers, static libraries and documentation
Group: X11/Libraries
Requires: zune

%description devel
Static libraries and header files for the Zune libraries, which are
available as public libraries.  Zune is a GUI toolkit / framework
to create applications, with the MUI API.

%changelog
* Mon Aug 2 1999 David Le Corfec <dlc@freesurf.fr>
- created from gtk+-1.2.3 spec file. Removed lines dealing with info files.

%prep
%setup

%build
# Needed for snapshot releases.
if [ ! -f configure ]; then
  CFLAGS="$RPM_OPT_FLAGS" ./autogen.sh --prefix=%prefix
else
  CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%prefix
fi

if [ "$SMP" != "" ]; then
  (make "MAKE=make -k -j $SMP"; exit 0)
  make
else
  make
fi

%install
rm -rf $RPM_BUILD_ROOT

make prefix=$RPM_BUILD_ROOT%{prefix} install

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%post devel

%preun devel

%files
%defattr(-, root, root)

%doc AUTHORS COPYING ChangeLog NEWS README TODO
%{prefix}/lib/libmuimaster.so.*
%{prefix}/lib/libamilist.so.*
%{prefix}/lib/libtagitem.so.* 

%files devel
%defattr(-, root, root)

%{prefix}/lib/lib*.so
%{prefix}/lib/*a
%{prefix}/include/*
%{prefix}/share/aclocal/*
%{prefix}/bin/*

