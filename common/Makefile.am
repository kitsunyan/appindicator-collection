AM_CPPFLAGS = \
	-I${top_srcdir} \
	$(PLATFORM_CPPFLAGS)

noinst_LTLIBRARIES = \
	libcommon.la \
	libappindicator-gtk2.la \
	libappindicator-gtk3.la

libcommon_la_SOURCES = \
	common.c \
	common.h

libcommon_la_LIBADD = \
	-ldl

libcommon_la_LDFLAGS = \
	-no-undefined \
	$(PLATFORM_LDFLAGS)

libappindicator_gtk2_la_SOURCES = \
	app-indicator.c \
	app-indicator.h

libappindicator_gtk2_la_CFLAGS = \
	$(APP_INDICATOR_GTK2_CFLAGS)

libappindicator_gtk2_la_LIBADD = \
	$(APP_INDICATOR_GTK2_LIBS)

libappindicator_gtk2_la_LDFLAGS = \
	-no-undefined \
	$(PLATFORM_LDFLAGS)

libappindicator_gtk3_la_SOURCES = \
	app-indicator.c \
	app-indicator.h

libappindicator_gtk3_la_CFLAGS = \
	$(APP_INDICATOR_GTK3_CFLAGS)

libappindicator_gtk3_la_LIBADD = \
	$(APP_INDICATOR_GTK3_LIBS)

libappindicator_gtk3_la_LDFLAGS = \
	-no-undefined \
	$(PLATFORM_LDFLAGS)
