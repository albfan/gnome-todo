dnl -*- mode: m4 -*-
AC_PREREQ([2.69])
AC_INIT([gnome-todo],[3.22.0],[https://bugzilla.gnome.org/enter_bug.cgi?product=gnome-todo],[gnome-todo])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([1.11.1 foreign -Wno-portability dist-xz no-dist-gzip tar-ustar subdir-objects])
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])

# Releases do not have git directories
AX_IS_RELEASE([git-directory])

## don't rerun to this point if we abort
AC_CACHE_SAVE

# We require additional macros later on
AX_REQUIRE_DEFINED([APPSTREAM_XML])
AX_REQUIRE_DEFINED([IT_PROG_INTLTOOL])

# Add debug symbol
AX_CHECK_ENABLE_DEBUG([yes], [GNOME_TODO_ENABLE_DEBUG])

AC_ARG_ENABLE([tests],
              AS_HELP_STRING([--enable-tests],
                             [Enable always building tests during 'make all' [default=no]]),
              [enable_tests=$enableval], [enable_tests=yes])
AM_CONDITIONAL([ENABLE_TESTS], test "$enable_tests" = "yes")

AC_ARG_ENABLE(installed-tests,
              AS_HELP_STRING([--enable-installed-tests],
                             [Enable installation of some test cases]),
              [enable_installed_tests=$enableval], [enable_installed_tests=yes])
AM_CONDITIONAL([ENABLE_INSTALLED_TESTS], test "$enable_installed_tests" = "yes")

if test "$enable_installed_tests" = "yes"; then
  AC_SUBST(installed_test_metadir, [${datadir}/installed-tests/]AC_PACKAGE_NAME)
  AC_SUBST(installed_testdir, [${libexecdir}/installed-tests/]AC_PACKAGE_NAME)
fi

# Enable compiler warnings
AX_COMPILER_FLAGS([GNOME_TODO_WARN_CFLAGS], [GNOME_TODO_WARN_LDFLAGS])

AC_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_SED

# enable libtool
LT_PREREQ([2.2.6])
LT_INIT([disable-static])

## don't rerun to this point if we abort
AC_CACHE_SAVE

PKG_PROG_PKG_CONFIG

GLIB_GSETTINGS

dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************
IT_PROG_INTLTOOL([0.40.6])

GETTEXT_PACKAGE=AC_PACKAGE_TARNAME
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])

AS_IF([test "x$USE_NLS" != "xno"],
      [AC_DEFINE([ENABLE_NLS], [1], [Native language support is enabled])])

dnl ================================================================
dnl GObject introspection support
dnl ================================================================
GOBJECT_INTROSPECTION_CHECK([1.42.0])

dnl ************************************
dnl *** Enable lcov coverage reports ***
dnl ************************************

AC_ARG_ENABLE(gcov,
  AS_HELP_STRING([--enable-gcov],
		 [Enable gcov]),
  [use_gcov=$enableval], [use_gcov=yes])

if test "x$use_gcov" = "xyes"; then
  dnl we need gcc:
  if test "$GCC" != "yes"; then
    AC_MSG_ERROR([GCC is required for --enable-gcov])
  fi

  dnl Check if ccache is being used
  AC_CHECK_PROG(SHTOOL, shtool, shtool)
  case `$SHTOOL path $CC` in
    *ccache*[)] gcc_ccache=yes;;
    *[)] gcc_ccache=no;;
  esac

  if test "$gcc_ccache" = "yes" && (test -z "$CCACHE_DISABLE" || test "$CCACHE_DISABLE" != "1"); then
    AC_MSG_ERROR([ccache must be disabled when --enable-gcov option is used. You can disable ccache by setting environment variable CCACHE_DISABLE=1.])
  fi

  lcov -v
  genhtml -v
  ltp_version_list="1.6 1.7 1.8 1.10 1.11 1.12"
  AC_CHECK_PROG(LTP, lcov, lcov)
  AC_CHECK_PROG(LTP_GENHTML, genhtml, genhtml)

  if test "$LTP"; then
    AC_CACHE_CHECK([for ltp version], glib_cv_ltp_version, [
      glib_cv_ltp_version=invalid
      ltp_version=`$LTP -v 2>/dev/null | $SED -e 's/^.* //'`
      for ltp_check_version in $ltp_version_list; do
        if test "$ltp_version" = "$ltp_check_version"; then
          glib_cv_ltp_version="$ltp_check_version (ok)"
        fi
      done
    ])
  else
    ltp_msg="To enable code coverage reporting you must have one of the following LTP versions installed: $ltp_version_list"
    AC_MSG_ERROR([$ltp_msg])
  fi

  case $glib_cv_ltp_version in
    ""|invalid[)]
      ltp_msg="You must have one of the following versions of LTP: $ltp_version_list (found: $ltp_version)."
      AC_MSG_ERROR([$ltp_msg])
      LTP="exit 0;"
      ;;
  esac

  if test -z "$LTP_GENHTML"; then
    AC_MSG_ERROR([Could not find genhtml from the LTP package])
  fi

  AC_DEFINE(HAVE_GCOV, 1, [Whether you have gcov])

  dnl Remove all optimization flags from CFLAGS
  changequote({,})
  CFLAGS=`echo "$CFLAGS" | $SED -e 's/-O[0-9]*//g'`
  changequote([,])

  dnl Add the special gcc flags
  CFLAGS="$CFLAGS -O0 -fprofile-arcs -ftest-coverage"
  LDFLAGS="$LDFLAGS -lgcov"
fi

dnl ================================================================
dnl Misc
dnl ================================================================
AC_PATH_PROG(GLIB_MKENUMS, glib-mkenums)

GLIB_COMPILE_RESOURCES=`$PKG_CONFIG --variable glib_compile_resources gio-2.0`
AC_SUBST([GLIB_COMPILE_RESOURCES])

PKG_CHECK_MODULES(GNOME_TODO_TEST,
                  glib-2.0 >= 2.43.4)

PKG_CHECK_MODULES(GNOME_TODO,
                  gmodule-export-2.0
                  gio-2.0 >= 2.43.4
                  glib-2.0 >= 2.43.4
                  goa-1.0 >= 3.2.0
                  gtk+-3.0 >= 3.22.0
                  libecal-1.2 >= 3.13.90
                  libedataserver-1.2 >= 3.17.1
                  libedataserverui-1.2 >= 3.17.1
                  libical >= 0.43
                  libpeas-1.0 >= 1.17)

APPSTREAM_XML


dnl ================================================================
dnl Plugins
dnl ================================================================
GNOME_TODO_ADD_PLUGIN([eds], [Evolution-Data-Server], [yes])
GNOME_TODO_ADD_PLUGIN([dark-theme], [Dark theme], [yes])
GNOME_TODO_ADD_PLUGIN([scheduled-panel], [Scheduled Panel], [yes])
GNOME_TODO_ADD_PLUGIN([score], [Score], [yes])
GNOME_TODO_ADD_PLUGIN([today-panel], [Today Panel], [yes])
GNOME_TODO_ADD_PLUGIN([unscheduled-panel], [Unscheduled Tasks Panel], [yes])
GNOME_TODO_ADD_PLUGIN([todo-txt], [Todo.Txt], [yes])


dnl ***********************************************************************
dnl Support for gtk-doc Documentation Engine
dnl ***********************************************************************
GTK_DOC_CHECK([1.14],[--flavour no-tmpl])
AM_CONDITIONAL(ENABLE_GTK_DOC, test "x$enable_gtk_doc" = "xyes")
AC_ARG_ENABLE(doc-cross-references,
              AS_HELP_STRING([--disable-doc-cross-references],
                             [cross reference symbols from other libraries @<:@default=yes@:>@]),
              enable_doc_cross_references=$enableval,
              enable_doc_cross_references=yes)
AM_CONDITIONAL(ENABLE_DOC_CROSS_REFERENCES, test x$enable_doc_cross_references != xno)


AC_CONFIG_FILES([
      Makefile
      src/Makefile
      tests/Makefile
      data/Makefile
      data/icons/Makefile
      data/gnome-todo.pc
      data/org.gnome.Todo.desktop.in
      doc/Makefile
      doc/reference/Makefile
      doc/reference/version.xml
      plugins/Makefile
      po/Makefile.in
])

AC_OUTPUT

dnl ==========================================================================
echo "

        gnome-todo $VERSION
        ==================

        prefix:          ${prefix}
        compiler:        ${CC}
        global flags:    ${CFLAGS} ${CPPFLAGS} ${LDFLAGS}
        release:         ${ax_is_release}
        documentation:   ${enable_gtk_doc}
        build Test Suite ${enable_tests}
        Installed tests  ${enable_installed_tests}


    Plugins:
        Dark theme .............. ${enable_dark_theme_plugin}
        Scheduled panel ......... ${enable_scheduled_panel_plugin}
        Score ................... ${enable_score_plugin}
        Today panel ............. ${enable_today_panel_plugin}
        Unscheduled panel ....... ${enable_unscheduled_panel_plugin}
        Todo.txt ................ ${enable_todo_txt_plugin}

        Now type 'make' to build $PACKAGE
"
