AC_INIT([appindicator-collection], [0.9.0])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall -Wno-portability])
AC_PREREQ([2.61])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_HOST

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_ARG_ENABLE([all], AC_HELP_STRING([--enable-all], [Enable support for all indicators]))

enabled=
require_gtk2=no
require_gtk3=no
AC_DEFUN([AI_SET_ENABLED], [
	if $4; then
		enabled="$enabled $1"
		case $3 in
			2) require_gtk2=yes ;;
			3) require_gtk3=yes ;;
		esac
	fi
	AM_CONDITIONAL([ENABLE_$2], [$4])
])

AC_DEFUN([AI_ENABLE], [
	AC_ARG_ENABLE([$1], AC_HELP_STRING([--enable-$1], [Enable $1 support]), [
		if test x"$enableval" = x"yes"; then
			AI_SET_ENABLED($1, $2, $3, true)
		else
			AI_SET_ENABLED($1, $2, $3, false)
		fi
	], [
		if test x"$enable_all" = x"yes"; then
			AI_SET_ENABLED($1, $2, $3, true)
		else
			AI_SET_ENABLED($1, $2, $3, false)
		fi
	])
])

AI_ENABLE(activate-gtk2, ACTIVATE_GTK2, 2)
AI_ENABLE(activate-gtk3, ACTIVATE_GTK3, 3)
AI_ENABLE(electron-gtk2, ELECTRON_GTK2, 2)
AI_ENABLE(electron-gtk3, ELECTRON_GTK3, 3)
AI_ENABLE(gajim, GAJIM, 2)
AI_ENABLE(hexchat, HEXCHAT, 2)
AI_ENABLE(systray-gtk2, SYSTRAY_GTK2, 2)
AI_ENABLE(systray-gtk3, SYSTRAY_GTK3, 3)
AI_ENABLE(telegram, TELEGRAM, 3)

if test -z "$enabled"; then
	echo "You haven't enabled any indicators. Skipping..."
	exit 1
fi

AC_PREFIX_DEFAULT(/usr/local)
AC_USE_SYSTEM_EXTENSIONS

LT_INIT([disable-static])
AC_ENABLE_STATIC([no])
AC_ENABLE_SHARED([yes])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_ARG_WITH([activate], AS_HELP_STRING([--with-activate], [Enable appindicator left-click activation]))
if test x"$with_activate" = x"yes"; then
	AC_DEFINE([WITH_ACTIVATE], [], [Enable appindicator left-click activation])
else
	with_activate=no
fi

PKG_CHECK_MODULES(GLIB2, glib-2.0)
AM_CONDITIONAL([ENABLE_GTK2], [test x"$require_gtk2" = x"yes"])
AM_CONDITIONAL([ENABLE_GTK3], [test x"$require_gtk3" = x"yes"])
AM_COND_IF([ENABLE_GTK2], [PKG_CHECK_MODULES(APP_INDICATOR_GTK2, appindicator-0.1)])
AM_COND_IF([ENABLE_GTK3], [PKG_CHECK_MODULES(APP_INDICATOR_GTK3, appindicator3-0.1)])
AC_SUBST(GLIB2_CFLAGS)
AC_SUBST(APP_INDICATOR_GTK2_CFLAGS)
AC_SUBST(APP_INDICATOR_GTK3_CFLAGS)
AC_SUBST(GLIB2_LIBS)
AC_SUBST(APP_INDICATOR_GTK2_LIBS)
AC_SUBST(APP_INDICATOR_GTK3_LIBS)

AC_HEADER_STDC

AC_CONFIG_FILES([
	Makefile
	common/Makefile
	indicators/Makefile
])
AC_OUTPUT

echo
echo "gtk+2: $require_gtk2"
echo "gtk+3: $require_gtk3"
echo "indicators:$enabled"
echo "activate: $with_activate"
echo
