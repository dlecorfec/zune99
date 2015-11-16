# Configure paths for Zune
# ripped from gdk_imlib


# Check for zune
AC_DEFUN(AM_PATH_ZUNE,
[dnl 
dnl Get the cflags and libraries from the zune-config script
dnl
AC_ARG_WITH(zune-prefix,[  --with-zune-prefix=PFX   Prefix where zune is installed (optional)],
            zune_prefix="$withval", zune_prefix="")
AC_ARG_WITH(zune-exec-prefix,[  --with-zune-exec-prefix=PFX Exec prefix where zune is installed (optional)],
            zune_exec_prefix="$withval", zune_exec_prefix="")

  if test x$zune_exec_prefix != x ; then
     zune_args="$zune_args --exec-prefix=$zune_exec_prefix"
     if test x${ZUNECONF+set} != xset ; then
        ZUNECONF=$zune_exec_prefix/bin/zune-config
     fi
  fi
  if test x$zune_prefix != x ; then
     zune_args="$zune_args --prefix=$zune_prefix"
     if test x${ZUNECONF+set} != xset ; then
        ZUNECONF=$zune_prefix/bin/zune-config
     fi
  fi

  AC_PATH_PROG(ZUNECONF, zune-config, no)
  min_zune_version=ifelse([$1], ,0.1.11,$1)
  AC_MSG_CHECKING(for Zune - version >= $min_zune_version)
  no_zune=""
  if test "$ZUNECONF" = "no" ; then
    no_zune=yes
  else
    ZUNE_CFLAGS=`$ZUNECONF $zuneconf_args --cflags`
    ZUNE_LIBS=`$ZUNECONF $zuneconf_args --libs`

    zune_major_version=`$ZUNECONF $zune_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    zune_minor_version=`$ZUNECONF $zune_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    if test "x$enable_zunetest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $ZUNE_CFLAGS"
      LIBS="$LIBS $ZUNE_LIBS"
dnl
dnl Now check if the installed ZUNE is sufficiently new. (Also sanity
dnl checks the results of zune-config to some extent
dnl
      rm -f conf.zunetest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <zune.h>

int main ()
{
  int major, minor;
  char *tmp_version;

  system ("touch conf.zunetest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_zune_version");
  if (sscanf(tmp_version, "%d.%d", &major, &minor) != 2) {
     printf("%s, bad version string\n", "$min_imlib_version");
     exit(1);
   }

    if (($zune_major_version > major) ||
        (($zune_major_version == major) && ($zune_minor_version > minor)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'zune-config --version' returned %d.%d, but the minimum version\n", $zune_major_version, $zune_minor_version);
      printf("*** of Zune required is %d.%d. If zune-config is correct, then it is\n", major, minor);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If zune-config was wrong, set the environment variable ZUNECONF\n");
      printf("*** to point to the correct copy of zune-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_zune=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])

       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_zune" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$ZUNECONF" = "no" ; then
       echo "*** The zune-config script installed by ZUNE could not be found"
       echo "*** If ZUNE was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the ZUNECONF environment variable to the"
       echo "*** full path to zune-config."
     else
       if test -f conf.zunetest ; then
        :
       else
          echo "*** Could not run Zune test program, checking why..."
          CFLAGS="$CFLAGS $ZUNE_CFLAGS"
          LIBS="$LIBS $ZUNE_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
#include <zune.h>
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding Zune or finding the wrong"
          echo "*** version of Zune. If it is not finding Zune, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means Zune was incorrectly installed"
          echo "*** or that you have moved Zune since it was installed. In the latter case, you"
          echo "*** may want to edit the zune-config script: $ZUNECONF" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     ZUNE_CFLAGS=""
     ZUNE_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(ZUNE_CFLAGS)
  AC_SUBST(ZUNE_LIBS)
  rm -f conf.zunetest
])
