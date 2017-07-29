AC_INIT([appindicator-collection], [0.9.0])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall -Wno-portability])
AC_PREREQ([2.61])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_HOST

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

enabled=
AC_DEFUN([AI_ENABLE], [
	AC_ARG_ENABLE([$1], AC_HELP_STRING([--enable-$1],
		[Enable $1 support]), [], [enable_$1=no])
	if test x"$enable_$1" == x"yes"; then
		enabled="$enabled $1"
		AM_CONDITIONAL([ENABLE_$2], [true])
	else
		AM_CONDITIONAL([ENABLE_$2], [false])
	fi
])

AI_ENABLE(electron, ELECTRON)
AI_ENABLE(hexchat, HEXCHAT)
AI_ENABLE(telegram, TELEGRAM)

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

PKG_CHECK_MODULES(APP_INDICATOR_GTK2, appindicator-0.1)
PKG_CHECK_MODULES(APP_INDICATOR_GTK3, appindicator3-0.1)
AC_SUBST(APP_INDICATOR_GTK2_CFLAGS)
AC_SUBST(APP_INDICATOR_GTK3_CFLAGS)
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
echo "prefix: $prefix"
echo "indicators:$enabled"
echo
